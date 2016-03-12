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
  * @file value.hpp
  * Contains the Value class and a few functions for Argument Dependent Lookup (ADL)
  *
  * @brief A generalized container object for all value types exchageable by the protocol
  * @author WhiZTiM
  * @date January, 2015
  * @version 0.0.1
  *
  * The @class Value provides a simple and initutive inteface for storage and manipulation of types
  * The class stores ints, unsigned ints, floating point numbers, strings, character, ke-value maps and arrays
  *
  * @code
  * Value v{345};
    Value ha;
    Value array = { Value("name", "Joy"), Value("id", 34) };

    cout << "NAME: " << array[0]["name"].asString() << "\nid: " << array[1]["id"].asString() << endl;

    ha["sani"] = "Mark";
    ha["musa"] = "Yusuf";
    ha["kabir"] = 4546.34;
    cout << "value: " << ha["kabir"].asString() << endl << endl;
    for(const auto& a : ha)
        cout << '\t' << a.asFloat() << endl;
    @endcode
  *
  */

#ifndef VALUE_H
#define VALUE_H

#include <memory>
#include <string>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <map>
#include <initializer_list>
#include "exception.hpp"
#include "iterator.hpp"
#include "types.hpp"

namespace ubjson {


    /*!
     * \brief The Value class
     * A generalized container object for all value types exchageable by the protocol
     */
    class Value
    {
    public:
        //! An alias for std::vector<std::strings> which is the Type returned by \ref keys()
        using Keys = std::vector<std::string>;

        //! A pattern type alias, i.e (std::shared_ptr<Value>)
        using Sptr = std::shared_ptr<Value>;

        //! A pattern type alias, i.e (std::unique_ptr<Value>)
        using Uptr = std::unique_ptr<Value>;

        //! An alias used to internally represent \ref Type "Array" types
        using ArrayType = std::vector<Uptr>;

        //! An alias used to internally represent \ref Type "Binary" types
        //! \note \a byte is an alias for \e unsigned \e char
        using BinaryType = std::vector<byte>;

        //! An alias used to internally represent \ref Type "Map" types
        using MapType = std::unordered_map<std::string, Uptr>;

        //! Iterator alias for accessing values of an iterable value object
        using iterator = value_iterator<Value, ArrayType::iterator, MapType::iterator>;

        //! Iterator alias for accessing values of an iterable value object
        using const_iterator = value_iterator<const Value, ArrayType::const_iterator, MapType::const_iterator>;

        friend iterator;
        friend const_iterator;


        /*!
         * \brief This is the union type that actually stores the data for Value class
         * Every Value object has exactly one instance of a ValueHolder.
         * \remarks ValueHolder is 64bytes on 64bit systems
         */
        union ValueHolder
        {
            char Char;
            bool Bool;
            long long SignedInt;        //! Prefered for all integer representable within it's range
            unsigned long long UnsignedInt;     //! To be Used when explicitly requested or higher values are to be stored
            double Float;
            std::string String;
            ArrayType Array;
            BinaryType Binary;
            MapType Map;

            ValueHolder() {}
            ~ValueHolder() {}
        };


        /*!
         * \brief contstructs a null Value containing nothing
         * \post isNull() == true
         */
        Value();

        /*!
         * \brief contstructs Value containing the given \e int
         * \post isSignedInteger() == true \e and type() == Type::SignedInt
         * \remarks you can safely call the \e int convertion operator int()
         *
         * This constructor is mainly for convenience, it delegates construction to Value(long long)
         * \see Value(long long)
         */
        Value(int);

        /*!
         * \brief contstructs Value containing the given \e bool
         * \post isBool() == true \e and type() == Type::Bool
         * \remarks you can safely call the \e bool conversion operator
         */
        Value(bool);

        /*!
         * \brief contstructs Value containing the given \e char
         * \post isChar() == true \e and type() == Type::Char
         * \remarks you can safely call the \e char conversion operator
         */
        Value(char);

        /*!
         * \brief contstructs Value containing the given \e double
         * \post isFloat() == true \e and type() == Type::Float
         * \note for \e float type, you may want to convert to double first, because there is no constructor for \e float
         *
         * \remarks you can safely call the \e double conversion operator
         */
        Value(double);

        /*!
         * \brief contstructs Value containing the given \e long \e long
         * \post isSignedInteger() == true \e and type() == Type::SignedInt
         * \remarks you can safely call the \e long \e long conversion operator long long& ()&
         *
         * \note if compiler complains of ambigious constructor call, use the \e ll prefix...
         * This is the constructor that get's called for signed integer type.
         */
        Value(long long);

        /*!
         * \brief contstructs Value containing the given \e const \e char*
         * \post isString() == true \e and type() == Type::String
         * \remarks you can safely call the \e std::string conversion operators
         *
         * This constructor is mainly for convenience, it delegates construction to Value(std::string)
         */
        Value(const char*);

        /*!
         * \brief contstructs Value containing the given \e long \e long
         * \post isUnsignedInteger() == true \e and type() == Type::UnsignedInt
         * \remarks you can safely call the \e unsigned \e long \e long conversion operator
         *
         * \note if compiler complains of ambigious constructor call, use the \e ull prefix...
         * This is the constructor that get's called for unsigned integer type.
         */
        Value(unsigned long long);

        /*!
         * \brief contstructs a map-type Value with the given \e std::string as \b key and \b val as Value
         * \post isMap() == true \e and type() == Type::Map
         *
         * \code
         * Value v1; //an empty value
         * v1["key"] = "C++14!";
         *
         * // This is same as
         * Value v2("key", "C++14!);
         *
         * v1 == v2 ; //true
         * \endcode
         *
         */
        Value(const std::string&, Value);


        /*!
         * \brief contstructs Value containing the given \e std::string
         * \post isString() == true \e and type() == Type::String
         * \remarks you can safely call the \e std::string conversion operators
         */
        Value(std::string);


        /*!
         * \brief contstructs Value containing the given BinaryType
         * \post isBinary() == true \e and type() == Type::Binary
         * \remarks you can safely call the \e BinaryType conversion operators
         */
        Value(BinaryType);


        /*!
         * \brief uniform-brace initialization constructor
         * \post For single arguments, it has the same effect as calling the single argument constructors...
         * For more than one argument, then \b type() \b == \b Type::Array \e and isArray() \b == \b true
         *
         * Example
         * \code
         * Value v1{-3.1416};
         * v1.isSignedInteger();    //true
         * v1.isArray();            //false
         *
         * Value v2{"coward", 9.345};
         * v2.isSignedInteger();    //false
         * v2.isArray();            //true
         *
         * Value v3("m1", "mmm");
         * Value v4{"m1", "mmm"};
         *
         * v3.isMap();              //true
         * v4.isMap();              //false
         * v3.isArray();            //false
         * v4.isArray();            //true
         * \endcode
         */
        Value(std::initializer_list<Value>);


        /*!
         * \brief moves the given value and sets \b val to \b Type::Null
         * \post this now contains moved value, and \b val.isNull() \b == \b true
         */
        Value(Value&&);

        /*!
         * \brief Copies the given value
         */
        Value(const Value&);


        /*!
         * \brief recursively destroys all contained objects
         */
        ~Value();

        //! returns the \b Type contained by Value::ValueHolder
        Type type() const noexcept;

        /*!
         * \brief returns the number of items contained in this Value Object
         * \return std::size_t(1) for none Null, Array and Map types
         */
        size_t size() const noexcept;

        //! Returns whether the contained type is a (Key-Value) Map {like std::unordered_map}
        bool isMap() const noexcept;

        //! Returns whether the contained type is Null (empty)
        bool isNull() const noexcept;

        //! Returns whether the contained type is a single Character (char)
        bool isChar() const noexcept;

        //! Returns whether the contained type is a boolean (bool)
        bool isBool() const noexcept;

        //! Returns whether the contained type is a floating point type (double)
        bool isFloat() const noexcept;

        //! Returns whether the contained type is a floating point type. (double)
        bool isArray() const noexcept;

        //! The same thing as \ref isMap()
        bool isObject() const noexcept;

        //! Returns whether the contained type is a string type. (std::string)
        bool isString() const noexcept;

        //! Returns whether the contained type is \ref BinaryType "Binary"
        bool isBinary() const noexcept;

        //! Returns whether the contained type is a numeric type. ( double or {(unsigned)int/long long} )
        bool isNumeric() const noexcept;

        //! Returns whether the contained type is an integer type. ( {(unsigned)int/long long} )
        bool isInteger() const noexcept;

        //! Returns whether the contained type is an signed integer type. ( int/long long )
        bool isSignedInteger() const noexcept;

        //! Returns whether the contained type is an unsigned integer type. ( unsigned int/unsigned long long )
        bool isUnsignedInteger() const noexcept;

        /*!
         * \fn isComparableWith
         * \brief determines whether the type of \a rhs can be compared with the type of the current \ref Value object
         * \param[in] rhs
         * \return \a true if
         * \code this->type() == rhs.type() or isNumeric(rhs.type()) and isNumeric(this->type()) \endcode
         */
        bool isComparableWith(const Value& rhs) const noexcept;

        /*!
         * \brief unconditionally converts the contained type to \e bool using the following rules
         * - correct values are guranteed if the contained type is \e bool
         * - if the contained type is numeric, it returns true if the value is not \b 0
         * - if is character, returns, operator char & ()& != '\0'
         * - if the contained type is string, map, array, or binary, it returns true if size() > 0;
         * \remarks This function is guranteed not to throw
         */
        bool                asBool()   const noexcept;

        /*!
         * \brief unconditionally converts the contained type to \e int using the following rules
         * - correct values are guranteed if Value was constructed with Value(int)
         * - if the contained type is numeric, it returns it if it fits within the range representable
         * by an integer, otherwise, returns \b 0
         * \note this function actually delgates conversion to asInt64(), please refer to it for further rules
         * \remarks This function is guranteed not to throw
         */
        int                 asInt()    const noexcept;

        /*!
         * \brief unconditionally converts the contained type to \a unsigned \a int using the following rule
         * - if the contained type is numeric, it returns it if it fits within the range representable
         * by an \e unsigned \e int, otherwise, returns \b 0
         * \note this function actually delgates conversion to asUint64(), please refer to it for further rules
         * \remarks This function is guranteed not to throw
         */
        unsigned int        asUint()   const noexcept;

        /*!
         * \brief unconditionally converts the contained type to \e long \e long using the following rules
         * - correct values are guranteed if type() is Type::SignedInt
         * - if the contained type is numeric, it returns it if it fits within the range representable
         * by an \e long \e long integer, otherwise, returns \b 0
         * - if the contained type is string, it attempts to convert it using "std::stoll".
         * Exceptions thrown by it are handled internally
         * - otherwise, this function returns size()
         * \remarks Except when operator ::new fails, this function is guranteed not to throw.
         */
        long long           asInt64()  const noexcept;

        /*!
         * \brief unconditionally converts the contained type to \e unsigned \e long \e long using the following rules
         * - correct values are guranteed if type() is Type::UnsignedInt
         * - if the contained type is numeric, it returns it if it fits within the range representable
         * by an \e unsigned \e long \e long integer, otherwise, returns \b 0
         * - if the contained type is string, it attempts to convert it using "std::stoull".
         * Exceptions thrown by it are handled internally
         * - otherwise, this function returns size()
         * \remarks Except when operator ::new fails, this function is guranteed not to throw.
         */
        unsigned long long  asUint64() const noexcept;

        /*!
         * \brief unconditionally converts the contained type to \e double using the following rules
         * - correct values are guranteed if type() is Type::Float
         * - if the contained type is numeric, it returns it if it fits within the range representable
         * by a \e double, otherwise, returns \b 0.0
         * - if the contained type is string, it attempts to convert it using "std::stod".
         * Exceptions thrown by it are handled internally
         * - otherwise, this function returns size()
         * \note this function actually delgates conversion to asInt64(), so refer to it for further behavior
         * \remarks Except when operator ::new fails, this function is guranteed not to throw.
         */
        double              asFloat()  const noexcept;

        /*!
         * \brief unconditionally converts the contained type to \e std::string using the following rules
         * - correct values are guranteed if type() is Type::String
         * or is of Type::String
         * - if the contained type is numeric, it returns it in string form,
         * - if the contained type is string, it attempts to convert it using "std::stod".
         * Exceptions thrown by it are handled internally
         * - otherwise, this function returns size()
         * \remarks This function is guranteed not to throw.
         */
        std::string         asString() const noexcept;
        BinaryType          asBinary() const noexcept;

        Value& operator = (const Value& lhs);
        Value& operator = (Value&& lhs);

        Value& operator [] (int i);
        Value const& operator [] (int i) const;

        Value& operator [] (const char*);
        Value const& operator [] (const char*) const;

        Value& operator [] (const std::string&);
        Value const& operator [] (const std::string&) const;

        void push_back(const Value&);
        void push_back(Value&&);

        bool contains(const Value&) const;
        void remove(const Value&);

        iterator find(const Value&);
        const_iterator find(const Value&) const;

        /*!
         * \brief retrieve all the keys for accessing this Object through operator []
         * \pre \code this->isMap() \endcode
         * \return \ref Keys containing map keys. If precondition isn't met, an empty \ref Keys "key"
         */
        Keys keys() const;

        iterator begin()
        { return iterator(this, iterator::pos::begin); }

        iterator end()
        { return iterator(this, iterator::pos::end); }

        const_iterator begin() const
        { return cbegin(); }

        const_iterator end() const
        { return cend(); }

        const_iterator cbegin() const
        { return const_iterator(this, const_iterator::pos::begin); }

        const_iterator cend() const
        { return const_iterator(this, const_iterator::pos::end); }

        //! casts to the to the integer contained, else throws bad_value_cast
        operator int ();
        operator int () const;

        //! casts to a reference to the bool contained, else throws bad_value_cast
        operator bool& () &;
        //! casts to a const reference to the bool contained, else throws bad_value_cast
        operator bool const& () const&;

        //! casts to a reference to the character contained, else throws bad_value_cast
        operator char& () &;
        //! casts to a const reference to the character contained, else throws bad_value_cast
        operator char const& () const&;

        operator double& () &;
        operator double const& () const&;

        operator long long& () &;
        operator long long const& () const&;

        operator unsigned long long& () &;
        operator unsigned long long const& () const&;

        operator std::string&& () &&;
        operator std::string& () &;
        operator std::string const& () const&;

        operator BinaryType&& () &&;
        operator BinaryType& () &;
        operator BinaryType const& () const&;

        friend void swap(Value&, Value&);
        friend bool operator == (const Value&, const Value&);

    private:

        void construct_fromString(std::string&&);
        void construct_fromArray(ArrayType&&);
        void construct_fromBinary(BinaryType&&);
        void construct_fromMap(MapType&&);
        inline void destruct() noexcept;

        void move_from(Value&&);
        void copy_from(const Value&);

        ValueHolder value;
        Type vtype = Type::Null;


    };

    void swap(Value&, Value&);
    bool operator == (const Value&, const Value&);
    bool operator != (const Value&, const Value&s);



    /*!
     * \brief The to_ostream class is responsible for pretty printing Value in JSON format
     *
     * You can either print it in compact mode, or pretty. Example
     * \code
     * Value v("faves", {"Nigeria", 3.1416, '@'});
     * v["region"] = "Africa";
     *
     * std::cout << to_ostream(v) << std::endl; //prints
     * // {
     * //      "faves" : [ "Nigeria", 3.1416, "@" ],
     * //      "region" : "Africa"
     * // }
     *
     * std::cout << to_ostream(v, to_ostream::compact) << std::endl; //prints
     * // {"faves":["Nigeria",3.1416,"@"],"region":"Africa"}
     *
     * \endcode
     * NOTE. to_ostream(Value) is same as to_ostream(Value, to_ostream::pretty)
     *
     * It can only used in rvalue context. Example
     * \code
     * Value v;
     * // . . .fill with whatever
     * std::cout << to_ostream(v) << std::endl;
     *
     * //Wrong way to use
     * to_ostream tos(v)
     * std::cout << tos << std::endl;   //Shouldn't be used this way
     * \endcode
     *
     */
    class to_ostream
    {
    public:
        enum Opt : char { pretty, compact };

        to_ostream(const Value& val, Opt option = pretty)
            : value(val), opt(option)
        { ppretty = opt == pretty; }

        friend std::ostream& operator << (std::ostream& stream, to_ostream&& tos);

    private:
        void print_value(std::ostream& os, const Value& v);
        void print_object(std::ostream& os, const Value& v);
        void print_array(std::ostream& os, const Value& v);

        inline void push_addendum(char c)
        { addendum.push_back(c); }

        inline void pop_addendum() {
            if(not addendum.empty())
                addendum.pop_back();
        }

        const Value& value;
        const Opt opt;
        std::string addendum;
        bool ppretty;
    };

    std::ostream& operator << (std::ostream& stream, to_ostream&& tos);



    /*!
     * \brief The ValueSizePolicy struct
     * This is a POD struct that should be used by utility routines to
     * Parse or construct a Value based on the member values
     *
     * It can be assigned using Uniform brace initialization or one after the other.
     * Clients are encouraged to always use a \b const copy of this class
     *
     * \code
     * ValueSizePolicy vs = {8, 1024, 1024, 4096, 255, 255};
     *
     * //Client
     * class Reader
     * {
     *      const ValueSizePolicy vsp; // NOTE the "const"
     * public:
     *      Reader(const ValueSizePolicy& vs)
     *          : vsp(vs) {}
     *      //Every other code. . . .
     * };
     * \endcode
     *
     */
    struct ValueSizePolicy      //NOTE: Please never reorder the members, because, brace initializer{}
    {
        //! This prevents stack overflow if the parser is implemented using some recursive technique
        std::size_t max_value_depth;

        //! This dictates the maximum size in bytes a binary type can have
        std::size_t max_binary_size;

        //! This dictates the maximum size in bytes a string can have
        std::size_t max_string_size;

        //! This dictates the maximum size in bytes a value type can have.
        //! \note since a standard UBEX document must be an \e object,
        //! this setting \b should superceed every other setting
        std::size_t max_object_size;

        //! This dictates the maximum items Value::size() in an Array
        std::size_t max_array_items;

        //! This dictates the maximum items Value::size() in an Object
        std::size_t max_object_items;
    };


}
#endif // VALUE_H
