#include "kvstore.h"
#include <iostream>

int main() {
    KVStore db("./crash_data");
    int found = 0;
    std::string value;
    for (int i = 0; i < 100; i++) {
        if (db.Get("key_" + std::to_string(i), &value)) {
            found++;
        }
    }
    std::cout << "\n恢复: " << found << "/100 条数据" << std::endl;
    if (found == 100) {
        std::cout << "✅ 崩溃恢复成功！" << std::endl;
    } else {
        std::cout << "❌ 崩溃恢复失败" << std::endl;
    }
    return 0;
}
