 g++ -lpthread LogCls.cpp log.cpp TestMain.cpp IniFile.cpp -o writelog
 killall -9 writelog
 ./writelog