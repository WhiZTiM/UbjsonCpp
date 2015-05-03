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

#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include <iterator>
#include "types.hpp"

namespace ubjson {


template<typename Value_Type,
         typename Array_IteratorType,
         typename Map_IteratorType>
class value_iterator :
        public std::iterator<std::forward_iterator_tag, Value_Type>
{
public:

    enum class pos { begin, end };
    value_iterator(Value_Type* Parent, pos p)
        : parent(Parent)
    {
        switch (parent->vtype) {
        case Type::Array:
            arr_iter = (p == pos::begin) ? parent->value.Array.begin() : parent->value.Array.end();
            break;
        case Type::Map:
            map_iter = (p == pos::begin) ? parent->value.Map.begin() : parent->value.Map.end();
            break;
        default:
            break;
        }
    }

    value_iterator(Value_Type* Parent, Array_IteratorType Array_iter)
        : parent(Parent), arr_iter(Array_iter)
    {  }

    value_iterator(Value_Type* Parent, Map_IteratorType Map_iter)
        : parent(Parent), map_iter(Map_iter)
    {  }

    Value_Type& operator * () const
    {   return *operator->(); }

    Value_Type* operator -> () const
    {
        switch (parent->vtype) {
        case Type::Array:
            return arr_iter->get();
        case Type::Map:
            return map_iter->second.get();
        default:
            break;
        }
        return parent;
    }

    value_iterator& operator ++ () //prefix
    {
        switch (parent->vtype) {
        case Type::Array:
            ++arr_iter;
            break;
        case Type::Map:
            ++map_iter;
            break;
        default:
            break;
        }
        return *this;
    }

    value_iterator operator ++ (int)
    {
        auto rtn = value_iterator(*this);
        operator ++();
        return rtn;
    }

    friend bool operator == (const value_iterator& lhs, const value_iterator& rhs)
    {
        // if the iterators do not have the same parent
        if( lhs.parent != rhs.parent)
            return false;

        switch (lhs.parent->type()) {
        case Type::Array:
            return lhs.arr_iter == rhs.arr_iter;
        case Type::Map:
            return lhs.map_iter == rhs.map_iter;
        default:
            break;
        }

        return true;
    }

    friend bool operator != (const value_iterator& lhs, const value_iterator& rhs)
    {   return  !(lhs == rhs);   }

private:
    Value_Type* parent;
    Map_IteratorType map_iter;
    Array_IteratorType arr_iter;
};

}

#endif // ITERATOR_HPP

