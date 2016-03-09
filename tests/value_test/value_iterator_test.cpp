#include "value.hpp"
#include <algorithm>
#include "../test_utils/format_helpers.hpp"
#include <cppunit/extensions/HelperMacros.h>

using namespace ubjson;
using namespace std;
int weird_cppunit_extern_bug_value_iterator_test = 0;

class Value_Iterator_Test : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( Value_Iterator_Test );
    CPPUNIT_TEST( test_find );
    CPPUNIT_TEST( test_keys );
    CPPUNIT_TEST( test_remove );
    CPPUNIT_TEST( test_iterator );
    CPPUNIT_TEST( test_modifying_iterator );
    CPPUNIT_TEST( test_iterator_on_algorithm );
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

    void test_find()
    {
        CPPUNIT_ASSERT( v_array->find(34.657) != v_array->end() );
        CPPUNIT_ASSERT( v_map->find("name") != v_map->end() );
        CPPUNIT_ASSERT( v_map->find("Name") == v_map->end() );
        CPPUNIT_ASSERT( v_string->find("Name") == v_string->end() );
        CPPUNIT_ASSERT( v_map->contains("name") );

        //Test const versions
        const Value* arr = v_array.get();
        const Value* mp = v_map.get();
        const Value* st = v_string.get();
        CPPUNIT_ASSERT( arr->find(34.657) != arr->end() );
        CPPUNIT_ASSERT( mp->find("name") != mp->end() );
        CPPUNIT_ASSERT( mp->find("Name") == mp->end() );
        CPPUNIT_ASSERT( st->find("Name") == st->end() );
    }

    void test_remove()
    {
        Value arr = *v_array;
        Value mp = *v_map;
        Value st = *v_string;

        //Tests for Array
        CPPUNIT_ASSERT_EQUAL(std::size_t(5), arr.size());
        arr.remove('m');
        CPPUNIT_ASSERT_EQUAL(std::size_t(5), arr.size());
        arr.remove('g');
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), arr.size());
        arr.remove(34.657);
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), arr.size());
        arr.push_back('g');
        CPPUNIT_ASSERT_EQUAL(std::size_t(4), arr.size());
        CPPUNIT_ASSERT_EQUAL("g"s, arr[3].asString());

        //Tests for Map
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), mp.size());
        mp.remove('m');
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), mp.size());
        mp.remove("id");
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), mp.size());
        mp.remove("extras");
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), mp.size());
        mp["extras"] = { *v_char, *v_signedint, *v_array };
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), mp.size());

        //Tests for String
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), st.size());
        st.remove('m');
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), st.size());
        st.remove("id");
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), st.size());
        st.remove("extras");
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), st.size());

        CPPUNIT_ASSERT( st.isString() );
        st = { *v_char, *v_signedint, *v_array };
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), st.size());
        CPPUNIT_ASSERT( st.isArray() );
    }

    void test_iterator()
    {
        Value ar(*v_array);
        Value mp(*v_map);
        const Value car(*v_array);
        const Value cmp(*v_map);

        int ar_i = 0;
        int mp_i = 0;
        int car_i = 0;
        int cmp_i = 0;

        for(auto p : ar)
            ++ar_i;
        for(auto p : car)
            ++car_i;
        for(auto p : mp)
            ++mp_i;
        for(auto p : cmp)
            ++cmp_i;
        CPPUNIT_ASSERT_EQUAL(5, ar_i);
        CPPUNIT_ASSERT_EQUAL(5, car_i);
        CPPUNIT_ASSERT_EQUAL(3, mp_i);
        CPPUNIT_ASSERT_EQUAL(3, cmp_i);
    }

    void test_keys()
    {
        Value::Keys k1 = v_float->keys();
        Value::Keys k2 = v_array->keys();
        Value::Keys k3 = v_map->keys();

        CPPUNIT_ASSERT_EQUAL(std::size_t(0), k1.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), k2.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), k3.size());

        CPPUNIT_ASSERT( v_map->contains(k3[0]) );
        CPPUNIT_ASSERT( v_map->contains(k3[1]) );
        CPPUNIT_ASSERT( v_map->contains(k3[2]) );
    }

    void test_modifying_iterator()
    {
        const double pi = 3.1416;
        Value arr{*v_array};

        std::for_each(arr.begin(), arr.end(),
                      [&pi](Value& v){ v = pi; } );

        for(auto& m : arr)
            CPPUNIT_ASSERT(m == Value(pi) );
    }

    void test_iterator_on_algorithm()
    {
        auto m = std::find(v_array->begin(), v_array->end(), Value(466));
        CPPUNIT_ASSERT( m != v_array->end() );
        CPPUNIT_ASSERT_EQUAL( "466"s, m->asString() );

        const Value const_v_array = *v_array;
        auto cm = std::find(const_v_array.begin(), const_v_array.end(), Value(466));
        CPPUNIT_ASSERT( cm != const_v_array.end() );
        CPPUNIT_ASSERT_EQUAL( "466"s, cm->asString() );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( Value_Iterator_Test );
