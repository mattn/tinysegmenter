#include "tinysegmenter.hxx"
#include <iostream>
#include <cassert>

void test_basic_segmentation() {
    tinysegmenter ts;
    auto result = ts.segment("私の名前は中野です");
    assert(result.size() == 6);
    assert(result[0] == "私");
    assert(result[1] == "の");
    assert(result[2] == "名前");
    assert(result[3] == "は");
    assert(result[4] == "中野");
    assert(result[5] == "です");
    std::cout << "test_basic_segmentation passed" << std::endl;
}

void test_numbers() {
    tinysegmenter ts;
    auto result = ts.segment("2023年12月31日");
    assert(result.size() == 6);
    assert(result[0] == "2023");
    assert(result[1] == "年");
    assert(result[2] == "12");
    assert(result[3] == "月");
    assert(result[4] == "31");
    assert(result[5] == "日");
    std::cout << "test_numbers passed" << std::endl;
}

void test_consecutive_numbers() {
    tinysegmenter ts;
    auto result = ts.segment("123456");
    assert(result.size() == 1);
    assert(result[0] == "123456");
    std::cout << "test_consecutive_numbers passed" << std::endl;
}

void test_katakana() {
    tinysegmenter ts;
    auto result = ts.segment("アリババと40人の盗賊");
    assert(result.size() == 6);
    assert(result[0] == "アリババ");
    assert(result[1] == "と");
    assert(result[2] == "40");
    assert(result[3] == "人");
    assert(result[4] == "の");
    assert(result[5] == "盗賊");
    std::cout << "test_katakana passed" << std::endl;
}

void test_preserve_list() {
    tinysegmenter ts;
    ts.set_preserve_list({"万博"});
    auto result = ts.segment("2025年大阪万博");
    assert(result.size() == 4);
    assert(result[0] == "2025");
    assert(result[1] == "年");
    assert(result[2] == "大阪");
    assert(result[3] == "万博");
    std::cout << "test_preserve_list passed" << std::endl;
}

void test_preserve_tokens() {
    tinysegmenter ts;
    ts.set_preserve_tokens(true);
    auto result = ts.segment("URLはhttps://example.comです");
    assert(result.size() == 4);
    assert(result[0] == "URL");
    assert(result[1] == "は");
    assert(result[2] == "https://example.com");
    assert(result[3] == "です");
    std::cout << "test_preserve_tokens passed" << std::endl;
}

int main() {
    test_basic_segmentation();
    test_numbers();
    test_consecutive_numbers();
    test_katakana();
    test_preserve_list();
    test_preserve_tokens();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
