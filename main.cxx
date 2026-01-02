#include "tinysegmenter.hxx"
#include <iostream>

int main() {
    tinysegmenter ts;
    auto result = ts.segment("私の名前は中野です");
    for (const auto& token : result) {
        std::cout << token << std::endl;
    }
    return 0;
}
