#include "parser.h"
#include"error.h"
#include<iostream>
#include<deque>
Parser::Parser(std::deque<TokenPtr> tokens) : m_tokens{std::move(tokens)} {
}
ValuePtr Parser::parse() {
    auto token = std::move(m_tokens.front());
    m_tokens.pop_front();
    if (token->getType() == TokenType::NUMERIC_LITERAL) {
        auto value = static_cast<NumericLiteralToken&>(*token).getValue();
        return std::make_shared<NumericValue>(value);
    } else if (token->getType() == TokenType::BOOLEAN_LITERAL) {
        auto value = static_cast<BooleanLiteralToken&>(*token).getValue();
        return std::make_shared<BooleanValue>(value);
    } else if (token->getType() == TokenType::STRING_LITERAL) {
        auto value = static_cast<StringLiteralToken&>(*token).getValue();
        return std::make_shared<StringValue>(value);
    } else if (token->getType() == TokenType::IDENTIFIER) {
        auto value = static_cast<IdentifierToken&>(*token).getName();
        return std::make_shared<SymbolValue>(value);
    } else if (token->getType() == TokenType::LEFT_PAREN) {
        
        return parseTails();
    } else if (token->getType() == TokenType::QUOTE) {
        
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("quote"),std::make_shared<PairValue>(this->parse(),std::make_shared<NilValue>()));
    } else if (token->getType() == TokenType::UNQUOTE) {
      
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("unquote"),
            std::make_shared<PairValue>(this->parse(),
                                        std::make_shared<NilValue>()));
    } else if (token->getType() == TokenType::QUASIQUOTE) {
      
        return std::make_shared<PairValue>(
            std::make_shared<SymbolValue>("quasiquote"),
            std::make_shared<PairValue>(this->parse(),
                                        std::make_shared<NilValue>()));
    }
    else {
        throw SyntaxError("Unimplemented");
    }
}

ValuePtr Parser::parseTails() {
    if (m_tokens.empty()) {
        throw SyntaxError("Unexpected end of input");
    }
    if (m_tokens.front()->getType() == TokenType::RIGHT_PAREN) {
        m_tokens.pop_front();
        return std::make_shared<NilValue>();
    }
    auto car = this->parse();
    if (m_tokens.empty()) {
        throw SyntaxError("Unexpected end of input");
    }

    if (m_tokens.front()->getType() == TokenType::DOT) {
        m_tokens.pop_front();
        auto cdr = this->parse();
        if (m_tokens.empty() ||m_tokens.front()->getType() != TokenType::RIGHT_PAREN) {
            throw SyntaxError("Expected ')' after dot");
        }
        m_tokens.pop_front();
        return std::make_shared<PairValue>(car, cdr);
    } else {
        auto cdr = this->parseTails();
        return std::make_shared<PairValue>(car, cdr);
    }
}
