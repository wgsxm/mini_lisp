#ifndef VALUE_H
#define VALUE_H
#include <memory>
#include <string>
#include<iostream>
class Value {
public:
    virtual ~Value() = default;
    virtual std::string toString() const = 0;
};
using ValuePtr = std::shared_ptr<Value>;
std::ostream& operator<<(std::ostream& os, const Value& value);
class BooleanValue : public Value {
public:
    BooleanValue(bool value);
    std::string toString() const override;
private:
    const bool m_value;
};
class NumericValue : public Value {
public:
    NumericValue(double value);
    std::string toString() const override;
private:
    const double m_value;
};
class StringValue : public Value {
public:
    StringValue(const std::string& value);
    std::string toString() const override;
private:
    const std::string m_value;
};
class NilValue : public Value {
public:
    NilValue() = default;
    std::string toString() const override;
};
class SymbolValue : public Value {
public:
    SymbolValue(const std::string& name);
    std::string toString() const override;
private:
    const std::string m_name;
};
class PairValue : public Value {
public:
    PairValue(std::shared_ptr<Value> left, std::shared_ptr<Value> right);
    std::string toString() const override;
    std::shared_ptr<Value> m_left;
    std::shared_ptr<Value> m_right;
};
#endif
