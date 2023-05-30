#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include"./value.h"
#include<unordered_map>

class EvalEnv {
public:
    EvalEnv();
    ValuePtr eval(ValuePtr expr);
    std::vector<ValuePtr> evalList(ValuePtr expr);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);
};
#endif
