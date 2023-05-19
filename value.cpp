#include "./value.h"
#include <cmath>
#include <iomanip>
#include<iostream>
#include <iomanip>
#include <sstream>
#include<vector>
std::ostream& operator<<(std::ostream& os,const Value& value) {
    os <<value.toString();
    return os;
}
BooleanValue::BooleanValue(bool value) : m_value(value) {}
std::string BooleanValue::toString() const {
    return m_value ? "#t" : "#f";
}
bool BooleanValue::isSelfEvaluating() const {
    return true;
}
NumericValue::NumericValue(double value) : m_value(value) {}
std::string NumericValue::toString() const  {
    if (std::floor(m_value) == m_value) {
        return std::to_string(static_cast<int>(m_value));
    } else {
        return std::to_string(m_value);
    }
}
bool NumericValue::isSelfEvaluating() const {
    return true;
}
StringValue::StringValue(const std::string& value) : m_value(value) {}
std::string StringValue::toString() const {
    std::stringstream ss;
    ss<<std::quoted(m_value);
    return ss.str();
}
bool StringValue::isSelfEvaluating() const {
    return true;
}
std::string NilValue::toString() const {
    return std::string("()");
}
bool NilValue::isNil() const {
    return true;
}
SymbolValue::SymbolValue(const std::string& name) : m_name(name) {}
std::string SymbolValue::toString() const  {
    return m_name;
}
std::optional<std::string> SymbolValue::asSymbol() const {
    return this->toString();
}
PairValue::PairValue(std::shared_ptr<Value> left, std::shared_ptr<Value> right)
    : m_left(left), m_right(right) {}
std::string PairValue::toString() const {
    std::string result = "(";
    result += m_left->toString();
    std::shared_ptr<Value> ptr = m_right;
    while(typeid(*ptr) == typeid(PairValue)) {
        auto& pair = static_cast<const PairValue&>(*ptr);
        result += std::string(" ") + pair.m_left->toString();
        ptr = pair.m_right;
    } 
    if (typeid(*ptr) != typeid(NilValue)) {
        result += std::string(" . ") + ptr->toString();
    }
    return result + std::string(")");
}
bool PairValue::isPair() const{
    return true;
}
std::vector<ValuePtr> PairValue::toVector() const {
    std::vector<ValuePtr> ret;
    ret.push_back(this->m_left);
    std::shared_ptr<Value> ptr = m_right;
    while (typeid(*ptr) == typeid(PairValue)) {
        auto& pair = static_cast<const PairValue&>(*ptr);
        ret.push_back(pair.m_left);
        ptr = pair.m_right;
    }
    ret.push_back(ptr);
    return ret;
}
