#include "eval_env.h"
#include"value.h"
#include "./builtins.h"
#include"forms.h"
using namespace std::literals; // 使用 s 后缀
EvalEnv::EvalEnv() {
    for (auto &i : BUILT_INS) {
        SymbolMap.insert_or_assign(
            i.first, std::make_shared<BuiltinProcValue>(i.second));
    }
}
EvalEnv::EvalEnv(std::shared_ptr<EvalEnv> parent) : parent{parent} {}
std::shared_ptr<EvalEnv> EvalEnv::createGlobal() {
    std::shared_ptr<EvalEnv> env(new EvalEnv);
    return env;
}
std::shared_ptr<EvalEnv> EvalEnv::createChild(
    const std::vector<std::string>& params, const std::vector<ValuePtr>& args) {
    std::shared_ptr<EvalEnv> env(new EvalEnv(this->shared_from_this()));
    for (int i = 0; i < params.size(); i++) {
            env->defineBinding(params[i], args[i]);
    }
    return env;
}

void EvalEnv::defineBinding(const std::string& name, ValuePtr ptr) {
    SymbolMap.insert_or_assign(name, ptr);
}
ValuePtr EvalEnv::lookupBinding(const std::string& name) {
    if (this->SymbolMap.count(name) > 0) {
        return SymbolMap.find(name)->second;
    } else if (this->parent != nullptr) {
        return parent->lookupBinding(name);
    } else {
        throw LispError("Variable " + name + " not defined");
    }
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
        ValuePtr ret = process.get_func()(args,*this);
        return ret;
    } else if (typeid(*proc) == typeid(LambdaValue)) {
        auto &process = static_cast<const LambdaValue &>(*proc);
        ValuePtr ret = process.apply(args);
        return ret;
    }
    else {
        throw LispError("Procedure Unimplemented");
    }
}
ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating()) {
        return expr;
    } else if (expr->isNil()) {
        throw LispError("Evaluating Nil");
    } else if (expr->isPair()) {
        std::vector<ValuePtr> v = expr->toVector();
        if (auto name = expr->left()->asSymbol()) {
            if (SPECIAL_FORMS.count(*name)>0) {
                return SPECIAL_FORMS.find(*name)->second(
                    expr->right()->toVector(), *this);
            } //如果是特殊形式
            else {
                ValuePtr proc = this->eval(v[0]);
                std::vector<ValuePtr> args = evalList(expr->right());
                return this->apply(proc,
                                   args);  // 最后用 EvalEnv::apply 实现调用
            }
        } 
        else {
            ValuePtr proc = this->eval(v[0]);
            std::vector<ValuePtr> args = evalList(expr->right());
            return this->apply(proc,
                               args);  // 最后用 EvalEnv::apply 实现调用
        }
    } else if (auto name = expr->asSymbol()) {
        return lookupBinding(*name);
    }
    else {
        throw LispError("Unimplemented Expression");
    }
}
