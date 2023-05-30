#include"builtins.h"
#include"value.h"
#include"eval_env.h"

ValuePtr add(const std::vector<ValuePtr>& params) {
    double result = 0;
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
std::vector<double> value2num(const std::vector<ValuePtr>& params) {
    std::vector<double> ret;
    for (auto& value : params) {
        if (auto i = value->asNumber()) {
            ret.push_back(*i);
        } else {
            throw LispError("not number");
        }
    }
    return ret;
}
ValuePtr minus(const std::vector<ValuePtr>& params) {
    std::vector<double> nums = value2num(params);
    return std::make_shared<NumericValue>(nums[0] - nums[1]);
}
ValuePtr multiply(const std::vector<ValuePtr>& params) {
    double ret = 1;
    for (auto value :value2num(params)) {
        ret*=value;
    }
    return std::make_shared<NumericValue>(ret);
}
ValuePtr greater(const std::vector<ValuePtr>& params) {
    std::vector<double> nums = value2num(params);
    if (nums.size() != 2) {
        throw LispError("");
    }
    return std::make_shared<BooleanValue>(nums[0] > nums[1]);
}

const std::unordered_map<std::string, BuiltinFuncType*> BUILT_INS{
    {"+", &add},
    {"print", &print},
    {"*", &multiply},
    {">", &greater},
    {"-", &minus}};
