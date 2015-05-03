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

/**
  * @file types.hpp
  * basic helper funtions and definition of types and Type enumeration
  *
  * @brief types helper
  * @author WhiZTiM
  * @date January, 2015
  * @version 0.0.1
  *
  */

#ifndef TYPES_HPP
#define TYPES_HPP

namespace ubjson {

    using byte = unsigned char;

    /*!
     * \enum Type
     * \brief The Type enum
     */
    enum class Type
    {
        Null,
        Char,
        Bool,
        SignedInt,
        UnsignedInt,
        Float,
        Map,
        Array,
        Binary,
        String
    };

    enum class Marker : byte
    {
        Invalid = '\0',

        Null    = 'Z',
        No_Op   = 'N',
        Char    = 'C',
        True    = 'T',
        False   = 'F',

        Int8    = 'i',
        Uint8   = 'U',
        Int16   = 'I',
        Int32   = 'l',
        Int64   = 'L',
        Float32 = 'd',
        Float64 = 'D',
        HighPrecision = 'H',

        String  = 'S',
        Binary  = 'b',  //Extension

        Object_Start   = '{',
        Object_End     = '}',
        Array_Start    = '[',
        Array_End      = ']',

        Array,
        Object,

        Optimized_Type  = '$',
        Optimized_Count = '#'
    };

    constexpr bool operator == (byte b, Marker m) { return b == static_cast<byte>(m); }

    constexpr bool isNo_Op(byte b){ return b == Marker::No_Op;}
    constexpr bool isNull(byte b) { return b == Marker::Null; }
    constexpr bool isChar(byte b) { return b == Marker::Char; }
    constexpr bool isTrue(byte b) { return b == Marker::True; }
    constexpr bool isFalse(byte b) { return b == Marker::False; }
    constexpr bool isInt8(byte b) { return b == Marker::Int8; }
    constexpr bool isInt16(byte b) { return b == Marker::Int16; }
    constexpr bool isInt32(byte b) { return b == Marker::Int32; }
    constexpr bool isInt64(byte b) { return b == Marker::Int64; }
    constexpr bool isUint8(byte b) { return b == Marker::Uint8; }
    constexpr bool isFloat32(byte b) { return b == Marker::Float32; }
    constexpr bool isFloat64(byte b) { return b == Marker::Float64; }
    constexpr bool isString(byte b) { return b == Marker::String; }
    constexpr bool isBinary(byte b) { return b == Marker::Binary; }
    constexpr bool isHighPrecision(byte b) { return b == Marker::HighPrecision; }


    constexpr bool isSignedNumber(byte b)
    { return isInt16(b) or isInt32(b) or isInt64(b);  }

    constexpr bool isNumber(byte b)
    { return isSignedNumber(b);  }

    constexpr bool isNone(byte b)
    { return isNull(b); }

    constexpr bool isObjectStart(byte b)
    { return b == Marker::Object_Start;  }

    constexpr bool isObjectEnd(byte b)
    { return b == Marker::Object_End;  }

    constexpr bool isArrayStart(byte b)
    { return b == Marker::Array_Start;  }

    constexpr bool isArrayEnd(byte b)
    { return b == Marker::Array_End;  }

    constexpr bool isOptimized_Type(byte b)
    { return b == Marker::Optimized_Type;  }

    constexpr bool isOptimized_Count(byte b)
    { return b == Marker::Optimized_Count; }

    constexpr bool isOptimizedMarker(byte b)
    { return isOptimized_Count(b) or isOptimized_Type(b); }

    constexpr bool requiresPayload(byte b)
    { return isObjectStart(b) or isString(b) or isBinary(b) or isArrayStart(b); }

    static_assert(sizeof(byte) == 1, "a byte must be exactly one byte(8 bits)");

}   //end namespace ubjson

#endif // TYPES_HPP

