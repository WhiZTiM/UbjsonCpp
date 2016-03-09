/*
 * Copyright(C):    WhiZTiM, 2015
 *
 * This file is part of the TIML::UBJSON C++14 library
 *
 * Distributed under the Boost Software License, Version 1.0.
 *      (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * Author: Ibrahim Timothy Onogu
 * Email:  ionogu@acm.org
 */

#ifndef STREAM_READER_HPP
#define STREAM_READER_HPP

#include "stream_helpers.hpp"
#include "value.hpp"
#include <fstream>
#include <cstring>
#include <tuple>
#include <iostream>

namespace ubjson {

    enum class MarkerType { Object, Array };

    constexpr ValueSizePolicy defaultStreamReaderPolicy()
    { return {32, 1024*1024*64, 1024*1024*8, 1024*1024*65, 1024, 1024}; }

    template<typename StreamType>
    class StreamReader
    {
    public:

        struct policy_violation : parsing_exception
        {
            policy_violation(const std::string& str) : parsing_exception(""), ss(str) {}
            const char* what() const noexcept { return ss.c_str(); }
            std::string ss;
        };


        StreamReader(StreamType& Stream, ValueSizePolicy policy = defaultStreamReaderPolicy());

        Value getNextValue();

        bool getNextValue(Value& v);

        StreamType& getStream() { return stream; }

        std::size_t getBytesRead() const { return bytes_so_far; }

        std::string getLastError() const { return last_error; }

    private:
        void extract_nextValue(Value &vref, MarkerType type = MarkerType::Object, STCHeader header = STCHeader{});

        KeyMarker extract_nextKeyMarker();

        std::pair<std::size_t, bool> extract_itemCount();
        std::pair<int8_t, bool> extract_Int8();
        std::pair<uint8_t, bool> extract_Uint8();
        std::pair<int16_t, bool> extract_Int16();
        std::pair<int32_t, bool> extract_Int32();
        std::pair<int64_t, bool> extract_Int64();
        std::pair<long long, bool> extract_Integer(bool read_uint8 = true);
        std::pair<float, bool> extract_Float32();
        std::pair<double, bool> extract_Float64();
        std::pair<std::string, bool> extract_String();
        std::pair<Value::BinaryType, bool> extract_Binary();

        void extract_Object(Value& v);
        void extract_Array(Value& v);
        void extract_singleValueTo(byte marker, Value& value);
        void extract_containerValueTo(byte marker, Value& value);
        bool is_container_end(MarkerType type, byte b);

        STCHeader extract_optimized_container_headers();

        byte peekNextByte();
        byte readNextByte();
        bool read(byte&);
        bool read(byte*, std::size_t);


        //SFINAE zone :-)
        template<typename U = StreamType>
        std::enable_if_t<std::is_base_of<std::istream, U>::value, bool> read_from_stream(byte*, std::size_t, bool peek = false);

        template<typename U = StreamType>
        std::enable_if_t<not std::is_base_of<std::istream, U>::value, bool> read_from_stream(byte*, std::size_t, bool peek = false);
        //decltype(std::declval<U>().peek(), std::true_type()()) read_from_stream(byte*, std::size_t);

        std::pair<byte, bool> peeked_byte;

        StreamType& stream;
        std::string last_error;
        std::size_t bytes_so_far = 0;    //! bytes so far
        size_t recursive_depth = 0;
        const ValueSizePolicy vsz;
    };

    template<typename StreamType>
    StreamReader<StreamType>::StreamReader(StreamType& Stream, ValueSizePolicy policy)
        : stream(Stream), vsz(policy)
    {}


    template<typename StreamType>
    Value StreamReader<StreamType>::getNextValue()
    {
        Value v;        getNextValue(v);        return v;
    }

    template<typename StreamType>
    bool StreamReader<StreamType>::getNextValue(Value& v)
    {
        bool good = false;

        try
        {
            bytes_so_far = 0;
            recursive_depth = 0;
            peeked_byte.second = false;
            byte b = readNextByte();
            extract_singleValueTo(b, v);
            extract_containerValueTo(b, v);
            good = true;
        }
        catch(parsing_exception& pexecpt)
        {
            last_error = pexecpt.what();
        }
        return good;
    }

    template<typename StreamType>
    byte StreamReader<StreamType>::readNextByte()
    {
        byte b;
        read(b);
        return b;
    }

    template<typename StreamType>
    bool StreamReader<StreamType>::read(byte& b)
    {
        read(&b, 1);
        return true;
    }

    template<typename StreamType>
    bool StreamReader<StreamType>::read(byte* b, std::size_t sz)
    {
        return read_from_stream<StreamType>(b, sz);
    }

    template<typename StreamType>
    template<typename U> std::enable_if_t<std::is_base_of<std::istream, U>::value, bool>
    StreamReader<StreamType>::read_from_stream(byte* b, std::size_t sz, bool peek)
    {
        using std::to_string;
        using std::string;

        if(bytes_so_far + sz > vsz.max_object_size)
            throw policy_violation("Maximum Object size read at: " + to_string(bytes_so_far));

        if(peek)
            b[0] = static_cast<byte>(stream.peek());
        else
        {
            stream.read(to_cbyte(b), sz);
            bytes_so_far += sz;
        }
        peeked_byte.second = false;
        return true;
    }


    template<typename StreamType>
    template<typename U> std::enable_if_t<not std::is_base_of<std::istream, U>::value, bool>
    StreamReader<StreamType>::read_from_stream(byte* b, std::size_t sz, bool peek)
    {
        using std::to_string;
        using std::string;

        if(sz > 0)
        {
            if(bytes_so_far + sz > vsz.max_object_size)
                throw policy_violation("Maximum Object size read at: " + to_string(bytes_so_far));

            if(peek and peeked_byte.second)
            {
                b[0] = peeked_byte.first;
                b = b + 1;
                    --sz;
            }
            else if (peek)
                peeked_byte.second = true;
            else
                peeked_byte.second = false;

            stream.read(to_cbyte(b), sz);
            bytes_so_far += sz;
        }
        return true;
    }


    template<typename StreamType>
    byte StreamReader<StreamType>::peekNextByte()
    {
        read_from_stream<StreamType>(&peeked_byte.first, 1, true);
        return peeked_byte.first;
    }

    template<typename StreamType>
    void StreamReader<StreamType>::extract_nextValue(Value& vref, MarkerType type, STCHeader header)
    {
        if(++recursive_depth > vsz.max_value_depth)
            throw parsing_exception("Maximum Parsing depth Exceeded!");

        decltype(KeyMarker::marker) marker;
        KeyMarker km;
        while ( (not header.is_valid) or (header.is_valid and header.item_count > 0)) {

            if(not header.is_valid and is_container_end(type, peekNextByte()))
            {
                readNextByte(); //peel it off the stream
                break;
            }

            //Edge case... I honestly find this a bit akward
            //
            //
            // N/A

            Value value;
            switch (type) {
            case MarkerType::Object:
                km.key = extract_String().first;  //extract key
                break;
            case MarkerType::Array:
            default:
                break;
            }

            if(header.marker != Marker::Invalid and header.is_valid)
                marker = km.marker;
            else
                marker = readNextByte();


            extract_singleValueTo(marker, value);
            extract_containerValueTo(marker, value);

            switch (type) {
            case MarkerType::Object:
                vref[km.key] = std::move(value);
                break;
            case MarkerType::Array:
                vref.push_back( std::move(value) );
                break;
            }
            --header.item_count;
        }
        --recursive_depth;
    }


    template<typename StreamType>
    void StreamReader<StreamType>::extract_singleValueTo(byte marker, Value& value)
    {
        if(isNull(marker))
        {
            value = Value();
        }
        else if(isTrue(marker))
        {
            value = true;
        }
        else if(isFalse(marker))
        {
            value = false;
        }
        else if(isChar(marker))
        {
            uint8_t extracted(extract_Uint8().first);
            char val(extracted);
            value = val;
        }
        else if(isUint8(marker))
        {
            uint8_t extracted(extract_Uint8().first);
            unsigned long long val(extracted);
            value = val;
        }
        else if(isInt8(marker))
        {
            int8_t extracted(extract_Uint8().first);
            long long val(extracted);
            value = val;
        }
        else if(isInt16(marker))
        {
            int16_t extracted(extract_Int16().first);
            int val(extracted);
            value = val;
        }
        else if(isInt32(marker))
        {
            int32_t extracted(extract_Int32().first);
            int val(extracted);
            value = val;
        }
        else if(isInt64(marker))
        {
            int64_t extracted(extract_Int64().first);
            long long val(extracted);
            value = val;
        }
        else if(isFloat32(marker))
        {
            float extracted(extract_Float32().first);
            double val(extracted);
            value = val;
        }
        else if(isFloat64(marker))
        {
            double extracted(extract_Float64().first);
            double val(extracted);
            value = val;
        }
        else if(isString(marker))
        {
            value = extract_String().first;
        }
        else if(isBinary(marker))
        {
            value = extract_Binary().first;
        }
    }

    template<typename StreamType>
    void StreamReader<StreamType>::extract_containerValueTo(byte marker, Value &value)
    {
        if(isObjectStart(marker))
            extract_Object(value);
        else if(isArrayStart(marker))
            extract_Array(value);
    }

    template<typename StreamType>
    std::pair<std::size_t, bool> StreamReader<StreamType>::extract_itemCount()
    {
        auto rtn = extract_Integer(false);  //extract without considering Uint8 type
        return std::make_pair(static_cast<size_t>(rtn.first), rtn.second);
    }

    template<typename StreamType>
    void StreamReader<StreamType>::extract_Object(Value& v)
    {
        STCHeader header;
        byte b = peekNextByte();
        if(isOptimizedMarker(b))
            header = extract_optimized_container_headers();

        extract_nextValue(v, MarkerType::Object, header);
    }

    template<typename StreamType>
    void StreamReader<StreamType>::extract_Array(Value& v)
    {
        STCHeader header;
        byte b = peekNextByte();
        if(isOptimizedMarker(b))
            header = extract_optimized_container_headers();

        extract_nextValue(v, MarkerType::Array, header);
    }

    template<typename StreamType>
    bool StreamReader<StreamType>::is_container_end(MarkerType type, byte b)
    {
        switch (type) {
        case MarkerType::Object:
            if(isObjectEnd(b))
                return true;
            break;
        case MarkerType::Array:
            if(isArrayEnd(b))
                return true;
        default:
            break;
        }
        return false;
    }



    template<typename StreamType>
    std::pair<int16_t, bool> StreamReader<StreamType>::extract_Int16()
    {
        byte b[2];
        read(b, 2);
        return std::make_pair(fromBigEndian16(b), true);
    }

    template<typename StreamType>
    std::pair<int32_t, bool> StreamReader<StreamType>::extract_Int32()
    {
        byte b[4];
        read(b, 4);
        return std::make_pair(fromBigEndian32(b), true);
    }

    template<typename StreamType>
    std::pair<int64_t, bool> StreamReader<StreamType>::extract_Int64()
    {
        byte b[8];
        read(b, 8);
        return std::make_pair(fromBigEndian64(b), true);
    }

    template<typename StreamType>
    std::pair<long long, bool> StreamReader<StreamType>::extract_Integer(bool read_uint8)
    {
        byte b = readNextByte();

        if(isUint8(b) and read_uint8)
        {
            auto rtn = extract_Uint8();
            return std::make_pair(rtn.first, rtn.second);
        }
        else if(isInt8(b))
        {
            auto rtn = extract_Int8();
            return std::make_pair(rtn.first, rtn.second);
        }
        else if(isInt16(b))
        {
            auto rtn = extract_Int16();
            return std::make_pair(rtn.first, rtn.second);
        }
        else if(isInt32(b))
        {
            auto rtn = extract_Int32();
            return std::make_pair(rtn.first, rtn.second);
        }
        else if(isInt64(b))
        {
            return extract_Int64();
        }

        return std::make_pair(0, false);

    }

    template<typename StreamType>
    std::pair<int8_t, bool> StreamReader<StreamType>::extract_Int8()
    {
        auto m = extract_Uint8();
        return std::make_pair(reinterpret_cast<int8_t&>(m.first), m.second);
    }

    template<typename StreamType>
    std::pair<uint8_t, bool> StreamReader<StreamType>::extract_Uint8()
    {
        byte b = readNextByte();
        return std::make_pair(fromBigEndian8(&b), true);
    }

    template<typename StreamType>
    std::pair<float, bool> StreamReader<StreamType>::extract_Float32()
    {
        byte b[4];
        read(b, 4);
        return std::make_pair(fromBigEndianFloat32(b), true);
    }

    template<typename StreamType>
    std::pair<double, bool> StreamReader<StreamType>::extract_Float64()
    {
        byte b[8];
        read(b, 8);
        return std::make_pair(fromBigEndianFloat64(b), true);
    }

    template<typename StreamType>
    std::pair<std::string, bool> StreamReader<StreamType>::extract_String()
    {
        auto icount = extract_itemCount();
        if(not icount.second)
            return std::make_pair(std::string(), false);

        std::unique_ptr<byte[]> b(new byte[icount.first]);
        read(b.get(), icount.first);

        std::string rtn(to_cbyte(b.get()), to_cbyte(b.get()) + icount.first);
        return std::make_pair(rtn, true);
    }

    template<typename StreamType>
    std::pair<Value::BinaryType, bool> StreamReader<StreamType>::extract_Binary()
    {
        auto icount = extract_itemCount();
        if(not icount.second)
            throw parsing_exception("Invalid count token encounted!");

        std::unique_ptr<byte[]> b(new byte[icount.first]);
        read(b.get(), icount.first);

        Value::BinaryType rtn(to_cbyte(b.get()), to_cbyte(b.get()) + icount.first);
        return std::make_pair(rtn, true);
    }

    /*!
     *
     * \pre The container starting marker has been extracted off the stream
     * \post All headers are completely parsed
     */
    template<typename StreamType>
    STCHeader StreamReader<StreamType>::extract_optimized_container_headers()
    {
        STCHeader header;
        byte b = readNextByte();

        auto get_optimized_count = [&]() -> void      //Local function... for convenience
                {
                    if(isOptimized_Count(b))
                    {
                        b = peekNextByte();
                        if(isSignedNumber(b))   //Directly objects
                        {
                            auto sz = extract_Integer(false);  //extract size...
                            header.item_count = sz.first;
                            header.is_valid = sz.second;
                        }
                    }
                };

        //! \see "spec for draft 10"

        get_optimized_count();  //if its only count, retrieve it
        if(header.is_valid)    // ..and return if we've successfully retrieved it
            return header;
        else if(isOptimized_Type(b))    //If is type
        {
            header.marker = static_cast<Marker>(readNextByte());  //extract the type
            get_optimized_count();              //extract count..
        }

        return header;
    }

    using OstreamReader = StreamReader<std::ifstream>;

}   //end namespace ubjson
#endif // STREAM_READER_HPP

