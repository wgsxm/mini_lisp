#include"builtins.h"
#include"value.h"
#include"eval_env.h"
ValuePtr add(const std::vector<ValuePtr>& params) {
    int result = 0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += i->asNumber().value();
    }
    return std::make_shared<NumericValue>(result);
}
ValuePtr print(const std::vector<ValuePtr>& params) {
    for (const auto&i:params) {
        std::cout << *i << std::endl;
    }
    return std::make_shared<NilValue>();
}
