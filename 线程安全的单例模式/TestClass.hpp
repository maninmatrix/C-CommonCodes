#include<iostream>
using namespace std;
class TestClass
{
public:
    TestClass():iElemValue(0){};
    ~TestClass(){};
    void PrintElem(){
        cout<<"Elem Value:"<<iElemValue<<endl;
    }
    void AddElemValue(){
        iElemValue++;
    }
private:
    int iElemValue;
};
