#include <iostream>
#include <string>
#include "./tokenizer.h"
#include"./value.h"
#include"./parser.h"
#include "./rjsj_test.hpp"
struct TestCtx {
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        return value->toString();
    }
};
int main() {
    RJSJ_TEST(TestCtx, Lv2, Lv2Only);
    while (true) {
        try {
            std::cout << ">>> " ;
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.eof()) {
                std::exit(0);
            }
            auto tokens = Tokenizer::tokenize(line);
            Parser parser(std::move(tokens));  // TokenPtr 不支持复制
            auto value = parser.parse();
            std::cout << value->toString() << std::endl;
            /*for (auto& token : tokens) {
                std::cout << *token << std::endl;
            }*/
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

