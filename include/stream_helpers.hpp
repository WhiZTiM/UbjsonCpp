#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <cstdint>
#include <cstring>
#include <endian.h>
#include <limits>
#include "types.hpp"

namespace ubjson {

    using cbyte = char;
    static_assert(sizeof(cbyte) == 1, "");
    static_assert(sizeof(std::uint16_t) == 2, "");
    static_assert(sizeof(std::uint32_t) == 4, "");
    static_assert(sizeof(std::uint64_t) == 8, "");
    static_assert(sizeof(float) == 4, "");
    static_assert(sizeof(double) == 8, "");

    inline namespace ubjson {
        inline byte* to_byte(cbyte* b) { return reinterpret_cast<byte*>(b); }
        inline cbyte* to_cbyte(byte* b) { return reinterpret_cast<cbyte*>(b); }

        struct KeyMarker
        {
            byte marker;        //ouch... there will be many void spaces due to alignment
            std::string key;
        };

        struct STCHeader        //For Strongly typed containers/ optimized container
        {
            bool    is_valid = false;
            bool    has_type = false;
            size_t  item_count = 0;
            Marker  marker = Marker::Invalid;
        };
    }

    inline bool in_range(double value, double min, double max)
    { return (min <= value and value <= max); }

	inline
    uint16_t toBigEndian16(uint16_t val)
    {  return htobe16(val); }

	inline
    uint32_t toBigEndian32(uint32_t val)
    {  return htobe32(val); }

	inline
    uint64_t toBigEndian64(uint64_t val)
    {  return htobe64(val); }

	inline
    uint32_t toBigEndianFloat32(float val)
    {
        uint32_t rtn;
        std::memcpy(&rtn, &val, sizeof(rtn));
        return toBigEndian32( rtn );
    }

	inline
    uint64_t toBigEndianFloat64(double val)
    {
        uint64_t rtn;
        std::memcpy(&rtn, &val, sizeof(rtn));
        return toBigEndian64( rtn );
    }



	inline
    uint16_t fromBigEndian16(uint16_t val)
    {  return be16toh(val); }

	inline
    uint32_t fromBigEndian32(uint32_t val)
    {  return be32toh(val); }

	inline
    uint64_t fromBigEndian64(uint64_t val)
    {  return be64toh(val); }

	inline
    float fromBigEndianFloat32(uint32_t val)
    {
        float rtn;
        val = fromBigEndian32(val);
        std::memcpy(&rtn, &val, sizeof(rtn));
        return rtn;
    }

	inline
    double fromBigEndianFloat64(uint64_t val)
    {
        double rtn;
        val = fromBigEndian64(val);
        std::memcpy(&rtn, &val, sizeof(rtn));
        return rtn;
    }



    ////////////////////////////////////
    ///
    ///
    ///
    ///////////////////////////////////////


	inline
    uint8_t fromBigEndian8(byte* b)
    {
        return *b;
    }

	inline
    uint16_t fromBigEndian16(byte* b)
    {
        uint16_t rtn;
        std::memcpy(&rtn, b, 2);
        return fromBigEndian16(rtn);
    }

	inline
    uint32_t fromBigEndian32(byte* b)
    {
        uint32_t rtn;
        std::memcpy(&rtn, b, 4);
        return fromBigEndian32(rtn);
    }

	inline
    uint64_t fromBigEndian64(byte* b)
    {
        uint64_t rtn;
        std::memcpy(&rtn, b, 8);
        return fromBigEndian64(rtn);
    }

	inline
    float fromBigEndianFloat32(byte* b)
    {
        float rtn;
        const uint32_t ans = fromBigEndian32(b);
        std::memcpy(&rtn, &ans, 4);
        return rtn;
    }

	inline
    double fromBigEndianFloat64(byte* b)
    {
        double rtn;
        const int64_t ans = fromBigEndian64(b);
        std::memcpy(&rtn, &ans, 8);
        return rtn;
    }

}
#endif // CONVERSIONS_HPP

