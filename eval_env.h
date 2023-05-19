#ifndef EVAL_ENV_H
#define EVAL_ENV_H
#include"./value.h"
#include<unordered_map>
class EvalEnv {


public:
    ValuePtr eval(ValuePtr expr);
};
#endif
