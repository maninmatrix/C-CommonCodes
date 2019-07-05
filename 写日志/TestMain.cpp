
#include<iostream>
#include"log.h"
using namespace std;

int main() {
    InitializeLog(NULL, NULL);
    __log(_ERROR, __FUNCTION__, "hello");
    return 0;
}
