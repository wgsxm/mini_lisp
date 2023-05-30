#ifndef FORMS_H
#define FORMS_H
#include"value.h"
#include"eval_env.h"
using SpecialFormType = ValuePtr(const std::vector<ValuePtr>&, EvalEnv&);
extern const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS;
#endif  // !FORMS_H
