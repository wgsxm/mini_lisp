#include "eval_env.h"
#include"error.h"
#include<vector>
#include<unordered_map>
#include <algorithm>
#include <iterator>
#include"value.h"
#include "./builtins.h"
using namespace std::literals; // 使用 s 后缀
std::unordered_map<std::string, ValuePtr> SymbolMap;
EvalEnv::EvalEnv() {
    SymbolMap.insert_or_assign("+", std::make_shared<BuiltinProcValue>(&add));
    SymbolMap.insert_or_assign("print", std::make_shared<BuiltinProcValue>(&print));
}
std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(), std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}
ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (typeid(*proc) == typeid(BuiltinProcValue)) {
        // 调用内置过程
        auto &process = static_cast<const BuiltinProcValue&>(*proc);
        ValuePtr ret = process.get_func()(args);
        return ret;
    } else {
        throw LispError("Unimplemented");
    }
}
ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating()) {
        return expr;
    } else if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    } else if (expr->isPair()) {
        std::vector<ValuePtr> v = expr->toVector();
        if (v[0]->asSymbol() == "define"s) {
            if (auto name = v[1]->asSymbol()) {
               // 将(*name, v[2]) 添加到符号表中;
                SymbolMap.insert_or_assign(name.value(), this->eval(v[2]));
               return std::make_shared<NilValue>();
            } else {
                throw LispError("Malformed define.");
            }
        } else {
            ValuePtr proc = this->eval(v[0]);
            std::vector<ValuePtr> args = evalList(expr->right());
            return this->apply(proc, args);  // 最后用 EvalEnv::apply 实现调用
        }
    } else if (auto name = expr->asSymbol()) {
        auto value = SymbolMap.find(name.value());
        if (value!=SymbolMap.end()) {
            return value->second;
        } else {
            throw LispError("Variable " + name.value() + " not defined.");
        }
    }
    else {
        throw LispError("Unimplemented");
    }
}
