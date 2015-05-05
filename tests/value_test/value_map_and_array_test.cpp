#include "value.hpp"
#include "../test_utils/format_helpers.hpp"
#include <cppunit/extensions/HelperMacros.h>

using namespace ubjson;
int weird_cppunit_extern_bug_value_map_and_array_test = 0;

class Value_Map_and_Array_Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( Value_Map_and_Array_Test );
    CPPUNIT_TEST( test_pushBack );
    CPPUNIT_TEST( test_IndexingOperator );
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

    void test_pushBack()
    {
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

        Null.push_back( "Sweet" ); Value m; m.push_back("Sweet");
        CPPUNIT_ASSERT( Null.isArray() );
        CPPUNIT_ASSERT_EQUAL(size_t(1), Null.size());
        CPPUNIT_ASSERT( m == Null );

        Char.push_back( "Sweet" );
        CPPUNIT_ASSERT( Char.isArray() );
        CPPUNIT_ASSERT_EQUAL(size_t(2), Char.size());
        CPPUNIT_ASSERT( Value({'c', "Sweet"}) == Char );

        Bool.push_back( Char );
        CPPUNIT_ASSERT( Bool.isArray() );
        CPPUNIT_ASSERT_EQUAL(size_t(2), Bool.size());
        CPPUNIT_ASSERT( Value({true, {'c', "Sweet"}}) == Bool );

        SignedInt.push_back( std::move(Bool) );
        CPPUNIT_ASSERT( SignedInt.isArray() );
        CPPUNIT_ASSERT_EQUAL(size_t(2), SignedInt.size());
        CPPUNIT_ASSERT( Value(
                                { -700,
                                  { true,
                                     {'c', "Sweet"}
                                  }
                                }
                            ) == SignedInt );
        CPPUNIT_ASSERT(Bool.isNull());

        //CPPUNIT_ASSERT_THROW( String )

    }

    void test_IndexingOperator()
    {
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

        //test Array indexing operator
        CPPUNIT_ASSERT_THROW(Null[0], value_exception);
        CPPUNIT_ASSERT_THROW(Char[2], value_exception);
        CPPUNIT_ASSERT_THROW(Bool[7], value_exception);
        CPPUNIT_ASSERT_THROW(SignedInt[0], value_exception);
        CPPUNIT_ASSERT_THROW(UnsignedInt[0], value_exception);
        CPPUNIT_ASSERT_THROW(Float[0], value_exception);
        CPPUNIT_ASSERT_THROW(String[0], value_exception);
        CPPUNIT_ASSERT_NO_THROW(Array[0] );
        CPPUNIT_ASSERT_THROW(Map[0], value_exception);
        CPPUNIT_ASSERT_THROW(Binary[0], value_exception);


        CPPUNIT_ASSERT_EQUAL( std::size_t(3), Map.size() );
        //Test Map insexing Operator
        CPPUNIT_ASSERT_NO_THROW(Null["haha"]);
        CPPUNIT_ASSERT_THROW(Char["haha"], value_exception);
        CPPUNIT_ASSERT_THROW(Bool["haha"], value_exception);
        CPPUNIT_ASSERT_THROW(SignedInt["haha"], value_exception);
        CPPUNIT_ASSERT_THROW(UnsignedInt["haha"], value_exception);
        CPPUNIT_ASSERT_THROW(Float["haha"], value_exception);
        CPPUNIT_ASSERT_THROW(String["haha"], value_exception);
        CPPUNIT_ASSERT_THROW(Array["haha"], value_exception );
        CPPUNIT_ASSERT_NO_THROW(Map["haha"]);
        CPPUNIT_ASSERT_THROW(Binary["haha"], value_exception);
        CPPUNIT_ASSERT_EQUAL( std::size_t(4), Map.size() );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( Value_Map_and_Array_Test );
