linux提供了一个叫pthread_once()的函数，它保证在一个进程中，某个函数只被执行一次。本例是使用pthread_once实现的线程安全的懒汉单例模式。
本例子在linux环境下运行shell脚本。

chmod 755 make.sh

./make.sh
