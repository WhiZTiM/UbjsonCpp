#ifndef FORMAT_HELPERS_HPP
#define FORMAT_HELPERS_HPP

#include <ostream>
#include <iomanip>
#include "value.hpp"

//for CPPUNIT_ASSERT_EQUAL ...using ADL
inline std::ostream& operator << (std::ostream& out, const ubjson::Value::BinaryType& b)
{
    out << std::hex;
    for(auto a : b)
        out << int(a) << ' ';
    return out;
}

#endif // FORMAT_HELPERS_HPP

