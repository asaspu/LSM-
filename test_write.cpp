#include "kvstore.h"
#include <unistd.h>
#include <iostream>

int main() {
    KVStore db("./crash_data");
    std::cout << "写入100条数据..." << std::endl;
    for (int i = 0; i < 100; i++) {
        db.Put("key_" + std::to_string(i), "value_" + std::to_string(i));
    }
    std::cout << "模拟崩溃" << std::endl;
    _exit(0);
}
