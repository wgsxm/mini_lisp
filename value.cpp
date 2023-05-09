#include "./value.h"
#include <cmath>
#include <iomanip>
BooleanValue::BooleanValue(bool value) : m_value(value) {}
std::string BooleanValue::toString() const {
    return m_value ? "#t" : "#f";
}
NumericValue::NumericValue(double value) : m_value(value) {}
std::string NumericValue::toString() const  {
    if (std::floor(m_value) == m_value) {
        return std::to_string(static_cast<int>(m_value));
    } else {
        return std::to_string(m_value);
    }
}
StringValue::StringValue(const std::string& value) : m_value(value) {}
std::string StringValue::toString() const  {
    return std::string("\"") + m_value + std::string("\"");
}
std::string NilValue::toString() const {
    return std::string("()");
}
SymbolValue::SymbolValue(const std::string& name) : m_name(name) {}
std::string SymbolValue::toString() const  {
    return m_name;
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
