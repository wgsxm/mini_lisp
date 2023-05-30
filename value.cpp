#include "./value.h"
#include <cmath>
#include <iomanip>
#include<iostream>
#include <iomanip>
#include <sstream>
#include<vector>
#include"eval_env.h"
ValuePtr vec2pair(std::vector<ValuePtr> vec) {
    if (vec.empty()) {
        return std::make_shared<NilValue>();
    } else {
        auto left = vec[0];
        vec.erase(vec.begin());
        return std::make_shared<PairValue>(left, vec2pair(vec));
    }
}
bool Value::isTrue()const {
    return true;
}
bool Value::isNil() const {
    return false;
}
bool Value::isSelfEvaluating() const {
    return false;
}
bool Value::isPair() const {
    return false;
}
bool Value::isNumber() const {
    return false;
}
std::optional<double> Value::asNumber() const {
    return std::nullopt;
}
std::optional<std::string> Value::asSymbol() const {
    return std::nullopt;
}
std::vector<ValuePtr> Value::toVector() const {
    std::vector<ValuePtr> ret;
    return ret;
}
ValuePtr Value::left() const {
    return std::make_shared<NilValue>();
}
ValuePtr Value::right() const {
    return std::make_shared<NilValue>();
}
std::ostream& operator<<(
    std::ostream& os, const Value& value) {
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
bool BooleanValue::isTrue() const {
    return m_value;
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
bool NumericValue::isNumber() const {
    return true;
}
std::optional<double> NumericValue::asNumber() const {
    return this->m_value;
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
ValuePtr PairValue::left() const {
    return m_left;
}
ValuePtr PairValue::right() const {
    return m_right;
}
bool PairValue::isPair() const {
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
    if (typeid(*ptr) != typeid(NilValue)) ret.push_back(ptr);
    return ret;
}
BuiltinProcValue::BuiltinProcValue(BuiltinFuncType* func) : func{func} {}
std::string BuiltinProcValue::toString() const {
    return "#<procedure>";
}
BuiltinFuncType* BuiltinProcValue::get_func() const {
    return this->func;
}
std::string LambdaValue::toString() const{
    return "#<procedure>";
}
LambdaValue::LambdaValue(std::vector<std::string> params,
                         std::vector<ValuePtr> body,
                         std::shared_ptr<EvalEnv> env)
    : params{params}, body{body}, parent{env} {
}
ValuePtr LambdaValue::apply(const std::vector<ValuePtr>& args)const {
    auto childEnv = this->parent->createChild(this->params,args);
    ValuePtr result;
    //result = childEnv->eval(vec2pair(body));
    for (auto& expr : body) {
        result = childEnv->eval(expr);
    }
    return result;
}
