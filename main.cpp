#include "circularbuffer.h"

#include <iostream>

int main() {
    std::string s = "Hello, buffer!";

    CircularBuffer buffer(64);
    buffer.write((unsigned char *) s.c_str(), s.size());

    int idx = buffer.search((unsigned char *) "uf", 2);
    std::cout << s << std::endl;
    std::cout << "'uf' is at " << idx << std::endl;
    return 0;
}
