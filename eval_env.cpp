#include "eval_env.h"
#include"error.h"
#include<vector>
#include<unordered_map>
using namespace std::literals; // 使用 s 后缀
std::unordered_map<std::string, ValuePtr> SymbolMap;
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
            throw LispError("Unimplemented");
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
