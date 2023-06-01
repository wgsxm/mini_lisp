#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include"./value.h"
#include<unordered_map>
#include"error.h"
#include<vector>
#include <algorithm>
#include <iterator>
class EvalEnv:public std::enable_shared_from_this<EvalEnv> {
private:
    std::shared_ptr<EvalEnv> parent = nullptr;
    std::unordered_map<std::string, ValuePtr> SymbolMap;
    EvalEnv();
    EvalEnv(std::shared_ptr<EvalEnv>);
    public:
    bool isdefined(const std::string& expr);
    static std::shared_ptr<EvalEnv> createGlobal();
    std::shared_ptr<EvalEnv> createChild(
        const std::vector<std::string>& params,
        const std::vector<ValuePtr>& args);
    ValuePtr lookupBinding(const std::string&);
    void defineBinding(const std::string&, ValuePtr);
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
};
#endif
