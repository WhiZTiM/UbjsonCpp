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

#include <chrono>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <type_traits>
#include "include/value.hpp"
#include "include/stream_reader.hpp"
#include "include/stream_writer.hpp"
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

using namespace std;
using namespace ubjson;

Value tst();
Value tst2();

int main()
{
    using namespace std::chrono;
    
    auto start = high_resolution_clock::now();
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry& registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    runner.run();

    cout << "Test time:" << (duration_cast<milliseconds>(high_resolution_clock::now() - start ).count()) << " milliseconds\n";

    extern int weird_cppunit_extern_bug_value_construction_test;    weird_cppunit_extern_bug_value_construction_test = 1;
    extern int weird_cppunit_extern_bug_value_conversion_test;      weird_cppunit_extern_bug_value_conversion_test = 1;
    extern int weird_cppunit_extern_bug_value_map_and_array_test;   weird_cppunit_extern_bug_value_map_and_array_test = 1;
    extern int weird_cppunit_extern_bug_value_iterator_test;        weird_cppunit_extern_bug_value_iterator_test = 1;

    Value v1 = tst(), v2 = tst2();
	cout << "V1 = " << to_ostream(v1) << '\n';
	cout << "V2 = " << to_ostream(v2) << '\n';
    cout << "Are the written values the same?: " << boolalpha << (v1 == v2) << endl;
    //cout << "FIRST\n====\n" << to_ostream(v1) << endl;
    //cout << "SECOND\n=====\n" << to_ostream(v2) << endl;

    return 0;

}


Value tst()
{
    Value v1;
    v1["name"] = "Ibrahim";
    v1["surname"] = "Onogu";
    v1["country"] = "NG";
    v1["faves"] = {453, -34, '@', true, /*-9.80665,*/ "So damn funny"};

    //v1["arrays"] = {v1, v1, v1};

    //cout << to_ostream(v1) << endl;

    std::ofstream file;
    file.open("tst.ubj", ios::binary);

    StreamWriter<decltype(file)> writer(file);
    auto result = writer.writeValue(v1);

    if(result.second)
        cout << "Successfully ";
    cout << "wrote " << result.first << " bytes" << endl;

    return v1;
}

Value tst2()
{
    Value v2;
    std::ifstream file;
    file.open("tst.ubj", ios::binary);
    //file.open("MediaTest.ubj", ios::binary);

    StreamReader<decltype(file)> reader(file);
    if(reader.getNextValue(v2))
    {
        cout << "Successfully READ:::\n-----------\n";
    }
    cout << reader.getBytesRead() << " bytes read\nERRORS: " << reader.getLastError() << endl << endl;
    //cout << to_ostream(v2) << endl;

    return v2;
}
