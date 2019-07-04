g++ -std=c++11 -lpthread  TestClass.hpp TestMain.cpp -o test
killall -9 test 
./test
