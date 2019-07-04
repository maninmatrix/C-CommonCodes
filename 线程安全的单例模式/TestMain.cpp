#include<iostream>
#include"Singleton.h"
#include"TestClass.hpp"
using namespace std;
int main(int argc, char** argv) {
    singleton<TestClass>::GetInstance()->AddElemValue();
    singleton<TestClass>::GetInstance()->AddElemValue();
    singleton<TestClass>::GetInstance()->PrintElem();
    return 0;
}
