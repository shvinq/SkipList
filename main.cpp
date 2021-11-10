#include <iostream>
#include "SkipList.h"
#define FILE_PATH "./store/dumpFile"

int main() {

    SkipList<int, std::string> skipList(6);
	skipList.insert_element(1, "一个"); 
	skipList.insert_element(3, "简单的"); 
	skipList.insert_element(7, "基于"); 
	skipList.insert_element(8, "跳表"); 
	skipList.insert_element(9, "实现的"); 
	skipList.insert_element(19, "K-V型"); 
	skipList.insert_element(19, "数据库"); 

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.dump_file();

    // skipList.load_file();

    skipList.search_element(9);
    skipList.search_element(18);


    skipList.display();

    skipList.delete_element(3);
    skipList.delete_element(7);

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.display();
}