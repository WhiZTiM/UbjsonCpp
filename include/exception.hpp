/*
 * Copyright(C):    WhiZTiM, 2015
 *
 * This file is part of the TIML::UBEX C++14 library
 *
 * Distributed under the Boost Software License, Version 1.0.
 *      (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * Author: Ibrahim Timothy Onogu
 * Email:  ionogu@acm.org
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <exception>
#include <stdexcept>

namespace timl
{
    class value_exception : public std::logic_error
    {
    public:
        value_exception(const char* msg) noexcept
            : std::logic_error(msg) {}
    };

    class bad_value_cast : public value_exception
    {
    public:
        bad_value_cast(const char* msg)
            : value_exception(msg) {}
    };

    class parsing_exception : public value_exception
    {
    public:
        parsing_exception(const char* msg)
            : value_exception(msg) {}
    };

}
#endif // EXCEPTION_H
