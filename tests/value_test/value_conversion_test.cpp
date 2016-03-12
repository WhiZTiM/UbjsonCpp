#include "value.hpp"
#include "../test_utils/format_helpers.hpp"
#include <cppunit/extensions/HelperMacros.h>

using namespace ubjson;
using namespace std;
int weird_cppunit_extern_bug_value_conversion_test = 0;

class Value_Conversion_Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( Value_Conversion_Test );
    CPPUNIT_TEST( test_asBool );
    CPPUNIT_TEST( test_asInt );
    CPPUNIT_TEST( test_asUInt );
    CPPUNIT_TEST( test_asInt64 );
    CPPUNIT_TEST( test_asUInt64 );
    CPPUNIT_TEST( test_asFloat );
    CPPUNIT_TEST( test_asString );
    CPPUNIT_TEST( test_asBinary );
    /////////////////////////////
    CPPUNIT_TEST( test_intOperator );
    CPPUNIT_TEST( test_boolOperator );
    CPPUNIT_TEST( test_charOperator );
    CPPUNIT_TEST( test_doubleOperator );
    CPPUNIT_TEST( test_longlongOperator );
    CPPUNIT_TEST( test_unsignedlonglongOperator );
    CPPUNIT_TEST( test_stringTypeOperator );
//    CPPUNIT_TEST( test_stringMoveOperation );
    CPPUNIT_TEST( test_binaryTypeOperator );
    CPPUNIT_TEST( test_binaryMoveOperation );
    CPPUNIT_TEST_SUITE_END();
public:
    using T = Value::BinaryType::value_type;
    void setUp() override
    {
        v_empty = std::make_unique<Value>(Value());
        v_char = std::make_unique<Value>(Value('c'));
        v_bool = std::make_unique<Value>(Value(true));
        v_signedint = std::make_unique<Value>(Value(-700));
        v_unsignedint = std::make_unique<Value>(Value(800ull));
        v_float = std::make_unique<Value>(Value(3.1416));
        v_string = std::make_unique<Value>(Value("string"));
        v_array = std::make_unique<Value>(Value({34.657, "Yeepa", 466, -53, 'g'}));
        v_map = std::make_unique<Value>(Value());

        v_binary = std::make_unique<Value>(Value(Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xdf)})));

        (*v_map)["name"] = "WhiZTiM";
        (*v_map)["id"] = 12343;
        (*v_map)["extras"] = { *v_array, "nice one bro!" };
    }
private:
    Value::Uptr v_empty;
    Value::Uptr v_char;
    Value::Uptr v_bool;
    Value::Uptr v_signedint;
    Value::Uptr v_unsignedint;
    Value::Uptr v_float;
    Value::Uptr v_string;
    Value::Uptr v_array;
    Value::Uptr v_map;
    Value::Uptr v_binary;
public:
    void test_asBool()
    {
        CPPUNIT_ASSERT_EQUAL( v_empty->asBool(),        false );
        CPPUNIT_ASSERT_EQUAL( v_char->asBool(),         true  );
        CPPUNIT_ASSERT_EQUAL( v_bool->asBool(),         true  );
        CPPUNIT_ASSERT_EQUAL( v_signedint->asBool(),    true  );
        CPPUNIT_ASSERT_EQUAL( v_unsignedint->asBool(),  true  );
        CPPUNIT_ASSERT_EQUAL( v_float->asBool(),        true  );
        CPPUNIT_ASSERT_EQUAL( v_string->asBool(),       true  );
        CPPUNIT_ASSERT_EQUAL( v_array->asBool(),        true  );
        CPPUNIT_ASSERT_EQUAL( v_map->asBool(),          true  );
        CPPUNIT_ASSERT_EQUAL( v_binary->asBool(),       true  );
    }

    void test_asInt()
    {
        CPPUNIT_ASSERT_EQUAL(0,             v_empty->asInt() );  //valid
        CPPUNIT_ASSERT_EQUAL(99,             v_char->asInt() );  //valid
        CPPUNIT_ASSERT_EQUAL(1,              v_bool->asInt() );  //valid
        CPPUNIT_ASSERT_EQUAL(-700,      v_signedint->asInt() );  //valid
        CPPUNIT_ASSERT_EQUAL(800,     v_unsignedint->asInt() );  //valid
        CPPUNIT_ASSERT_EQUAL(3,             v_float->asInt() );  //valid, floor division
        CPPUNIT_ASSERT_EQUAL(0,            v_string->asInt() );  //undefined if invalid
        CPPUNIT_ASSERT_EQUAL(5,             v_array->asInt() );  //undefined
        CPPUNIT_ASSERT_EQUAL(3,               v_map->asInt() );  //undefined
        CPPUNIT_ASSERT_EQUAL(1,            v_binary->asInt() );  //undefined
    }

    void test_asUInt()
    {
        CPPUNIT_ASSERT_EQUAL(0u,            v_empty->asUint() );  //valid
        CPPUNIT_ASSERT_EQUAL(99u,            v_char->asUint() );  //valid
        CPPUNIT_ASSERT_EQUAL(1u,             v_bool->asUint() );  //valid
        CPPUNIT_ASSERT_EQUAL(0u,        v_signedint->asUint() );  //valid
        CPPUNIT_ASSERT_EQUAL(800u,    v_unsignedint->asUint() );  //valid
        CPPUNIT_ASSERT_EQUAL(3u,            v_float->asUint() );  //valid, floor division
        CPPUNIT_ASSERT_EQUAL(0u,           v_string->asUint() );  //undefined if invalid
        CPPUNIT_ASSERT_EQUAL(5u,            v_array->asUint() );  //undefined
        CPPUNIT_ASSERT_EQUAL(3u,              v_map->asUint() );  //undefined
        CPPUNIT_ASSERT_EQUAL(1u,           v_binary->asUint() );  //undefined
    }

    void test_asInt64()
    {
        CPPUNIT_ASSERT_EQUAL(0ll,            v_empty->asInt64() );  //valid
        CPPUNIT_ASSERT_EQUAL(99ll,            v_char->asInt64() );  //valid
        CPPUNIT_ASSERT_EQUAL(1ll,             v_bool->asInt64() );  //valid
        CPPUNIT_ASSERT_EQUAL(-700ll,        v_signedint->asInt64() );  //valid
        CPPUNIT_ASSERT_EQUAL(800ll,    v_unsignedint->asInt64() );  //valid
        CPPUNIT_ASSERT_EQUAL(3ll,            v_float->asInt64() );  //valid, floor division
        CPPUNIT_ASSERT_EQUAL(0ll,           v_string->asInt64() );  //undefined if invalid
        CPPUNIT_ASSERT_EQUAL(5ll,            v_array->asInt64() );  //undefined
        CPPUNIT_ASSERT_EQUAL(3ll,              v_map->asInt64() );  //undefined
        CPPUNIT_ASSERT_EQUAL(1ll,           v_binary->asInt64() );  //undefined
    }

    void test_asUInt64()
    {
        CPPUNIT_ASSERT_EQUAL(0ull,            v_empty->asUint64() );  //valid
        CPPUNIT_ASSERT_EQUAL(99ull,            v_char->asUint64() );  //valid
        CPPUNIT_ASSERT_EQUAL(1ull,             v_bool->asUint64() );  //valid
        CPPUNIT_ASSERT_EQUAL(0ull,        v_signedint->asUint64() );  //valid
        CPPUNIT_ASSERT_EQUAL(800ull,    v_unsignedint->asUint64() );  //valid
        CPPUNIT_ASSERT_EQUAL(3ull,            v_float->asUint64() );  //valid, floor division
        CPPUNIT_ASSERT_EQUAL(0ull,           v_string->asUint64() );  //undefined if invalid
        CPPUNIT_ASSERT_EQUAL(5ull,            v_array->asUint64() );  //undefined
        CPPUNIT_ASSERT_EQUAL(3ull,              v_map->asUint64() );  //undefined
        CPPUNIT_ASSERT_EQUAL(1ll,             v_binary->asInt64() );  //undefined
    }

    void test_asFloat()
    {
        CPPUNIT_ASSERT_EQUAL(0.0,            v_empty->asFloat() );  //valid
        CPPUNIT_ASSERT_EQUAL(99.0,            v_char->asFloat() );  //valid
        CPPUNIT_ASSERT_EQUAL(1.0,             v_bool->asFloat() );  //valid
        CPPUNIT_ASSERT_EQUAL(0.0,        v_signedint->asFloat() );  //valid
        CPPUNIT_ASSERT_EQUAL(800.0,    v_unsignedint->asFloat() );  //valid
        CPPUNIT_ASSERT_EQUAL(3.1416,         v_float->asFloat() );  //valid, floor division
        CPPUNIT_ASSERT_EQUAL(0.0,           v_string->asFloat() );  //undefined if invalid
        CPPUNIT_ASSERT_EQUAL(5.0,            v_array->asFloat() );  //undefined
        CPPUNIT_ASSERT_EQUAL(3.0,              v_map->asFloat() );  //undefined
        CPPUNIT_ASSERT_EQUAL(1.0,           v_binary->asFloat() );  //undefined
    }

    void test_asString()
    {
        CPPUNIT_ASSERT_EQUAL(""s,               v_empty->asString() );  //valid
        CPPUNIT_ASSERT_EQUAL("c"s,               v_char->asString() );  //valid
        CPPUNIT_ASSERT_EQUAL("true"s,            v_bool->asString() );  //valid
        CPPUNIT_ASSERT_EQUAL("-700"s,       v_signedint->asString() );  //valid
        CPPUNIT_ASSERT_EQUAL("800"s,      v_unsignedint->asString() );  //valid
        CPPUNIT_ASSERT_EQUAL("3.141600"s,       v_float->asString() );  //valid, zero dependent
        CPPUNIT_ASSERT_EQUAL("string"s,           v_string->asString() );  //undefined if invalid
        CPPUNIT_ASSERT_EQUAL(""s,            v_array->asString() );  //undefined
        CPPUNIT_ASSERT_EQUAL(""s,              v_map->asString() );  //undefined
        CPPUNIT_ASSERT_EQUAL(""s,           v_binary->asString() );  //undefined
    }

    void test_asBinary()
    {
        using Binary = Value::BinaryType;
        Value v1('c'), v2(true), v3(false);
        CPPUNIT_ASSERT_EQUAL(Binary({T(0x63)}),  v1.asBinary() );  //valid
        CPPUNIT_ASSERT_EQUAL(Binary({T(0x01)}),  v2.asBinary() );  //valid
        CPPUNIT_ASSERT_EQUAL(Binary({T(0x00)}),  v3.asBinary() );  //valid

        CPPUNIT_ASSERT_EQUAL(Binary(),          v_empty->asBinary() );  //valid
        CPPUNIT_ASSERT_EQUAL(Binary({T(0x63)}),  v_char->asBinary() );  //valid
        CPPUNIT_ASSERT_EQUAL(Binary({T(0x1)}),   v_bool->asBinary() );  //valid

        CPPUNIT_ASSERT_EQUAL(Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xdf)}), v_binary->asBinary() );

        //The rest are platform independent
    }

    void test_intOperator()
    {
        CPPUNIT_ASSERT_THROW(static_cast<int>(*v_empty), bad_value_cast);
        CPPUNIT_ASSERT_THROW(static_cast<int>(*v_char), bad_value_cast);
        CPPUNIT_ASSERT_THROW(static_cast<int>(*v_bool), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(static_cast<int>(*v_signedint));
        CPPUNIT_ASSERT_THROW(static_cast<int>(*v_unsignedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(static_cast<int>(*v_float), bad_value_cast);
        CPPUNIT_ASSERT_THROW(static_cast<int>(*v_string), bad_value_cast);
        CPPUNIT_ASSERT_THROW(static_cast<int>(*v_array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(static_cast<int>(*v_map), bad_value_cast);
        CPPUNIT_ASSERT_THROW(static_cast<int>(*v_binary), bad_value_cast);

        CPPUNIT_ASSERT_THROW(int k = *v_empty, bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = *v_char, bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = *v_bool, bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(int k = *v_signedint);
        CPPUNIT_ASSERT_THROW(int k = *v_unsignedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = *v_float, bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = *v_string, bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = *v_array, bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = *v_map, bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = *v_binary, bad_value_cast);

        //Call const and call version
        CPPUNIT_ASSERT_THROW(int k = const_cast<const Value&>(*v_empty), bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = const_cast<const Value&>(*v_char), bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = const_cast<const Value&>(*v_bool), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(int k = const_cast<const Value&>(*v_signedint));
        CPPUNIT_ASSERT_THROW(int k = const_cast<const Value&>(*v_unsignedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = const_cast<const Value&>(*v_float), bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = const_cast<const Value&>(*v_string), bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = const_cast<const Value&>(*v_array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = const_cast<const Value&>(*v_map), bad_value_cast);
        CPPUNIT_ASSERT_THROW(int k = const_cast<const Value&>(*v_binary), bad_value_cast);
    }

    void test_boolOperator()
    {
        //Type bool is a first-class type of Value, hence, we can obtain a reference
        CPPUNIT_ASSERT_THROW(bool& k = *v_empty, bad_value_cast);
        CPPUNIT_ASSERT_THROW(bool& k = *v_char, bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(bool& k = *v_bool);
        CPPUNIT_ASSERT_THROW(bool& k = *v_signedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(bool& k = *v_unsignedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(bool& k = *v_float, bad_value_cast);
        CPPUNIT_ASSERT_THROW(bool& k = *v_string, bad_value_cast);
        CPPUNIT_ASSERT_THROW(bool& k = *v_array, bad_value_cast);
        CPPUNIT_ASSERT_THROW(bool& k = *v_map, bad_value_cast);
        CPPUNIT_ASSERT_THROW(bool& k = *v_binary, bad_value_cast);

        //Call const and call version
        CPPUNIT_ASSERT_THROW(const bool& k = const_cast<const Value&>(*v_empty), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const bool& k = const_cast<const Value&>(*v_char), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(const bool& k = const_cast<const Value&>(*v_bool));
        CPPUNIT_ASSERT_THROW(const bool& k = const_cast<const Value&>(*v_signedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const bool& k = const_cast<const Value&>(*v_unsignedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const bool& k = const_cast<const Value&>(*v_float), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const bool& k = const_cast<const Value&>(*v_string), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const bool& k = const_cast<const Value&>(*v_array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const bool& k = const_cast<const Value&>(*v_map), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const bool& k = const_cast<const Value&>(*v_binary), bad_value_cast);

        // Reference modification
        Value Bool(true);
        CPPUNIT_ASSERT(Bool == Value(true));
        bool& ref_var = Bool;
        ref_var = false;
        CPPUNIT_ASSERT(Bool == Value(false));
    }

    void test_charOperator()
    {

        //Type char is a first-class type of Value, hence, we can obtain a reference
        CPPUNIT_ASSERT_THROW(char& k = *v_empty, bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(char& k = *v_char);
        CPPUNIT_ASSERT_THROW(char& k = *v_bool, bad_value_cast);
        CPPUNIT_ASSERT_THROW(char& k = *v_signedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(char& k = *v_unsignedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(char& k = *v_float, bad_value_cast);
        CPPUNIT_ASSERT_THROW(char& k = *v_string, bad_value_cast);
        CPPUNIT_ASSERT_THROW(char& k = *v_array, bad_value_cast);
        CPPUNIT_ASSERT_THROW(char& k = *v_map, bad_value_cast);
        CPPUNIT_ASSERT_THROW(char& k = *v_binary, bad_value_cast);

        //Call const and call version
        CPPUNIT_ASSERT_THROW(const char& k = const_cast<const Value&>(*v_empty), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(const char& k = const_cast<const Value&>(*v_char));
        CPPUNIT_ASSERT_THROW(const char& k = const_cast<const Value&>(*v_bool), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const char& k = const_cast<const Value&>(*v_signedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const char& k = const_cast<const Value&>(*v_unsignedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const char& k = const_cast<const Value&>(*v_float), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const char& k = const_cast<const Value&>(*v_string), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const char& k = const_cast<const Value&>(*v_array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const char& k = const_cast<const Value&>(*v_map), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const char& k = const_cast<const Value&>(*v_binary), bad_value_cast);

        // Reference modification
        Value Char('Q');
        CPPUNIT_ASSERT(Char == Value('Q'));
        char& ref_var = Char;
        ref_var = '@';
        CPPUNIT_ASSERT(Char == Value('@'));
    }

    void test_doubleOperator()
    {
        //Type double is a first-class type of Value, hence, we can obtain a reference
        CPPUNIT_ASSERT_THROW(double& k = *v_empty, bad_value_cast);
        CPPUNIT_ASSERT_THROW(double& k = *v_char, bad_value_cast);
        CPPUNIT_ASSERT_THROW(double& k = *v_bool, bad_value_cast);
        CPPUNIT_ASSERT_THROW(double& k = *v_signedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(double& k = *v_unsignedint, bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(double& k = *v_float);
        CPPUNIT_ASSERT_THROW(double& k = *v_string, bad_value_cast);
        CPPUNIT_ASSERT_THROW(double& k = *v_array, bad_value_cast);
        CPPUNIT_ASSERT_THROW(double& k = *v_map, bad_value_cast);
        CPPUNIT_ASSERT_THROW(double& k = *v_binary, bad_value_cast);

        //Call const and call version
        CPPUNIT_ASSERT_THROW(const double& k = const_cast<const Value&>(*v_empty), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const double& k = const_cast<const Value&>(*v_char), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const double& k = const_cast<const Value&>(*v_bool), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const double& k = const_cast<const Value&>(*v_signedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const double& k = const_cast<const Value&>(*v_unsignedint), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(const double& k = const_cast<const Value&>(*v_float));
        CPPUNIT_ASSERT_THROW(const double& k = const_cast<const Value&>(*v_string), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const double& k = const_cast<const Value&>(*v_array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const double& k = const_cast<const Value&>(*v_map), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const double& k = const_cast<const Value&>(*v_binary), bad_value_cast);

        // Reference modification
        Value Double(3.1416);
        CPPUNIT_ASSERT(Double == Value(3.1416));
        double& ref_var = Double;
        ref_var = 9.80665;
        CPPUNIT_ASSERT(Double == Value(9.80665));
    }

    void test_longlongOperator()
    {
        //Type long long is a first-class type of Value, hence, we can obtain a reference
        CPPUNIT_ASSERT_THROW(long long& k = *v_empty, bad_value_cast);
        CPPUNIT_ASSERT_THROW(long long& k = *v_char, bad_value_cast);
        CPPUNIT_ASSERT_THROW(long long& k = *v_bool, bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(long long& k = *v_signedint);
        CPPUNIT_ASSERT_THROW(long long& k = *v_unsignedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(long long& k = *v_float, bad_value_cast);
        CPPUNIT_ASSERT_THROW(long long& k = *v_string, bad_value_cast);
        CPPUNIT_ASSERT_THROW(long long& k = *v_array, bad_value_cast);
        CPPUNIT_ASSERT_THROW(long long& k = *v_map, bad_value_cast);
        CPPUNIT_ASSERT_THROW(long long& k = *v_binary, bad_value_cast);

        //Call const and call version
        CPPUNIT_ASSERT_THROW(const long long& k = const_cast<const Value&>(*v_empty), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const long long& k = const_cast<const Value&>(*v_char), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const long long& k = const_cast<const Value&>(*v_bool), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(const long long& k = const_cast<const Value&>(*v_signedint));
        CPPUNIT_ASSERT_THROW(const long long& k = const_cast<const Value&>(*v_unsignedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const long long& k = const_cast<const Value&>(*v_float), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const long long& k = const_cast<const Value&>(*v_string), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const long long& k = const_cast<const Value&>(*v_array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const long long& k = const_cast<const Value&>(*v_map), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const long long& k = const_cast<const Value&>(*v_binary), bad_value_cast);

        // Reference modification
        Value LongLong(987654321);
        CPPUNIT_ASSERT(LongLong == Value(987654321));
        long long& ref_var = LongLong;
        ref_var = -123456789;
        CPPUNIT_ASSERT(LongLong == Value(-123456789));
    }

    void test_unsignedlonglongOperator()
    {
        //Type unsigned long long is a first-class type of Value, hence, we can obtain a reference
        CPPUNIT_ASSERT_THROW(unsigned long long& k = *v_empty, bad_value_cast);
        CPPUNIT_ASSERT_THROW(unsigned long long& k = *v_char, bad_value_cast);
        CPPUNIT_ASSERT_THROW(unsigned long long& k = *v_bool, bad_value_cast);
        CPPUNIT_ASSERT_THROW(unsigned long long& k = *v_signedint, bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(unsigned long long& k = *v_unsignedint);
        CPPUNIT_ASSERT_THROW(unsigned long long& k = *v_float, bad_value_cast);
        CPPUNIT_ASSERT_THROW(unsigned long long& k = *v_string, bad_value_cast);
        CPPUNIT_ASSERT_THROW(unsigned long long& k = *v_array, bad_value_cast);
        CPPUNIT_ASSERT_THROW(unsigned long long& k = *v_map, bad_value_cast);
        CPPUNIT_ASSERT_THROW(unsigned long long& k = *v_binary, bad_value_cast);

        //Call const and call version
        CPPUNIT_ASSERT_THROW(const unsigned long long& k = const_cast<const Value&>(*v_empty), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const unsigned long long& k = const_cast<const Value&>(*v_char), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const unsigned long long& k = const_cast<const Value&>(*v_bool), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const unsigned long long& k = const_cast<const Value&>(*v_signedint), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(const unsigned long long& k = const_cast<const Value&>(*v_unsignedint));
        CPPUNIT_ASSERT_THROW(const unsigned long long& k = const_cast<const Value&>(*v_float), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const unsigned long long& k = const_cast<const Value&>(*v_string), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const unsigned long long& k = const_cast<const Value&>(*v_array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const unsigned long long& k = const_cast<const Value&>(*v_map), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const unsigned long long& k = const_cast<const Value&>(*v_binary), bad_value_cast);

        // Reference modification
        Value ULongLong(987654321ull);
        CPPUNIT_ASSERT(ULongLong == Value(987654321));
        unsigned long long& ref_var = ULongLong;
        ref_var = 123456789;
        CPPUNIT_ASSERT(ULongLong == Value(123456789));
    }

    void test_stringTypeOperator()
    {
        //Type std::string is a first-class type of Value, hence, we can obtain a reference
        CPPUNIT_ASSERT_THROW(std::string& k = *v_empty, bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string& k = *v_char, bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string& k = *v_bool, bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string& k = *v_signedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string& k = *v_unsignedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string& k = *v_float, bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(std::string& k = *v_string);
        CPPUNIT_ASSERT_THROW(std::string& k = *v_array, bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string& k = *v_map, bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string& k = *v_binary, bad_value_cast);

        //Call const and call version
        CPPUNIT_ASSERT_THROW(const std::string& k = const_cast<const Value&>(*v_empty), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const std::string& k = const_cast<const Value&>(*v_char), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const std::string& k = const_cast<const Value&>(*v_bool), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const std::string& k = const_cast<const Value&>(*v_signedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const std::string& k = const_cast<const Value&>(*v_unsignedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const std::string& k = const_cast<const Value&>(*v_float), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(const std::string& k = const_cast<const Value&>(*v_string));
        CPPUNIT_ASSERT_THROW(const std::string& k = const_cast<const Value&>(*v_array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const std::string& k = const_cast<const Value&>(*v_map), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const std::string& k = const_cast<const Value&>(*v_binary), bad_value_cast);

        // Reference modification
        Value String_A("Nice");
        CPPUNIT_ASSERT(String_A == Value("Nice"));
        std::string& ref_var = String_A;
        ref_var = "Great";
        CPPUNIT_ASSERT(String_A == Value("Great"));
    }

    void test_stringMoveOperation()
    {
        //For Move conversion test
        Value Null(*v_empty);
        Value Char(*v_char);
        Value Bool(*v_bool);
        Value SignedInt(*v_signedint);
        Value UnsignedInt(*v_unsignedint);
        Value Float(*v_float);
        Value String(*v_string);
        Value Array(*v_array);
        Value Map(*v_map);
        Value Binary(*v_binary);

        CPPUNIT_ASSERT_THROW(std::string&& k = std::move(Null), bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string&& k = std::move(Char), bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string&& k = std::move(Bool), bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string&& k = std::move(SignedInt), bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string&& k = std::move(UnsignedInt), bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string&& k = std::move(Float), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(std::string&& k = std::move(String));
        CPPUNIT_ASSERT_THROW(std::string&& k = std::move(Array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string&& k = std::move(Map), bad_value_cast);
        CPPUNIT_ASSERT_THROW(std::string&& k = std::move(Binary), bad_value_cast);

        //Assert Invariants are maintained after move failure
        CPPUNIT_ASSERT( *v_empty == Null    );
        CPPUNIT_ASSERT( *v_char == Char  );
        CPPUNIT_ASSERT( *v_bool == Bool );
        CPPUNIT_ASSERT( *v_signedint == SignedInt );
        CPPUNIT_ASSERT( *v_unsignedint == UnsignedInt );
        CPPUNIT_ASSERT( *v_float == Float );
        CPPUNIT_ASSERT( *v_string != String );
        CPPUNIT_ASSERT( *v_array == Array );
        CPPUNIT_ASSERT( *v_map == Map );
        CPPUNIT_ASSERT( *v_binary == Binary);

        // Move Assertion
        Value String_A("Nice");
        CPPUNIT_ASSERT(String_A == Value("Nice"));
        std::string var = std::move(String_A);
        CPPUNIT_ASSERT(String_A == Value(""));
        CPPUNIT_ASSERT(var == Value("Nice"));

        //Test unused rvalue conversion
        Value temp01 = "Hahah";
        auto&& temp0001 = static_cast<std::string&&>(std::move(temp01));
        (void)temp0001;
        CPPUNIT_ASSERT(temp01 == "Hahah");

    }

    void test_binaryTypeOperator()
    {
        //Type Value::BinaryType is a first-class type of Value, hence, we can obtain a reference
        CPPUNIT_ASSERT_THROW(Value::BinaryType& k = *v_empty, bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType& k = *v_char, bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType& k = *v_bool, bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType& k = *v_signedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType& k = *v_unsignedint, bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType& k = *v_float, bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType& k = *v_string, bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType& k = *v_array, bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType& k = *v_map, bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(Value::BinaryType& k = *v_binary);

        //Call const and call version
        CPPUNIT_ASSERT_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_empty), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_char), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_bool), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_signedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_unsignedint), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_float), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_string), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_map), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(const Value::BinaryType& k = const_cast<const Value&>(*v_binary));


        // Reference modification
        Value Binary = Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xdf)});
        CPPUNIT_ASSERT(Binary == Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xdf)}));
        Value::BinaryType& ref_var = Binary;
        ref_var[3] = T(0xFF);
        CPPUNIT_ASSERT(Binary == Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xff)}));
    }

    void test_binaryMoveOperation()
    {
        //For Move conversion test
        Value Null(*v_empty);
        Value Char(*v_char);
        Value Bool(*v_bool);
        Value SignedInt(*v_signedint);
        Value UnsignedInt(*v_unsignedint);
        Value Float(*v_float);
        Value String(*v_string);
        Value Array(*v_array);
        Value Map(*v_map);
        Value Binary(*v_binary);
/*
        CPPUNIT_ASSERT_THROW(Value::BinaryType&& k = std::move(Null), bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType&& k = std::move(Char), bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType&& k = std::move(Bool), bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType&& k = std::move(SignedInt), bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType&& k = std::move(UnsignedInt), bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType&& k = std::move(Float), bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType&& k = std::move(String), bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType&& k = std::move(Array), bad_value_cast);
        CPPUNIT_ASSERT_THROW(Value::BinaryType&& k = std::move(Map), bad_value_cast);
        CPPUNIT_ASSERT_NO_THROW(Value::BinaryType&& k = std::move(Binary));

        //Assert Invariants are maintained after move failure
        CPPUNIT_ASSERT( *v_empty == Null    );
        CPPUNIT_ASSERT( *v_char == Char  );
        CPPUNIT_ASSERT( *v_bool == Bool );
        CPPUNIT_ASSERT( *v_signedint == SignedInt );
        CPPUNIT_ASSERT( *v_unsignedint == UnsignedInt );
        CPPUNIT_ASSERT( *v_float == Float );
        CPPUNIT_ASSERT( *v_string == String );
        CPPUNIT_ASSERT( *v_array == Array );
        CPPUNIT_ASSERT( *v_map == Map );
        CPPUNIT_ASSERT( *v_binary != Binary);

        // Move Assertion
        Value Binary_A = Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xdf)});
        CPPUNIT_ASSERT(Binary_A == Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xdf)}));
        Value::BinaryType var = std::move(Binary_A);
        CPPUNIT_ASSERT(Binary_A == Value(Value::BinaryType()));
        CPPUNIT_ASSERT(var == Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xdf)}));
*/
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( Value_Conversion_Test );
