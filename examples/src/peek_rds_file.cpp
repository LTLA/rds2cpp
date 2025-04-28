#include "rds2cpp/rds2cpp.hpp"
#include <string>
#include <iostream>

int main(int, char* argv[]) {
    auto finfo = rds2cpp::parse_rds(std::string(argv[1]), {});
    std::cout << static_cast<int>(finfo.object->type()) << std::endl;
    return 0;
}
