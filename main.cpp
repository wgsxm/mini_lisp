#include <iostream>
#include <string>
#include <fstream>
#include "./tokenizer.h"
#include"./value.h"
#include"./parser.h"
#include "./rjsj_test.hpp"
#include"eval_env.h"
#include"builtins.h"
struct TestCtx {
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    std::string eval(std::string input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env->eval(std::move(value));
        return result->toString();
    }
};
int main() {
    RJSJ_TEST(TestCtx, Lv2, Lv3,Lv4,Lv5,Lv5Extra,Lv6,Lv7,Lv7Lib,Sicp);
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();

    //std::string filePath = "lv7-answer.scm";  // 文件路径
    //std::ifstream inputFile(filePath);        // 打开文件
    //std::string line;
    //if (inputFile.is_open()) {
    //    // 逐行读取文件内容
    //    while (std::getline(inputFile, line)) {
    //        try {
    //            // 处理读取到的每一行数据
    //            auto tokens = Tokenizer::tokenize(line);
    //            Parser parser(std::move(tokens));  // TokenPtr 不支持复制
    //            auto value = parser.parse();
    //            env->eval(std::move(value));
    //        } catch (std::runtime_error& e) {
    //            std::cerr << "Error: " << e.what() << std::endl;
    //        }
    //    }
    //    inputFile.close();  // 关闭文件
    //} else {
    //    std::cerr << "Failed to open file: " << filePath << std::endl;
    //}
    
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
            auto result = env->eval(std::move(value));
            std::cout << result->toString() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

