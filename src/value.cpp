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


#include "value.hpp"
#include <cmath>
#include <limits>
#include <cstring>
#include <algorithm>
#include <iostream>

using namespace ubjson;

/////////////////  FREE FUNCTIONS
template<typename T>
T unique_ptr_copy(const T& src);

template<>
inline Value::ArrayType unique_ptr_copy(const Value::ArrayType& src)
{
    Value::ArrayType rtn;
    for(auto& v : src)
        rtn.emplace_back( std::make_unique<Value>(*v) );
    rtn.shrink_to_fit();
    return rtn;
}

template<>
inline Value::MapType unique_ptr_copy(const Value::MapType& src)
{
    Value::MapType rtn;
    for(auto& v : src)
        rtn.emplace( std::make_pair(v.first, std::make_unique<Value>(*(v.second)) ));
    return rtn;
}

inline bool in_range(double value, double min, double max)
{ return (min <= value and value <= max); }

inline bool is_equal(const Value::MapType& lhs, const Value::MapType& rhs)
{
    if(lhs.size() != rhs.size())    //edge case
        return false;

    for(const auto& val : lhs)
    {
        if(rhs.find(val.first) == rhs.end())
            return false;
        if(not (*(lhs.at(val.first)) == *(rhs.at(val.first))))
            return false;
    }
    return true;
}

inline bool is_equal(const Value::ArrayType& lhs, const Value::ArrayType& rhs)
{
    return std::equal(lhs.begin(), lhs.end(),
                      rhs.begin(), rhs.end(),
                      []( const Value::Uptr& lhss, const Value::Uptr& rhss )
                        { return *lhss == *rhss; }
                     );
}

//////////////// VALUE IMpl


Value::Value()
    : vtype(Type::Null)
{ }

Value::Value(char c)
    : vtype(Type::Char)
{  value.Char = c; }

Value::Value(bool b)
    : vtype(Type::Bool)
{  value.Bool = b; }

Value::Value(long long ll)
    : vtype(Type::SignedInt)
{  value.SignedInt = ll; }

Value::Value(int i)
    : Value(static_cast<long long>(i))
{  /**/ }

Value::Value(unsigned long long ull)
    : vtype(Type::UnsignedInt)
{  value.UnsignedInt = ull; }

Value::Value(double d)
    : vtype(Type::Float)
{  value.Float = d; }

Value::Value(BinaryType b)
    : vtype(Type::Binary)
{   construct_fromBinary(std::move(b)); }

Value::Value(std::string s)
    : vtype(Type::String)
{   construct_fromString(std::move(s)); }

Value::Value(const char* c)
    : Value(std::string(c))
{  /**/  }

Value::Value(std::initializer_list<Value> v)
    : Value()   //I suffered a setback here... explanation below
{
    if(v.size() == 1)
    {
        copy_from(*v.begin());
        return;
    }
    for(auto a : v)
        push_back( std::move(a) );
// The bug was the fact that I didn't call the default constructor( basically initializes vtype=Type::Null)
// Thus, the move was failing in optimized builds
// Lesson: learn to use in-class default construction to maintain a first-class construction invariant
}

Value::Value(const std::string& key, Value val)
    : Value()
{
    operator[](key) = val;
}


Value::Value(Value&& v)
    : Value()
{   move_from(std::move(v)); }

Value::Value(const Value& v)
    : Value()
{   copy_from(v); }


Value& Value::operator = (const Value& v)
{
    if(this == &v)
        return *this;
    copy_from(v);
    return *this;
}

Value& Value::operator = (Value&& v)
{
    move_from(std::move(v));
    return *this;
}


Value::~Value()
{
    destruct();
}

size_t Value::size() const noexcept
{
    switch (vtype) {
    case Type::Null:
        return 0;
    case Type::Array:
        return value.Array.size();
    case Type::Map:
        return value.Map.size();
    default:
        return 1;
    }
}

Type Value::type() const noexcept
{ return vtype; }

Value& Value::operator [] (int i)
{
    if(vtype == Type::Array)
        return *(value.Array[i]);
    throw value_exception("Attempt to index 'Value'; 'Value' is not an Array!");
}

Value const& Value::operator [] (int i) const
{
    if(vtype == Type::Array)
        return *(value.Array[i]);
    throw value_exception("Attempt to index 'Value const&'; 'Value const&' is not an Array!");
}

Value& Value::operator [] (const std::string& s)
{
    if(vtype == Type::Map)
    {
        if(value.Map.find(s) == value.Map.end())
            value.Map.emplace( std::make_pair(s, std::make_unique<Value>(Value())));
        return *(value.Map[s]);
    }
    if(vtype == Type::Null)
    {
        // convert to Map
        destruct();
        construct_fromMap(MapType());
        vtype = Type::Map;
        value.Map.emplace( std::make_pair(s, std::make_unique<Value>(Value())));
        return *(value.Map[s]);
    }
    throw value_exception("Attempt to index 'Value'; 'Value' is not a Key-Value pair (aka Object) !");
}

Value const& Value::operator [] (const std::string& s) const
{
    if(vtype == Type::Map)
        return *(const_cast<const MapType&>(value.Map).at(s));
    throw value_exception("Attempt to index 'Value const&'; 'Value const&' is not a Key-Value pair (aka Object) !");
}

Value& Value::operator [] (const char* c)
{ return operator [] (std::string(c)); }

Value const& Value::operator [] (const char* c) const
{ return operator [] (std::string(c)); }

void Value::push_back(Value&& v)
{
    switch (vtype) {
    case Type::Null:
        construct_fromArray(ArrayType());
        vtype = Type::Array;
    case Type::Array:
        value.Array.emplace_back( std::make_unique<Value>( std::move(v) ) );
        break;
    default:
    {
        Value tmp(std::move(*this));
        construct_fromArray(ArrayType());
        value.Array.emplace_back(std::make_unique<Value>( std::move(tmp) ));
        value.Array.emplace_back(std::make_unique<Value>( std::move(v) ));
        vtype = Type::Array;
        break;
    }

    }
}

void Value::push_back(const Value& v)
{
    switch (vtype) {
    case Type::Null:
        construct_fromArray(ArrayType());
        vtype = Type::Array;
    case Type::Array:
        value.Array.emplace_back( std::make_unique<Value>(v) );
        break;
    default:
    {
        Value tmp(std::move(*this));
        construct_fromArray(ArrayType());
        value.Array.emplace_back(std::make_unique<Value>( std::move(tmp) ));
        value.Array.emplace_back(std::make_unique<Value>( v ));
        vtype = Type::Array;
        break;
    }

    }
}

void Value::remove(const Value& v)
{
    switch (vtype) {
    case Type::Array:
    {
        auto it = std::find_if(value.Array.begin(), value.Array.end(),
                     [&v](const auto& m){ return v == *m; } );
        if(it != value.Array.end() )
            value.Array.erase(it);
        break;
    }
    case Type::Map:
        value.Map.erase(v.asString());
    default:
        break;
    }
}

Value::iterator Value::find(const Value& v)
{
    switch (vtype) {
    case Type::Array:
    {
        auto it = std::find_if(value.Array.begin(), value.Array.end(),
                     [&v](const auto& m){ return v == *m; } );
        if(it == value.Array.end() )
            return end();
        return iterator(this, it);
    }
    case Type::Map:
    {
        auto it = value.Map.find(v.asString());
        if(it == value.Map.end())
            return end();
        return iterator(this, it);
    }
    default:
        break;
    }
    return end();
}


Value::const_iterator Value::find(const Value& v) const
{
    switch (vtype) {
    case Type::Array:
    {
        auto it = std::find_if(value.Array.begin(), value.Array.end(),
                     [&v](const auto& m){ return v == *m; } );
        if(it == value.Array.end() )
            return end();
        return const_iterator(this, it);
    }
    case Type::Map:
    {
        auto it = value.Map.find(v);
        if(it == value.Map.end())
            return end();
        return const_iterator(this, it);
    }
    default:
        break;
    }
    return end();
}

bool Value::contains(const Value& v) const
{ return find(v) != end(); }

Value::Keys Value::keys() const
{
    if(not isMap())
        return Keys();

    Keys rtn;
    for(const auto& k : value.Map)
        rtn.push_back( k.first );
    return rtn;
}

bool Value::isNull()    const noexcept { return vtype == Type::Null;   }
bool Value::isArray()   const noexcept { return vtype == Type::Array;  }
bool Value::isBinary()  const noexcept { return vtype == Type::Binary; }
bool Value::isBool()    const noexcept { return vtype == Type::Bool;   }
bool Value::isChar()    const noexcept { return vtype == Type::Char;   }
bool Value::isFloat()   const noexcept { return vtype == Type::Float;  }
bool Value::isMap()     const noexcept { return vtype == Type::Map;    }
bool Value::isString()  const noexcept { return vtype == Type::String; }
bool Value::isSignedInteger()   const noexcept { return vtype == Type::SignedInt;   }
bool Value::isUnsignedInteger() const noexcept { return vtype == Type::UnsignedInt; }
bool Value::isObject()  const noexcept  { return isMap();                       }
bool Value::isNumeric()  const noexcept { return isInteger() or isFloat();     }
bool Value::isInteger() const noexcept  { return isSignedInteger() or isUnsignedInteger(); }

bool Value::isComparableWith(const Value &rhs) const noexcept
{
    if(type() == rhs.type())
        return true;
    return (isNumeric() and rhs.isNumeric());
}


//////////////// as<...> functions //////
///
///
//////////////////////////

long long Value::asInt64() const noexcept
{
    using limit = std::numeric_limits<long long>;

    if(isSignedInteger())
        return value.SignedInt;
    if(isUnsignedInteger())
        return in_range(value.UnsignedInt, limit::lowest(), limit::max()) ? value.UnsignedInt : 0;
    if(isFloat())
        return in_range(value.Float, limit::lowest(), limit::max()) ? value.Float : 0;

    if(isChar())
        return static_cast<long long>(value.Char);
    if(isBool())
        return value.Bool ? 1 : 0;
    if(isString())
    {
        try { return std::stoll(value.String); }
        catch (std::invalid_argument&) {}
        catch (std::out_of_range&) {}
        return 0;
    }
    return size();
}


unsigned long long Value::asUint64() const noexcept
{
    using limit = std::numeric_limits<unsigned long long>;

    if(isUnsignedInteger())
        return value.UnsignedInt;
    if(isSignedInteger())
        return in_range(value.SignedInt, limit::lowest(), limit::max()) ? value.SignedInt : 0;
    if(isFloat())
        return in_range(value.Float, limit::lowest(), limit::max()) ? value.Float : 0;
    if(isChar())
        return static_cast<unsigned long long>(value.Char);
    if(isBool())
        return value.Bool ? 1 : 0;
    if(isString())
    {
        try { return std::stoull(value.String); }
        catch (std::invalid_argument&) {}
        catch (std::out_of_range&) {}
        return 0;
    }

    return size();
}

double Value::asFloat() const noexcept
{

    if(isFloat())
        return value.Float;
    if(isString())
    {
        try { return std::stod(value.String); }
        catch (std::invalid_argument&) {}
        catch (std::out_of_range&) {}
        return 0;
    }

    double k1 = asUint64();
    double k2 = asInt64();

    return k1 > k2 ? k1 : k2;
}

bool Value::asBool() const noexcept
{
    if(isBool())
        return value.Bool;
    if(isUnsignedInteger())
        return value.UnsignedInt != 0;
    if(isSignedInteger())
        return value.SignedInt != 0;
    if(isFloat())
        return value.Float != 0;
    if(isChar())
        return value.Char != '\0';
    return size() != 0;
}

int Value::asInt() const noexcept
{
    using limit = std::numeric_limits<int>;
    return in_range(asInt64(), limit::lowest(), limit::max()) ? asInt64() : 0;
}

unsigned int Value::asUint() const noexcept
{
    using limit = std::numeric_limits<unsigned int>;
    return in_range(asUint64(), limit::lowest(), limit::max()) ? asUint64() : 0;
}

std::string Value::asString() const noexcept
{
    if(isString())
        return value.String;
    if(isBool())
        return value.Bool ? "true" : "false";
    // if(isBinary())
        //What should we do for Binary? Base64? or what?
    if(isChar())
        return {value.Char};
    if(isSignedInteger())
        return std::to_string(value.SignedInt);
    if(isUnsignedInteger())
        return std::to_string(value.UnsignedInt);
    if(isFloat())
        return std::to_string(value.Float);
    return "";
}

inline Value::BinaryType as_binary(const void* src, size_t size)
{
    Value::BinaryType rtn(size);
    std::memcpy(&rtn[0], src, size);
    return rtn;
}

Value::BinaryType Value::asBinary() const noexcept
{
    if(isBinary())
        return value.Binary;
    switch (vtype) {
    case Type::Char:
        return as_binary(&value.Char, sizeof(value.Char));
    case Type::Bool:
        return as_binary(&value.Bool, sizeof(value.Bool));
    case Type::SignedInt:
        return as_binary(&value.SignedInt, sizeof(value.SignedInt));
    case Type::UnsignedInt:
        return as_binary(&value.UnsignedInt, sizeof(value.UnsignedInt));
    case Type::Float:
        return as_binary(&value.Float, sizeof(value.Float));
    case Type::String:
        //return as_binary(reinterpret_cast<byte*>(value.String), sizeof(value.Char));
    case Type::Array:
        //return as_binary(reinterpret_cast<byte*>(value.Array), sizeof(value.Array));
    case Type::Map:
        //return as_binary(reinterpret_cast<byte*>(value.Map), sizeof(value.Map));
    case Type::Binary:
    default:
        break;
    }
    return Value::BinaryType();
}

//////////////// PRIVATE ////////////////
/////////////////////////////////////////
///
///
///
/////////////////////////////////////////


void Value::construct_fromString(std::string&& s)
{
    new( &(value.String)) std::string(std::move(s));
}

void Value::construct_fromBinary(BinaryType&& b)
{
    new( &(value.Binary)) BinaryType(std::move(b));
}

void Value::construct_fromArray(ArrayType&& a)
{
    new( &(value.Array)) ArrayType(std::move(a));
}

void Value::construct_fromMap(MapType&& m)
{
    new( &(value.Map)) MapType(std::move(m));
}

void Value::move_from(Value&& v)
{
    destruct();

    switch (v.vtype) {
    case Type::Char:
        value.Char = v.value.Char;
        break;
    case Type::Bool:
        value.Bool = v.value.Bool;
        break;
    case Type::SignedInt:
        value.SignedInt = v.value.SignedInt;
        break;
    case Type::UnsignedInt:
        value.UnsignedInt = v.value.UnsignedInt;
        break;
    case Type::Float:
        value.Float = v.value.Float;
        break;
    case Type::String:
        construct_fromString( std::move(v.value.String) );
        break;
    case Type::Binary:
        construct_fromBinary( std::move(v.value.Binary) );
        break;
    case Type::Array:
        construct_fromArray(  std::move(v.value.Array)  );
        break;
    case Type::Map:
        construct_fromMap(    std::move(v.value.Map)    );
        break;
    default:
        break;
    }

    vtype = v.vtype;
    v.destruct();
}

void Value::copy_from(const Value& v)
{
    destruct();

    switch (v.vtype) {
    case Type::Char:
        value.Char = v.value.Char;
        break;
    case Type::Bool:
        value.Bool = v.value.Bool;
        break;
    case Type::SignedInt:
        value.SignedInt = v.value.SignedInt;
        break;
    case Type::UnsignedInt:
        value.UnsignedInt = v.value.UnsignedInt;
        break;
    case Type::Float:
        value.Float = v.value.Float;
        break;
    case Type::String:
        construct_fromString( std::string( v.value.String ));
        break;
    case Type::Binary:
        construct_fromBinary( BinaryType(  v.value.Binary ));
        break;
    case Type::Array:
        construct_fromArray( ArrayType(unique_ptr_copy(v.value.Array)));
        break;
    case Type::Map:
        construct_fromMap( MapType(unique_ptr_copy(v.value.Map)));
        break;
    default:
        break;
    }
    vtype = v.vtype;
}

inline void Value::destruct() noexcept
{
    using std::string;
    using std::vector;


    switch (vtype) {
    case Type::Null:
        return;
    case Type::String:
        value.String.~string();
        break;
    case Type::Array:
        value.Array.~vector();
        break;
    case Type::Binary:
        value.Binary.~vector();
        break;
    case Type::Map:
        value.Map.~unordered_map();
        break;
    default:
        break;
    }

    vtype = Type::Null;
    //std::cout << "d " << std::flush;
}


//////////////////////////  CONVERSION OPERATOR  ///////////////////////////
///// I currently don't know a better way to avoid code duplication here
///// C++ forbids a const function to call a none const function


///simple int
Value::operator int ()
{ return int( operator long long& () ); }
Value::operator int () const
{ return int( operator long long const& () ); }


///// long long
Value::operator long long& () &
{
    if(vtype == Type::SignedInt)
        return value.SignedInt;
    throw bad_value_cast("'Value&' cannot be casted to 'long long'");
}

Value::operator long long const& () const&
{
    if(vtype == Type::SignedInt)
        return value.SignedInt;
    throw bad_value_cast("'Value const&' cannot casted to 'long long'");
}


///// unsigned long long
Value::operator unsigned long long& () &
{
    if(vtype == Type::UnsignedInt)
        return value.UnsignedInt;
    throw bad_value_cast("'Value&' cannot be casted to 'unsigned long long'");
}

Value::operator unsigned long long const& () const&
{
    if(vtype == Type::UnsignedInt)
        return value.UnsignedInt;
    throw bad_value_cast("'Value const&' cannot be casted to 'unsigned long long'");
}


///// bool
Value::operator bool & () &
{
    if(vtype == Type::Bool)
        return value.Bool;
    throw bad_value_cast("'Value&' cannot be casted to 'bool'");
}

Value::operator bool const& () const&
{
    if(vtype == Type::Bool)
        return value.Bool;
    throw bad_value_cast("'Value const&' cannot be casted to 'bool'");
}


///// char
Value::operator char & () &
{
    if(vtype == Type::Char)
        return value.Char;
    throw bad_value_cast("'Value&' cannot be casted to 'char'");
}

Value::operator char const& () const&
{
    if(vtype == Type::Char)
        return value.Char;
    throw bad_value_cast("'Value const&' cannot be casted to 'char'");
}


///// double
Value::operator double & () &
{
    if(vtype == Type::Float)
        return value.Float;
    throw bad_value_cast("'Value&' cannot be casted to 'double'");
}

Value::operator double const& () const&
{
    if(vtype == Type::Float)
        return value.Float;
    throw bad_value_cast("'Value&' cannot be casted to 'double'");
}


///// std::string
Value::operator std::string&& () &&
{
    if(vtype == Type::String)
        return std::move(value.String);
    throw bad_value_cast("'Value&&' cannot be casted to 'std::string&&'");
}

Value::operator std::string& () &
{
    if(vtype == Type::String)
        return value.String;
    throw bad_value_cast("'Value&' cannot be casted to 'std::string&'");
}

Value::operator std::string const& () const&
{
    if(vtype == Type::String)
        return value.String;
    throw bad_value_cast("'Value const&' cannot be casted to 'std::string const&'");
}


///// BinaryType
Value::operator BinaryType&& () &&
{
    if(vtype == Type::Binary)
        return std::move(value.Binary);
    throw bad_value_cast("'Value&&' cannot be casted to 'BinaryType&&'");
}

Value::operator BinaryType& () &
{
    if(vtype == Type::Binary)
        return value.Binary;
    throw bad_value_cast("'Value&' cannot be casted to 'BinaryType&'");
}

Value::operator BinaryType const& () const&
{
    if(vtype == Type::Binary)
        return value.Binary;
    throw bad_value_cast("'Value const&' cannot be casted to 'BinaryType const&'");
}


//////////////////////// FRIEND FUNCTION ///////////

void ubjson::swap(Value& v1, Value& v2)
{
    Value vt;
    vt.move_from( std::move( v2 ));
    v2.move_from( std::move( v1 ));
    v1.move_from( std::move( vt ));
}

/////////////////////// FREE OPERATORS ////////////

bool ubjson::operator == (const Value& lhs, const Value& rhs)
{
    using limit = std::numeric_limits<double>;

    if(lhs.isNumeric() and rhs.isNumeric())
        return std::abs(lhs.asFloat() - rhs.asFloat()) <= limit::epsilon();

    if(lhs.type() != rhs.type())
        return false;

    switch (rhs.type()) {
    case Type::Null:
        return true;
    case Type::Char:
        return lhs.value.Char == rhs.value.Char;
    case Type::Bool:
        return lhs.value.Bool == rhs.value.Bool;
    case Type::String:
        return lhs.value.String == rhs.value.String;
    case Type::Binary:
        return lhs.value.Binary == rhs.value.Binary;
    case Type::Array:
        return is_equal(lhs.value.Array, rhs.value.Array);
    case Type::Map:
        return is_equal(lhs.value.Map, rhs.value.Map);
    default:
        break;
    }
    return false;
}

bool ubjson::operator != (const Value& lhs, const Value& rhs)
{ 
	return not ubjson::operator == (lhs, rhs); 
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///

///////////////////////////////////////////////////////////


std::ostream& ubjson::operator << (std::ostream& os, to_ostream&& tos)
{
    tos.print_value(os, tos.value);
    return os;
}

void to_ostream::print_object(std::ostream &os, const Value &v)
{
    os << '{' << (ppretty ? "\n" : "");
    if(ppretty)
        push_addendum('\t');

    const auto keys = v.keys();

    size_t idx = 0, idxEnd = keys.size();
    for(const auto& key : keys)
    {
        os << addendum << "\"" << key << "\"" << (ppretty ? " : " : ":");;
        print_value(os, v[key]);

        if(++idx < idxEnd)
            os << ',';
        os << (ppretty ? "\n" : "");
    }

    pop_addendum();
    os << addendum << '}';
}

void to_ostream::print_array(std::ostream &os, const Value &v)
{
    if(ppretty)
        push_addendum('\t');
    os << '[';

    for(size_t i=0; i < v.size(); i++)
    {
        print_value(os, v[i]);

        if(i + 1 < v.size())
            os << (ppretty ? ", " : ",");
    }

    pop_addendum();
    os << ']';
}

void to_ostream::print_value(std::ostream &os, const Value &v)
{
    if(v.isNull())
        os << "null";
    else if(v.isBool())
        os << (v ? "true" : "false");
    else if(v.isChar())
        os << "\"" << static_cast<char>(v) << "\"";
    else if(v.isSignedInteger())
        os << static_cast<long long>(v);
    else if(v.isUnsignedInteger())
        os << static_cast<unsigned long long>(v);
    else if(v.isFloat())
        os << static_cast<double>(v);
    else if(v.isString())
        os << "\"" << static_cast<std::string>(v) << "\"";
    else if(v.isBinary())
        os << "BINARY DATA (" << static_cast<const Value::BinaryType&>(v).size() << " bytes)";
    else if(v.isArray())
        print_array(os, v);
    else if(v.isObject())
        print_object(os, v);
}
