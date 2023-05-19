#ifndef VALUE_H
#define VALUE_H
#include <memory>
#include <string>
#include<iostream>
#include<optional>
#include"error.h"
#include<vector>
class Value;
using ValuePtr = std::shared_ptr<Value>;
class Value {
public:
    virtual ~Value() = default;
    virtual std::string toString() const = 0;
    virtual bool isSelfEvaluating() const {
        return false;
    }
    virtual bool isNil() const {
        return false;
    }
    virtual bool isPair() const {
        return false;
    }
    virtual std::optional<std::string> asSymbol() const{
        return std::nullopt;
    }
    virtual std::vector<ValuePtr> toVector() const {
        throw LispError("Not a Pair");
    }
};
std::ostream& operator<<(std::ostream& os, const Value& value);
class BooleanValue : public Value {
public:
    BooleanValue(bool value);
    std::string toString() const override;
    bool isSelfEvaluating() const override;

private:
    const bool m_value;
};
class NumericValue : public Value {
public:
    NumericValue(double value);
    std::string toString() const override;
    bool isSelfEvaluating() const override;

private:
    const double m_value;
};
class StringValue : public Value {
public:
    StringValue(const std::string& value);
    std::string toString() const override;
    bool isSelfEvaluating() const override;

private:
    const std::string m_value;
};
class NilValue : public Value {
public:
    NilValue() = default;
    std::string toString() const override;
    bool isNil() const override;
};
class SymbolValue : public Value {
public:
    SymbolValue(const std::string& name);
    std::string toString() const override;
    std::optional<std::string> asSymbol() const override;

private:
    const std::string m_name;
};
class PairValue : public Value {
public:
    PairValue(std::shared_ptr<Value> left, std::shared_ptr<Value> right);
    std::string toString() const override;
    std::shared_ptr<Value> m_left;
    std::shared_ptr<Value> m_right;
    bool isPair() const override;
    std::vector<ValuePtr> toVector() const override;
};
#endif
