#include <iostream>
#include <string>
#include <fstream>
#include "./tokenizer.h"
#include"./value.h"
#include"./parser.h"
#include "./rjsj_test.hpp"
#include"eval_env.h"
#include"builtins.h"
#include<conio.h>
#include<Windows.h>
#include<stack>
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
bool ValidInput(const std::string& temp) {
    std::stack<char> store;
    for (char ch : temp) {
        if (ch == '(') {
            store.push(ch);
        } else if (ch == ')') {
            if (store.empty()) return false;
            store.pop();
        }
    }
    return store.empty();
}
void clearline(int length) {
    for (int i = 0; i < length; i++) {
        std::cout << char(8);
    }
    for (int i = 0; i < length; i++) {
        std::cout << char(32);
    }
    for (int i = 0; i < length; i++) {
        std::cout << char(8);
    }
}
int main() {
    //RJSJ_TEST(TestCtx, Lv2, Lv3,Lv4,Lv5,Lv5Extra,Lv6,Lv7,Lv7Lib,Sicp);
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    while (true) {
        try {
            SetConsoleTextAttribute(
                GetStdHandle(STD_OUTPUT_HANDLE),
                FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
            std::cout << ">>>";
            //处理字符串
            int temp_line = 0;
            std::vector<std::string> lines;
            std::vector<int> space;
            lines.push_back(std::string());
            space.push_back(3);
            while (true) {
                std::string& line = lines[temp_line];
                char ch = _getch();
                if (ch == 26 || ch == 23) exit(0);  // EOF
                if (ch == 0) {                      //输入F1等
                    ch = _getch();
                    continue;
                } else if (ch == -32) {  //输入方向,delete
                    ch = _getch();
                } else if (ch < 0) {
                    //非ascii
                    ch = _getch();
                    throw std::runtime_error("read non-ascii");
                }
                if (ch != 13)  //非回车
                {
                    if (ch != 8)  //不是退格
                    {
                        line += ch;
                        clearline(line.size() - 1);
                    } else if (!line.empty()) {
                        line.pop_back();
                        clearline(line.size() + 1);
                    } else {
                        if (temp_line == 0)
                            continue;
                        else {  //回到上一行
                            clearline(space[temp_line]);
                            temp_line--;
                            CONSOLE_SCREEN_BUFFER_INFO info;
                            GetConsoleScreenBufferInfo(
                                GetStdHandle(STD_OUTPUT_HANDLE), &info);
                            COORD cursorPosition;
                            cursorPosition.X = info.dwCursorPosition.X + 3 +
                                               lines[temp_line].size();
                            cursorPosition.Y = info.dwCursorPosition.Y - 1;
                            SetConsoleCursorPosition(
                                GetStdHandle(STD_OUTPUT_HANDLE),
                                cursorPosition);
                            continue;
                        }
                    }
                } else {  //回车
                    std::string ret;
                    for (auto& i : lines) {
                        ret += i;
                        ret += " ";
                    }
                    if (ValidInput(ret)) {//输出
                        std::cout << std::endl;
                        bool cer = 1;
                        for (int i = 0; i < ret.size(); i++) {
                            if (ret[i] != ' ') cer = 0;
                        }
                        if (cer) break;
                        auto tokens = Tokenizer::tokenize(ret);
                        Parser parser(std::move(tokens));  
                        auto value = parser.parse();
                        auto result = env->eval(std::move(value));
                        std::cout << result->toString() << std::endl;
                        Sleep(5);
                        break;
                    } else {
                        std::cout << std::endl << "...";  //继续输入
                        int temp_space = 0;               //控制缩进
                        lines.push_back(std::string());
                        temp_line++;
                        if (lines.size() == temp_line)
                            lines.push_back(std::string());
                        if (space.size() > temp_line)
                            space[temp_line] = 3 + temp_space;
                        else
                            space.push_back(3 + temp_space);
                        continue;
                    }
                }

                std::cout << line;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
    
    //while (true) {
    //    try {
    //        std::cout << ">>> " ;
    //        std::string line;
    //        std::getline(std::cin, line);
    //        if (std::cin.eof()) {
    //            std::exit(0);
    //        }
    //        auto tokens = Tokenizer::tokenize(line);
    //        Parser parser(std::move(tokens));  // TokenPtr 不支持复制
    //        auto value = parser.parse();
    //        auto result = env->eval(std::move(value));
    //        std::cout << result->toString() << std::endl;
    //        Sleep(5);
    //        //system("cls");
    //    } catch (std::runtime_error& e) {
    //        std::cerr << "Error: " << e.what() << std::endl;
    //    }
    //}
}

// std::string filePath = "lv7-answer.scm";  // 文件路径
// std::ifstream inputFile(filePath);        // 打开文件
// std::string line;
// if (inputFile.is_open()) {
//     // 逐行读取文件内容
//     while (std::getline(inputFile, line)) {
//         try {
//             // 处理读取到的每一行数据
//             auto tokens = Tokenizer::tokenize(line);
//             Parser parser(std::move(tokens));  // TokenPtr 不支持复制
//             auto value = parser.parse();
//             env->eval(std::move(value));
//         } catch (std::runtime_error& e) {
//             std::cerr << "Error: " << e.what() << std::endl;
//         }
//     }
//     inputFile.close();  // 关闭文件
// } else {
//     std::cerr << "Failed to open file: " << filePath << std::endl;
// }
