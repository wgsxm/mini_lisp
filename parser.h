#ifndef PARSER_H
#define PARSER_H
#include"value.h"
#include<deque>
#include"token.h"

class Parser {
private:
    std::deque<TokenPtr> m_tokens;

public:
    Parser(std::deque<TokenPtr> tokens);
    ValuePtr parse();
    ValuePtr parseTails();
};
#endif  // !PARSER_H
