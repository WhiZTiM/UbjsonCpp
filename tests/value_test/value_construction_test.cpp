#include "value.hpp"
#include "../test_utils/format_helpers.hpp"
#include <cppunit/extensions/HelperMacros.h>

using namespace ubjson;
int weird_cppunit_extern_bug_value_construction_test = 0;

class Value_Construction_Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( Value_Construction_Test );
    CPPUNIT_TEST( testConstructors );
    CPPUNIT_TEST( testUniformBraceInitialization);
    CPPUNIT_TEST( testSizes );
    CPPUNIT_TEST( testEqaulity );
    CPPUNIT_TEST( testIneqaulity );
    CPPUNIT_TEST( testCopyConstruction );
    CPPUNIT_TEST( testMoveConstruction );
    CPPUNIT_TEST( testCopyAssignment );
    CPPUNIT_TEST( testMoveAssignment );
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
    void testConstructors()
    {
        CPPUNIT_ASSERT( v_empty->isNull() );
        CPPUNIT_ASSERT( v_char->isChar() );
        CPPUNIT_ASSERT( v_bool->isBool() );
        CPPUNIT_ASSERT( v_signedint->isSignedInteger() );
        CPPUNIT_ASSERT( v_unsignedint->isUnsignedInteger() );
        CPPUNIT_ASSERT( v_float->isFloat() );
        CPPUNIT_ASSERT( v_string->isString() );
        CPPUNIT_ASSERT( v_array->isArray() );
        CPPUNIT_ASSERT( v_map->isObject() );
        CPPUNIT_ASSERT( v_binary->isBinary() );

        CPPUNIT_ASSERT( v_signedint->isInteger() );
        CPPUNIT_ASSERT( v_unsignedint->isInteger() );

        CPPUNIT_ASSERT( v_signedint->isNumeric() );
        CPPUNIT_ASSERT( v_unsignedint->isNumeric() );
        CPPUNIT_ASSERT( v_float->isNumeric() );

        CPPUNIT_ASSERT( v_map->operator[]("extras").isArray() );
    }

    void testUniformBraceInitialization()
    {
        Value Null{};
        Value Char{'c'};
        Value Bool{true};
        Value SignedInt{-345};
        Value UnsignedInt{34543ull};
        Value Float{9.80665};
        Value String{"Love"};
        Value Array{*v_array};
        Value Map{*v_map};
        Value Binary{*v_binary};

        CPPUNIT_ASSERT( Null.isNull() );
        CPPUNIT_ASSERT( Char.isChar() );
        CPPUNIT_ASSERT( Bool.isBool() );
        CPPUNIT_ASSERT( SignedInt.isSignedInteger() );
        CPPUNIT_ASSERT( UnsignedInt.isUnsignedInteger() );
        CPPUNIT_ASSERT( Float.isFloat() );
        CPPUNIT_ASSERT( String.isString() );
        CPPUNIT_ASSERT( Array.isArray());
        CPPUNIT_ASSERT( Map.isMap() );
        CPPUNIT_ASSERT( Binary.isBinary() );
    }

    void testSizes()
    {
        CPPUNIT_ASSERT_EQUAL( std::size_t(0), v_empty->size() );
        CPPUNIT_ASSERT_EQUAL( std::size_t(1), v_char->size() );
        CPPUNIT_ASSERT_EQUAL( std::size_t(1), v_bool->size() );
        CPPUNIT_ASSERT_EQUAL( std::size_t(1), v_signedint->size() );
        CPPUNIT_ASSERT_EQUAL( std::size_t(1), v_unsignedint->size() );
        CPPUNIT_ASSERT_EQUAL( std::size_t(1), v_float->size() );
        CPPUNIT_ASSERT_EQUAL( std::size_t(1), v_string->size() );
        CPPUNIT_ASSERT_EQUAL( std::size_t(5), v_array->size() );
        CPPUNIT_ASSERT_EQUAL( std::size_t(3), v_map->size() );
        CPPUNIT_ASSERT_EQUAL( std::size_t(1), v_binary->size() );
    }

    void testEqaulity()
    {
        CPPUNIT_ASSERT( *v_empty == Value()    );
        CPPUNIT_ASSERT( *v_char == Value('c')  );
        CPPUNIT_ASSERT( *v_bool == Value(true) );
        CPPUNIT_ASSERT( *v_signedint == Value(-700) );
        CPPUNIT_ASSERT( *v_unsignedint == Value(800) );
        CPPUNIT_ASSERT( *v_float == Value(3.141600000000000) );
        CPPUNIT_ASSERT( *v_string == "string" );
        CPPUNIT_ASSERT( *v_array == Value({34.657, "Yeepa", 466, -53, 'g'}) );
        CPPUNIT_ASSERT( *v_binary == Value(Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xdf)})));

        Value map;
        map["name"] = "WhiZTiM";
        map["id"] = 12343;
        map["extras"] = { *v_array, "nice one bro!" };
        CPPUNIT_ASSERT( *v_map == map );
    }

    void testIneqaulity()
    {
        CPPUNIT_ASSERT( *v_empty != Value('c')    );
        CPPUNIT_ASSERT( *v_char != Value()  );
        CPPUNIT_ASSERT( *v_bool != Value(false) );
        CPPUNIT_ASSERT( *v_signedint != Value(700) );
        CPPUNIT_ASSERT( *v_unsignedint != Value(-800) );
        CPPUNIT_ASSERT( *v_float != Value(3.141600000000001) );
        CPPUNIT_ASSERT( *v_string != "strings" );
        CPPUNIT_ASSERT( *v_array != Value({34.6574, "Yeepa", 466, -53, 'g'}) );
        CPPUNIT_ASSERT( *v_map != Value({34.657, "Yeepa", 466, -53, 'g'}) );
        CPPUNIT_ASSERT( *v_binary != Value(Value::BinaryType({T(0xab), T(0xbc), T(0xcd), T(0xff)})));
    }

    void testCopyConstruction()
    {
        CPPUNIT_ASSERT( *v_empty == Value(*v_empty)    );
        CPPUNIT_ASSERT( *v_char == Value(*v_char)  );
        CPPUNIT_ASSERT( *v_bool == Value(*v_bool) );
        CPPUNIT_ASSERT( *v_signedint == Value(*v_signedint) );
        CPPUNIT_ASSERT( *v_unsignedint == Value(*v_unsignedint) );
        CPPUNIT_ASSERT( *v_float == Value(*v_float) );
        CPPUNIT_ASSERT( *v_string == Value(*v_string) );
        CPPUNIT_ASSERT( *v_array == Value(*v_array));
        CPPUNIT_ASSERT( *v_map == Value(*v_map));
        CPPUNIT_ASSERT( *v_binary == Value(*v_binary));
    }

    void testMoveConstruction()
    {

        Value Null_1(*v_empty);
        Value Char_1(*v_char);
        Value Bool_1(*v_bool);
        Value SignedInt_1(*v_signedint);
        Value UnsignedInt_1(*v_unsignedint);
        Value Float_1(*v_float);
        Value String_1(*v_string);
        Value Array_1(*v_array);
        Value Map_1(*v_map);
        Value Binary_1(*v_binary);

        //redo copy construction equality test
        CPPUNIT_ASSERT( *v_empty == Null_1    );
        CPPUNIT_ASSERT( *v_char == Char_1  );
        CPPUNIT_ASSERT( *v_bool == Bool_1 );
        CPPUNIT_ASSERT( *v_signedint == SignedInt_1 );
        CPPUNIT_ASSERT( *v_unsignedint == UnsignedInt_1 );
        CPPUNIT_ASSERT( *v_float == Float_1 );
        CPPUNIT_ASSERT( *v_string == String_1 );
        CPPUNIT_ASSERT( *v_array == Array_1);
        CPPUNIT_ASSERT( *v_map == Map_1 );
        CPPUNIT_ASSERT( *v_binary == Binary_1 );

        //Move items
        Value Null_2(std::move(Null_1));
        Value Char_2(std::move(Char_1));
        Value Bool_2(std::move(Bool_1));
        Value SignedInt_2(std::move(SignedInt_1));
        Value UnsignedInt_2(std::move(UnsignedInt_1));
        Value Float_2(std::move(Float_1));
        Value String_2(std::move(String_1));
        Value Array_2(std::move(Array_1));
        Value Map_2(std::move(Map_1));
        Value Binary_2(std::move(Binary_1));

        //Assert moved items are Null
        CPPUNIT_ASSERT( Null_1.isNull()         );
        CPPUNIT_ASSERT( Char_1.isNull()         );
        CPPUNIT_ASSERT( Bool_1.isNull()         );
        CPPUNIT_ASSERT( SignedInt_1.isNull()    );
        CPPUNIT_ASSERT( UnsignedInt_1.isNull()  );
        CPPUNIT_ASSERT( Float_1.isNull()        );
        CPPUNIT_ASSERT( String_1.isNull()       );
        CPPUNIT_ASSERT( Array_1.isNull()        );
        CPPUNIT_ASSERT( Map_1.isNull()          );
        CPPUNIT_ASSERT( Binary_1.isNull()          );

        //assert move-constructed items are same as original
        CPPUNIT_ASSERT( *v_empty == Null_2    );
        CPPUNIT_ASSERT( *v_char == Char_2  );
        CPPUNIT_ASSERT( *v_bool == Bool_2 );
        CPPUNIT_ASSERT( *v_signedint == SignedInt_2 );
        CPPUNIT_ASSERT( *v_unsignedint == UnsignedInt_2 );
        CPPUNIT_ASSERT( *v_float == Float_2 );
        CPPUNIT_ASSERT( *v_string == String_2 );
        CPPUNIT_ASSERT( *v_array == Array_2 );
        CPPUNIT_ASSERT( *v_map == Map_2 );
        CPPUNIT_ASSERT( *v_binary == Binary_2 );

    }

    void testMoveAssignment()
    {

        Value Null_to_Char(*v_empty);
        Value Char_to_Unsignedint(*v_char);
        Value Bool_to_Map(*v_bool);
        Value SignedInt_to_Array(*v_signedint);
        Value UnsignedInt_to_Null(*v_unsignedint);
        Value Float_to_Binary(*v_float);
        Value String_to_Bool(*v_string);
        Value Array_to_Float(*v_array);
        Value Map_to_Signedint(*v_map);
        Value Binary_to_String(*v_map);

        //make second copy, to be moved
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

        //move elements
        Null_to_Char = std::move(Char);
        Char_to_Unsignedint = std::move(UnsignedInt);
        Bool_to_Map = std::move(Map);
        SignedInt_to_Array = std::move(Array);
        UnsignedInt_to_Null = std::move(Null);
        Float_to_Binary = std::move(Binary);
        String_to_Bool = std::move(Bool);
        Array_to_Float = std::move(Float);
        Map_to_Signedint = std::move(SignedInt);
        Binary_to_String = std::move(String);

        //Assert Moved elements are nulled
        CPPUNIT_ASSERT( Null.isNull()         );
        CPPUNIT_ASSERT( Char.isNull()         );
        CPPUNIT_ASSERT( Bool.isNull()         );
        CPPUNIT_ASSERT( SignedInt.isNull()    );
        CPPUNIT_ASSERT( UnsignedInt.isNull()  );
        CPPUNIT_ASSERT( Float.isNull()        );
        CPPUNIT_ASSERT( String.isNull()       );
        CPPUNIT_ASSERT( Array.isNull()        );
        CPPUNIT_ASSERT( Map.isNull()          );
        CPPUNIT_ASSERT( Binary.isNull()          );

        //Assert move assignments
        CPPUNIT_ASSERT( *v_empty == UnsignedInt_to_Null    );
        CPPUNIT_ASSERT( *v_char == Null_to_Char  );
        CPPUNIT_ASSERT( *v_bool == String_to_Bool );
        CPPUNIT_ASSERT( *v_signedint == Map_to_Signedint );
        CPPUNIT_ASSERT( *v_unsignedint == Char_to_Unsignedint );
        CPPUNIT_ASSERT( *v_float == Array_to_Float );
        CPPUNIT_ASSERT( *v_string == Binary_to_String );
        CPPUNIT_ASSERT( *v_array == SignedInt_to_Array );
        CPPUNIT_ASSERT( *v_map == Bool_to_Map );
        CPPUNIT_ASSERT( *v_binary == Float_to_Binary );

    }

    void testCopyAssignment()
    {
        Value Null_to_Char(*v_empty);
        Value Char_to_Unsignedint(*v_char);
        Value Bool_to_Map(*v_bool);
        Value SignedInt_to_Array(*v_signedint);
        Value UnsignedInt_to_Null(*v_unsignedint);
        Value Float_to_Binary(*v_float);
        Value String_to_Bool(*v_string);
        Value Array_to_Float(*v_array);
        Value Map_to_Signedint(*v_map);
        Value Binary_to_String(*v_map);

        //assign new elements to them
        Null_to_Char = *v_char;
        Char_to_Unsignedint = *v_unsignedint;
        Bool_to_Map = *v_map;
        SignedInt_to_Array = *v_array;
        UnsignedInt_to_Null = *v_empty;
        Float_to_Binary = *v_binary;
        String_to_Bool = *v_bool;
        Array_to_Float = *v_float;
        Map_to_Signedint = *v_signedint;
        Binary_to_String = *v_string;

        //Assert assignments
        CPPUNIT_ASSERT( *v_empty == UnsignedInt_to_Null    );
        CPPUNIT_ASSERT( *v_char == Null_to_Char  );
        CPPUNIT_ASSERT( *v_bool == String_to_Bool );
        CPPUNIT_ASSERT( *v_signedint == Map_to_Signedint );
        CPPUNIT_ASSERT( *v_unsignedint == Char_to_Unsignedint );
        CPPUNIT_ASSERT( *v_float == Array_to_Float );
        CPPUNIT_ASSERT( *v_string == Binary_to_String );
        CPPUNIT_ASSERT( *v_array == SignedInt_to_Array );
        CPPUNIT_ASSERT( *v_map == Bool_to_Map );
        CPPUNIT_ASSERT( *v_binary == Float_to_Binary );

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( Value_Construction_Test );
