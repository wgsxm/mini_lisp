#ifndef BUILTINS_H
#define BUILTINS_H
#include"value.h"
#include<unordered_map>
extern const std::unordered_map<std::string, BuiltinFuncType*> BUILT_INS;
#endif  // !BUILTINS_H
