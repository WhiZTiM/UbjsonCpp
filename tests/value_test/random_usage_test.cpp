#include "value.hpp"
#include <iostream>

using namespace std;
using namespace ubjson;
void random_use_01()
{
    Value v{345};
    Value ha;
    Value array = { Value("name", "Joy"), Value("id", 34) };

    cout << "NAME: " << array[0]["name"].asString() << "\nid: " << array[1]["id"].asString() << endl;

    ha["sani"] = "Mark";
    ha["musa"] = "Yusuf";
    ha["kabir"] = 4546.34;
    cout << "Hello World!" << endl;
    double m = static_cast<int>(v) * static_cast<double>(ha["kabir"]) * 3.0;

    cout << "m: " << m << endl;
    cout << "value: " << ha["kabir"].asString() << endl << endl;

    for(auto a = ha.begin(); a != ha.end() ; a++)
    {
        cout << '\t' << a->asString() << endl;
    }

    for(const auto& a : ha)
        cout << '\t' << a.asFloat() << endl;

    cout << "Sizeof(Value::ValueHolder::Char): " << sizeof(Value::ValueHolder::Char) << " bytes\n";
    cout << "Sizeof(Value::ValueHolder::Bool): " << sizeof(Value::ValueHolder::Bool) << " bytes\n";
    cout << "Sizeof(Value::ValueHolder::SignedInt): " << sizeof(Value::ValueHolder::SignedInt) << " bytes\n";
    cout << "Sizeof(Value::ValueHolder::UnsignedInt): " << sizeof(Value::ValueHolder::UnsignedInt) << " bytes\n";
    cout << "Sizeof(Value::ValueHolder::Float): " << sizeof(Value::ValueHolder::Float) << " bytes\n";
    cout << "Sizeof(Value::ValueHolder::String): " << sizeof(Value::ValueHolder::String) << " bytes\n";
    cout << "Sizeof(Value::ValueHolder::Array): " << sizeof(Value::ValueHolder::Array) << " bytes\n";
    cout << "Sizeof(Value::ValueHolder::Binary): " << sizeof(Value::ValueHolder::Binary) << " bytes\n";
    cout << "Sizeof(Value::ValueHolder::Map): " << sizeof(Value::ValueHolder::Map) << " bytes\n\n";

    cout << "Sizeof(Value::ValueHolder): " << sizeof(Value::ValueHolder) << " bytes\n";
    cout << "Sizeof(Value): " << sizeof(Value) << " bytes\n";

}
