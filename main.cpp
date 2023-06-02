#include <iostream>
#include <string>
#include <fstream>
#include "./tokenizer.h"
#include"./value.h"
#include"./parser.h"
#include "./rjsj_test.hpp"
#include"eval_env.h"
#include"builtins.h"
#include"forms.h"
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
std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
bool ValidInput(const std::string& temp) {
    int quotes = 0;
    std::string no_string;
    bool isStr = false;
    for (char ch:temp) {
        if (ch == '\"') quotes++, isStr = !isStr;
        else if (isStr) {
            ;//do nothing
        } else if (ch == '(' || ch == ')')
            no_string.push_back(ch);}
    std::stack<char> store;
    for (char ch : no_string) {
        if (ch == '(') {
            store.push(ch);
        } else if (ch == ')') {
            if (store.empty()) return false;
            store.pop();
        }
    }
    return store.empty() && (!(quotes % 2));
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
void clearall(const std::vector<std::string>& lines) {
    setvbuf(stdout, NULL, _IOFBF, 4096);
    for (int i = 0; i < lines.size(); i++) {
        std::cout << '\r';
        for (int j = 0; j < 3 + lines[i].size()+10; j++) std::cout << ' ';
        if (i!=lines.size()-1)
        std::cout << '\n';
    }
    fflush(stdout);
    setvbuf(stdout, NULL, _IONBF, 4096);
}
const int COLOR_DEFAULT = 7;    // 默认颜色（白色）
const int COLOR_KEYWORD = 9;    // 关键字颜色（洋红色）
const int COLOR_STRING = 10;    // 字符串颜色（绿色）
const int COLOR_NUMBER = 6;     // 数字颜色（黄色）
const int COLOR_COMMENT = 14;   // 注释颜色（亮黄色）
const int COLOR_FUNCTION = 11;  // 函数名颜色（蓝色）
const int COLOR_VARIABLE = 13;   // 变量名颜色（青色）
const int COLOR_ERROR = 4;      //红色
const std::set<char> TOKEN_END{'(', ')', '\'', '`', ',', '"', ' ', ';'};
void setConsoleColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
bool is_keyword(const std::string& expr) {
    return SPECIAL_FORMS.count(expr);
}
bool is_number_or_boolean(const std::string& expr) {
    if (expr == "#t" || expr == "#f") return true;
    char* p = nullptr;
    double d = strtod(expr.c_str(), &p);
    return d != HUGE_VAL && *p == '\0';
}
bool is_function(const std::string& expr) {
    if (env->isdefined(expr)) {
        if (env->lookupBinding(expr)->toString() == "#<procedure>") return true;
    }
    return false;
}
bool is_variable(const std::string& expr) {
    if (env->isdefined(expr)) {
        if (env->lookupBinding(expr)->toString() != "#<procedure>") return true;
    }
    return false;
}
int highlight_expr(const std::string& expr) {
    if (is_number_or_boolean(expr)) return COLOR_NUMBER;
    if (is_keyword(expr)) return COLOR_KEYWORD;
    if (is_function(expr)) return COLOR_FUNCTION;
    return COLOR_DEFAULT;
}
void output_completion(const std::string& expr) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD currentAttributes = csbi.wAttributes;
    SetConsoleTextAttribute(hConsole, BACKGROUND_GREEN);
    std::cout << expr;
    SetConsoleTextAttribute(hConsole, currentAttributes);
}
bool is_similar(const std::string& symbol, const std::string& expr) {
    if (expr.size() > symbol.size()) return false;
    double same = 0;
    for (int i = 0; i < expr.size(); i++) {
        if (expr[i] == symbol[i])
            same += 1;
        else
            return false;
        if (same / symbol.size() >= 0.5) return true;
    }
    return false;
}
std::string code_complete(const std::string& expr) {
    std::vector<std::string> symbols;
    std::string ret;
    for (auto &i : env->getSymbol()) {
        symbols.push_back(i);
    }
    for (auto& i : getForm()) {
        symbols.push_back(i);
    }
    for (auto& symbol : symbols) {
        if (is_similar(symbol,expr)) {
            for (int i = expr.size(); i < symbol.size();i++) {
                ret += symbol[i];
            }
            output_completion(ret);
            break;
        }
    }
    return ret;
}
struct bracket {
    int place;
    int row;
    char ch;
};
bool hasMatchingBracket(const std::vector<std::string>& lines, int position,int row) {
    std::vector<bracket> symbols;
    bool isStr = false;
    for (int j = 0; j < lines.size(); j++)
    {
        //处理注释和string的情况
        auto& expr = lines[j];
        for (int i = 0; i < expr.size(); i++) {
            char ch = expr[i];
            if (ch =='\"') {
                isStr=!isStr;
            } else if (isStr)
                continue;
            else if (ch ==';') {
                break;
            }
            else if (ch == '(' || ch == ')') {
                symbols.push_back(bracket{i,j,ch});
            }
        }
    }
    std::stack<bracket> bracketStack;
    std::vector<bracket> wrong;
    for (auto& i : symbols) {
        if (i.ch == '(')
            bracketStack.push(i);
        else {
            if (bracketStack.empty()) {
                wrong.push_back(i);
            } else {
                bracketStack.pop();
            }
        }
    }
    while (!bracketStack.empty()) {
        bracket temp = bracketStack.top();
        bracketStack.pop();
        if (temp.place == position&&temp.row==row) return false;
    }
    for (auto& i:wrong) {
        if (i.place == position&&i.row==row) return false;
    }
    return true;
}
void outputBracket(const std::vector<std::string>& lines, int place,int row) {
    if (hasMatchingBracket(lines, place,row)) {
        setConsoleColor(COLOR_DEFAULT);
        std::cout << lines[row][place];
    }else {
        setConsoleColor(COLOR_ERROR);
        std::cout << lines[row][place];
        setConsoleColor(COLOR_DEFAULT);
    }
}
bool highlight_line(const std::vector< std::string>& lines,int row, bool isStr = false) {
    const std::string& line = lines[row];
    bool isComment = false;
    for (int i = 0; i < line.size(); i++) {
        char ch = line[i];
        if (isStr) {
            setConsoleColor(COLOR_STRING);
            if (ch != '\"')
                std::cout << ch;
            else {
                isStr = !isStr;
                std::cout << ch;
            }
        } else if (isComment) {
            setConsoleColor(COLOR_COMMENT);
            std::cout << ch;
        } else if (TOKEN_END.count(ch)) {
            if (ch == '\"') {
                isStr = !isStr;  //注意转义字符 to be added
                setConsoleColor(COLOR_STRING);
                std::cout << ch;
            } else if (ch == ';') {
                isComment = true;
                setConsoleColor(COLOR_COMMENT);
                std::cout << ch;
            } else if (ch == ')' || ch == '(') {
                outputBracket(lines,i,row);
            } 
            else {
                setConsoleColor(COLOR_DEFAULT);
                std::cout << ch;
            }
        } else {
            std::string temp;
            int j;
            for (j = i; j < line.size(); j++) {
                temp += line[j];
                if (TOKEN_END.count(line[j + 1])) {
                    i = j;
                    break;
                }
            }
            setConsoleColor(highlight_expr(temp));
            std::cout << temp;
            if (j == line.size()) break;
        }
    }
    setConsoleColor(COLOR_DEFAULT);
    return isStr;
}
std::string highlight_last(const std::vector<std::string>& lines, int row,
                           bool isStr = false) {
    const std::string& line = lines[row];
    bool isComment = false;
    for (int i = 0; i < line.size(); i++) {
        char ch = line[i];
        if (isStr) {
            setConsoleColor(COLOR_STRING);
            if (ch != '\"')
                std::cout << ch;
            else {
                isStr = !isStr;
                std::cout << ch;
            }
        } else if (isComment) {
            setConsoleColor(COLOR_COMMENT);
            std::cout << ch;
        } else if (TOKEN_END.count(ch)) {
            if (ch == '\"') {
                isStr = !isStr;  //注意转义字符 to be added
                setConsoleColor(COLOR_STRING);
                std::cout << ch;
            } else if (ch == ';') {
                isComment = true;
                setConsoleColor(COLOR_COMMENT);
                std::cout << ch;
            } else if (ch == ')' || ch =='(') {
                outputBracket(lines,i,row);
            }
            else {
                setConsoleColor(COLOR_DEFAULT);
                std::cout << ch;
            }
        } else {
            std::string temp;
            int j;
            for (j = i; j < line.size(); j++) {
                temp += line[j];
                if (TOKEN_END.count(line[j + 1])) {
                    i = j;
                    break;
                }
            }
            setConsoleColor(highlight_expr(temp));
            std::cout << temp;
            if (j == line.size()) {
                if ((!is_function(temp)) && (!is_variable(temp) )&& (!is_number_or_boolean(temp)))
                {
                    return code_complete(temp);
                }
                break;
            }
        }
    }
    setConsoleColor(COLOR_DEFAULT);
    return std::string();
}
std::string outputall(const std::vector<std::string>& lines,bool is_valid=false) {
    //输出所有
    bool isStr = false;
    for (int i = 0; i < lines.size(); i++) {
        //先清除
        clearline(lines[i].size() + 2 + 3);
        setConsoleColor(COLOR_DEFAULT);
        if (i == 0)
            std::cout << ">>>";
        else
            std::cout << "...";
        if (i != lines.size() - 1)
            isStr = highlight_line(lines,i, isStr);
        else if (!is_valid)
            return highlight_last(lines,i, isStr);
        else {
            highlight_line(lines,i, isStr);
            return std::string();
        }
        std::cout << std::endl;
    }
}
bool isLastRow(int row) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return row == csbi.dwSize.Y - 1;
}
bool isLineEnd(int column) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return column == csbi.dwSize.X - 1;
}
int find_next_par(const std::string& line, int place) {
    if (place >= line.size()) return -1;
    if (line[place] == '(') {
        for (int i = place+1; i < line.size(); i++) {
            {
                if (line[i] == '(') {
                    return i - place;
                }
            }
        }
    } else {
        for (int i = place; i < line.size(); i++) {
            {
                if (line[i] == '(') {
                    return i - place;
                }
            }
        }
    }
    return line.size()-place-1;
}
bool to_complete(const std::string& line, int place) {
    int i;
    for (i = line.size() - 1; i >= 0; i--) {
        if (TOKEN_END.count(line[i])) {
            return place > i;
        }
    }
    if (i == -1) return true;
    return false;
}
    int main() {
   //RJSJ_TEST(TestCtx, Lv2, Lv3,Lv4,Lv5,Lv5Extra,Lv6,Lv7,Lv7Lib,Sicp);
      
    while (true) {
        try {
            CONSOLE_SCREEN_BUFFER_INFO ori;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ori);
            COORD firstline;
            firstline.Y = ori.dwCursorPosition.Y;
            std::cout << ">>>";
            //处理字符串
            int temp_line = 0;
            std::string completion;
            std::vector<std::string> lines;
            lines.push_back(std::string());
            while (true) {
                if (isLastRow(firstline.Y + lines.size()-2)) {
                    firstline.Y--;
                    if (firstline.Y<0)
                    throw std::runtime_error("too many lines");
                }
                CONSOLE_SCREEN_BUFFER_INFO info;
                GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
                                           &info);
                COORD cursorPosition;
                cursorPosition.X = info.dwCursorPosition.X;
                cursorPosition.Y = info.dwCursorPosition.Y;
                if (isLineEnd(cursorPosition.X+1)) {
                    std::cout << std::endl;
                    throw std::runtime_error("input is too long");
                }
                std::string& line = lines[temp_line];
                char ch = _getch();
                firstline.X = 0;
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                                         firstline);
                clearall(lines);
                if (ch == 26 || ch == 23) exit(0);  // EOF
                if (ch == 0) {                      //输入F1等
                    firstline.X = 3 + lines[0].size();
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                                             firstline);
                    outputall(lines);
                    ch = _getch();
                    continue;
                } else if (ch == -32) {  //输入方向,delete
                    firstline.X = 3 + lines[0].size();
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                                             firstline);
                    outputall(lines);
                    //右77 左75 下80 上72
                    ch = _getch();
                    if (ch == 77) {
                        if (cursorPosition.X >= 3 + lines[temp_line].size())
                            ;
                        else
                            cursorPosition.X++;
                    }
                    if (ch == 75) {
                        if (cursorPosition.X == 3) {
                            if (lines[temp_line].empty()) {
                                if (temp_line == 0)
                                    ;
                                else {  //回到上一行
                                    lines.erase(lines.begin() + temp_line);
                                    clearline(3);
                                    temp_line--;
                                    cursorPosition.X =
                                        3 + lines[temp_line].size();
                                    cursorPosition.Y--;
                                }
                            }
                        } else
                            cursorPosition.X--;
                    }
                    if (ch == 80) {
                        if (temp_line == lines.size() - 1)
                            ;
                        else {
                            cursorPosition.Y++, temp_line++;
                            if (cursorPosition.X > 3 + lines[temp_line].size())
                                cursorPosition.X = 3 + lines[temp_line].size();
                        }
                    }
                    if (ch == 72) {
                        if (temp_line == 0) {
                            ;
                        } else {
                            if (line.empty() && temp_line == lines.size() - 1) {
                                clearline(3);
                                lines.erase(lines.begin() + temp_line);
                            }
                            cursorPosition.Y--;
                            temp_line--;
                            if (cursorPosition.X > 3 + lines[temp_line].size())
                                cursorPosition.X = 3 + lines[temp_line].size();
                        }
                    }
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                                             cursorPosition);
                    continue;
                } else if (ch < 0) {
                    firstline.X = 3 + lines[0].size();
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                                             firstline);
                    outputall(lines);
                    std::cout << std::endl;
                    //非ascii
                    ch = _getch();
                    throw std::runtime_error("read non-ascii");
                }
                if (ch != 13)  //非回车
                {
                    if (ch == 9)//tab
                    {
                        if (temp_line!=lines.size()-1||completion.empty()||!to_complete(line,cursorPosition.X-3-1)) {
                            if (temp_line != 0)
                            {
                                int space = find_next_par(lines[temp_line - 1],
                                                          cursorPosition.X - 3);
                                if (space > 0) {
                                    if (cursorPosition.X - 3 == line.size())
                                        for (int i = 0; i < space; i++) {
                                            lines[temp_line] += ' ';
                                            cursorPosition.X++;
                                        }
                                    else
                                        for (int i = 0; i < space; i++) {
                                            cursorPosition.X++;
                                        }
                                    } else {
                                        if (cursorPosition.X - 3 >= line.size())
                                            ;
                                        else {
                                            cursorPosition.X = line.size() + 3;
                                        }
                                    }
                            } else {
                                if (cursorPosition.X - 3 == line.size())
                                    ;
                                else {
                                    cursorPosition.X = line.size() + 3;
                                }
                            }    
                        }
                        else {
                            cursorPosition.Y = firstline.Y + lines.size() - 1;
                            lines[lines.size() - 1] += completion;
                            temp_line = lines.size() - 1;
                            cursorPosition.X =
                                3 + lines[lines.size() - 1].size();
                        }
                    }
                    else if (ch != 8)  //不是退格
                    {
                        if (cursorPosition.X == 3 + lines[temp_line].size())
                            line += ch;
                        else {
                            line.insert(line.begin() + (cursorPosition.X - 3),
                                        ch);
                        }
                        cursorPosition.X++;
                    } else if (!line.empty()) {
                        cursorPosition.X--;
                        if (cursorPosition.X == 3 + lines[temp_line].size())
                            line.pop_back();
                        else if (cursorPosition.X+1==3) {//如果在开头
                            cursorPosition.X++;
                            if (temp_line == 0)
                                ;
                            else {
                                cursorPosition.Y--;
                                lines[temp_line-1] += line;
                                lines.erase(lines.begin() + temp_line);
                                cursorPosition.X =
                                    3 + lines[temp_line - 1].size();
                                temp_line--;
                            }
                        }
                        else {
                            line.erase(line.begin() + (cursorPosition.X - 3));
                        }

                    } else {
                        if (temp_line == 0)
                            ;
                        else {  //回到上一行
                            lines.erase(lines.begin() + temp_line);
                            clearline(3);
                            temp_line--;
                            cursorPosition.X = 3 + lines[temp_line].size();
                            cursorPosition.Y--;
                        }
                    }
                } else {  //回车
                    //如果完成了表达式怎么办？to be added
                    std::string ret;
                    int isstr = 0;
                    for (auto& i : lines) {
                        std::string no_comment;
                        for (char ch : i) {
                            if (ch == '\"') isstr = !isstr;
                            if (ch != ';') {
                                no_comment += ch;
                            } else if (isstr) {
                                no_comment += ch;
                            }else
                                break;
                        }
                        isstr = 0;
                        for (char ch : no_comment) {
                            if (ch == '\"') isstr = !isstr;
                        }
                        ret += no_comment;
                        if (!isstr) ret += " ";
                    }
                    if (ValidInput(ret) && temp_line == lines.size() - 1 &&
                        cursorPosition.X == 3 + lines[temp_line].size()) {
                        firstline.X = 3 + lines[0].size();
                        SetConsoleCursorPosition(
                            GetStdHandle(STD_OUTPUT_HANDLE), firstline);
                        outputall(lines,true);
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
                    }
                    int temp_space = 3;  //控制缩进
                    if (lines.size() == temp_line + 1) {
                        if (cursorPosition.X ==
                            3 + lines[temp_line].size())  //在中间行的最后
                        {
                            lines.push_back(std::string());  //多一行
                        } else {
                            std::string temp;
                            int len = lines[temp_line].size();
                            for (int i = cursorPosition.X - 3;
                                 i < lines[temp_line].size(); i++) {
                                temp += lines[temp_line][i];
                            }
                            for (int i = cursorPosition.X - 3; i < len; i++) {
                                lines[temp_line].pop_back();
                            }
                            //	std::cout << temp;
                            lines.push_back(std::string(temp));
                        }
                    } else {
                        if (cursorPosition.X ==
                            3 + lines[temp_line].size())  //在中间行的最后
                        {
                            lines.insert(lines.begin() + temp_line + 1,
                                         std::string());  //多一行
                        } else {
                            std::string temp;
                            int len = lines[temp_line].size();
                            for (int i = cursorPosition.X - 3;
                                 i < lines[temp_line].size(); i++) {
                                temp += lines[temp_line][i];
                            }
                            for (int i = cursorPosition.X - 3; i < len; i++) {
                                lines[temp_line].pop_back();
                            }
                            lines.insert(lines.begin() + temp_line + 1,
                                         std::string(temp));
                        }
                    }
                    temp_line++;
                    cursorPosition.Y++;
                    cursorPosition.X = 3;
                }
                firstline.X = 3 + lines[0].size();
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                                         firstline);
                completion=outputall(lines);
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
                                         cursorPosition);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
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
