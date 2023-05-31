#include"builtins.h"
#include"value.h"
#include"eval_env.h"
#include<cmath>
std::vector<double> value2num(const std::vector<ValuePtr>& params) {
    std::vector<double> ret;
    for (auto& value : params) {
        if (auto i = value->asNumber()) {
            ret.push_back(*i);
        } else {
            throw LispError("non-numeric value exist");
        }
    }
    return ret;
}

ValuePtr add(const std::vector<ValuePtr>& params,EvalEnv& env) {
    double result = 0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value");
        }
        result += i->asNumber().value();
    }
    return std::make_shared<NumericValue>(result);
}
ValuePtr minus(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 2) throw LispError("too many values to minus");
    if (params.size() == 0) throw LispError("too few values to minus");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot minus a non-numeric value");
        }
    }
    std::vector<double> nums = value2num(params);
    if (nums.size() == 1) return std::make_shared<NumericValue>(-nums[0]);
    return std::make_shared<NumericValue>(nums[0] - nums[1]);
}
ValuePtr multiply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double ret = 1;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot multiply a non-numeric value");
        }
    }
    for (auto value : value2num(params)) {
        ret *= value;
    }
    return std::make_shared<NumericValue>(ret);
}
ValuePtr divide(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 2) throw LispError("too many values to divide");
    if (params.size() == 0) throw LispError("too few values to divide");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot divide a non-numeric value");
        }
    }
    std::vector<double> nums = value2num(params);
    if (nums.size() == 1) {
        if (nums[0] == 0) throw LispError("divide by zero");
        return std::make_shared<NumericValue>(1 / nums[0]);
    } else {
        if (nums[1] == 0) throw LispError("divide by zero");
        return std::make_shared<NumericValue>(nums[0] / nums[1]);
    }
}
ValuePtr greater(const std::vector<ValuePtr>& params, EvalEnv& env) {
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot compare non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    if (nums.size() > 2) {
        throw LispError("too many values to compare");
    } else if (nums.size() < 2) {
        throw LispError("too few values to compare");
    }
    return std::make_shared<BooleanValue>(nums[0] > nums[1]);
}
ValuePtr abs(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 1) throw LispError("can only get abs for one value");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot get abs for a non-numeric value");
        }
    }
    std::vector<double> nums = value2num(params);
    return std::make_shared<NumericValue>(std::abs(nums[0]));
}
ValuePtr expt(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() !=2) throw LispError("can only get expt for two values");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot get expt for non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    if (nums[0] == 0 && nums[1] == 0)
        throw LispError("zero raised to the power of zero");
    return std::make_shared<NumericValue>(std::pow(nums[0], nums[1]));
}
int quotient(double x, double y) {
    if (y == 0) throw LispError("divide by zero");
    double temp = x / y;
    if (temp >= 0)
        temp = int(temp);
    else
        temp = std::ceil(temp);
    return temp;
}
bool is_int(double x) {
    return std::floor(x) == x;
}
ValuePtr quotient(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2) throw LispError("can only get quotient for two values");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot get quotient for non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    return std::make_shared<NumericValue>(quotient(nums[0],nums[1]));
}
ValuePtr modulo(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2)
        throw LispError("can only get modulo for two values");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot get modulo for non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    int x = nums[0], y = nums[1];
    if (y == 0) throw LispError("mod by zero");
    int temp = x % y;
    if (x * y < 0) temp += y;
    temp = std::abs(temp);
    return std::make_shared<NumericValue>(temp * (std::abs(y) / y));
    }
bool equal(const ValuePtr& a, const ValuePtr& b) {
        bool ret = false;
        if (typeid(*a) != typeid(*b)) {
            ret = false;
        } else {
            if (typeid(*a) == typeid(NilValue))
                ret = true;
            else if (typeid(*a) == typeid(BooleanValue)) {
                auto& a_ = static_cast<const BooleanValue&>(*a);
                auto& b_ = static_cast<const BooleanValue&>(*b);
                ret = a_.getValue() == b_.getValue();
            } else if (typeid(*a) == typeid(NumericValue)) {
                auto& a_ = static_cast<const NumericValue&>(*a);
                auto& b_ = static_cast<const NumericValue&>(*b);
                ret = a_.getNum() == b_.getNum();
            } else if (typeid(*a) == typeid(StringValue)) {
                auto& a_ = static_cast<const StringValue&>(*a);
                auto& b_ = static_cast<const StringValue&>(*b);
                ret = a_.getStr() == b_.getStr();
            } else if (typeid(*a) == typeid(SymbolValue)) {
                auto& a_ = static_cast<const SymbolValue&>(*a);
                auto& b_ = static_cast<const SymbolValue&>(*b);
                ret = a_.getName() == b_.getName();
            } else if (typeid(*a) == typeid(PairValue)) {
                //pair
                ret = equal(a->left(), b->left()) &&
                      equal(a->right(), b->right());
            } else
                throw LispError("cannot compare procedure");
        }
        return ret;
    }
ValuePtr eq(const std::vector<ValuePtr>& params, EvalEnv& env) {
        if (params.size() > 2) throw LispError("too many values to compare");
        else if (params.size() < 2)
            throw LispError("too few values to compare");
    if (typeid(*params[0]) != typeid(*params[1]))
        return std::make_shared<BooleanValue>(false);
    if (typeid(*params[0]) == typeid(BooleanValue) ||
        typeid(*params[0]) == typeid(NumericValue) ||
        typeid(*params[0]) == typeid(StringValue) ||
        typeid(*params[0]) == typeid(SymbolValue) ||
        typeid(*params[0]) == typeid(NilValue))
        return std::make_shared<BooleanValue>(equal(params[0], params[1]));
    return std::make_shared<BooleanValue>(params[0]==params[1]);
}
ValuePtr equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 2)
        throw LispError("too many values to compare");
    else if (params.size() < 2)
        throw LispError("too few values to compare");
    return std::make_shared<BooleanValue>(equal(params[0], params[1]));
}
ValuePtr equal_num(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 2)
        throw LispError("too many values to compare");
    else if (params.size() < 2)
        throw LispError("too few values to compare");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot compare non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    return std::make_shared<BooleanValue>(nums[0] == nums[1]);
}
ValuePtr less(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 2)
        throw LispError("too many values to compare");
    else if (params.size() < 2)
        throw LispError("too few values to compare");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot compare non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    return std::make_shared<BooleanValue>(nums[0]< nums[1]);
}
ValuePtr less_equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 2)
        throw LispError("too many values to compare");
    else if (params.size() < 2)
        throw LispError("too few values to compare");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot compare non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    return std::make_shared<BooleanValue>(nums[0] <= nums[1]);
}
ValuePtr greater_equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 2)
        throw LispError("too many values to compare");
    else if (params.size() < 2)
        throw LispError("too few values to compare");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot compare non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    return std::make_shared<BooleanValue>(nums[0] >= nums[1]);
}
ValuePtr even(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 1)
        throw LispError("too many values to assert");
    else if (params.size() < 1)
        throw LispError("too few values to assert");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot assert non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    return std::make_shared<BooleanValue>(int(nums[0])%2==0);
}
ValuePtr odd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 1)
        throw LispError("too many values to assert");
    else if (params.size() < 1)
        throw LispError("too few values to assert");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot assert non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    return std::make_shared<BooleanValue>(int(std::abs(nums[0])) % 2 == 1);
}
ValuePtr zero(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() > 1)
        throw LispError("too many values to assert");
    else if (params.size() < 1)
        throw LispError("too few values to assert");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot assert non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    return std::make_shared<BooleanValue>(nums[0] == 0);
}
ValuePtr Not(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() >1)
        throw LispError("too many values to assert");
    else if (params.size() <1)
        throw LispError("too few values to assert");
    return std::make_shared<BooleanValue>(!params[0]->isTrue());
}
ValuePtr remainder(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() != 2)
        throw LispError("can only get remainder for two values");
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot get remainder for non-numeric values");
        }
    }
    std::vector<double> nums = value2num(params);
    int x = nums[0], y = nums[1];
    return std::make_shared<NumericValue>(x % y);
}
ValuePtr print(const std::vector<ValuePtr>& params, EvalEnv& env) {
    for (const auto& i : params) {
        std::cout << *i << std::endl;
    }
    return std::make_shared<NilValue>();
}
ValuePtr apply(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 2)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 2)
        throw LispError("excessive number of arguments");
    return env.apply(params[0], params[1]->toVector());
}
ValuePtr display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() <1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    if (params[0]->isStr()) {
        auto &ret = static_cast<const StringValue&>(*params[0]);
        std::cout << ret.getStr();
    } else {
        std::cout <<'\''<< *params[0];
    }
    return std::make_shared<NilValue>();
}
ValuePtr newline(const std::vector<ValuePtr>& params, EvalEnv& env) {
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}
ValuePtr displayln(const std::vector<ValuePtr>& params, EvalEnv& env) {
    display(params, env);
    newline(params, env);
    return std::make_shared<NilValue>();
}
ValuePtr error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) {
        throw LispError("");
    } else {
        if (params[0]->isStr()) {
            auto& ret = static_cast<const StringValue&>(*params[0]);
            throw LispError(ret.getStr());
        } else {
            throw LispError(params[0]->toString());
        }
    }
}
ValuePtr exit(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) {
        exit(0);
    } else {
        exit(*params[0]->asNumber());
    }
}
ValuePtr eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return env.eval(params[0]);
}
ValuePtr is_atom(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    if (typeid(*params[0]) == typeid(BooleanValue)||
        typeid(*params[0]) == typeid(NumericValue) ||
        typeid(*params[0]) == typeid(StringValue) ||
        typeid(*params[0]) == typeid(SymbolValue) ||
        typeid(*params[0]) == typeid(NilValue) )
        return std::make_shared<BooleanValue>(true);
    return std::make_shared<BooleanValue>(false);
}
ValuePtr is_boolean(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return std::make_shared<BooleanValue>(typeid(*params[0])==typeid(BooleanValue));
}
ValuePtr is_integer(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    if (typeid(*params[0]) == typeid(NumericValue)){
        auto& num = static_cast<const NumericValue&>(*params[0]);
        return std::make_shared<BooleanValue>(num.isInt());
    }
    return std::make_shared<BooleanValue>(false);
}
bool is_list(const ValuePtr& pair) {

    if (pair->isNil()) return true;
    else {
        if (pair->isPair()) {
            return is_list(pair->right());
        } else
            return false;
    }
}
ValuePtr is_list(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return std::make_shared<BooleanValue>(is_list(params[0]));
}
ValuePtr is_num(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return std::make_shared<BooleanValue>(params[0]->isNumber());
}
ValuePtr is_null(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return std::make_shared<BooleanValue>(params[0]->isNil());
}
ValuePtr is_pair(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return std::make_shared<BooleanValue>(params[0]->isPair());
}
ValuePtr is_procedure(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return std::make_shared<BooleanValue>(typeid(*params[0]) ==
                                          typeid(BuiltinProcValue)||typeid(*params[0])==typeid(LambdaValue));
}
ValuePtr is_string(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return std::make_shared<BooleanValue>(params[0]->isStr());
}
ValuePtr is_symbol(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return std::make_shared<BooleanValue>(typeid(*params[0]) ==
                                          typeid(SymbolValue));
}


ValuePtr append(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) return std::make_shared<NilValue>();
    std::vector<ValuePtr> ret;
    for (auto& i : params) {
        for (auto& j : i->toVector()) {
            ret.push_back(j);
        }
    }
    return vec2pair(ret);
}
ValuePtr car(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    if (!params[0]->isPair()) throw LispError("can only get car for a pair");
    return params[0]->left();
}
ValuePtr cdr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    if (!params[0]->isPair()) throw LispError("can only get cdr for a pair");
    return params[0]->right();
}
ValuePtr cons(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 2)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 2)
        throw LispError("excessive number of arguments");
    return std::make_shared<PairValue>(params[0], params[1]);
}
ValuePtr length(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 1)
        throw LispError("excessive number of arguments");
    return std::make_shared<NumericValue>(params[0]->toVector().size());
}
ValuePtr list(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return vec2pair(params);
}
ValuePtr map(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 2)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 2)
        throw LispError("excessive number of arguments");
    std::vector<ValuePtr> data = params[1]->toVector(), ret;
    for (auto& i : data) {
        if (typeid(*i) == typeid(PairValue)) {
            ret.push_back(env.apply(params[0], i->toVector()));
        } else {
            std::vector<ValuePtr> temp;
            temp.push_back(i);
            ret.push_back(env.apply(params[0], temp));
        }
    }
    return vec2pair(ret);
}
ValuePtr filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 2)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 2)
        throw LispError("excessive number of arguments");
    std::vector<ValuePtr> data = params[1]->toVector(), ret;
    for (auto& i : data) {
        std::vector<ValuePtr> temp;
        temp.push_back(i);
        auto store = env.apply(params[0],temp);
        if (store->isTrue()) ret.push_back(i);
    }
    return vec2pair(ret);
}
ValuePtr reduce(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() < 2)
        throw LispError("insufficient number of arguments");
    else if (params.size() > 2)
        throw LispError("excessive number of arguments");
    std::vector<ValuePtr> data = params[1]->toVector();
    if (data.empty()) throw LispError("reduce Nil");
    if (data.size() == 1) {
        std::vector<ValuePtr> temp;
        temp.push_back(params[1]);
        return car(temp, env);
    } else {
        std::vector<ValuePtr> temp;
        temp.push_back(params[1]);
        std::vector<ValuePtr> args;
        args.push_back(car(temp,env));
        std::vector<ValuePtr> next;
        next.push_back(params[0]);
        next.push_back(cdr(temp, env));
        args.push_back(reduce(next, env));
        return env.apply(params[0],args);
    }
}
    const std::unordered_map<std::string, BuiltinFuncType*> BUILT_INS{
    {"+", &add},
    {"print", &print},
    {"*", &multiply},
    {">", &greater},
    {"-", &minus},
    {"apply", &apply},
    {"display", &display},
    {"newline", &newline},
    {"displayln", &displayln},
    {"error", &error},
    {"exit", &exit},
    {"eval", &eval},
    {"atom?", &is_atom},
    {"boolean?", &is_boolean},
    {"integer?", &is_integer},
    {"list?", &is_list},
    {"number?", &is_num},
    {"null?", &is_null},
    {"pair?", &is_pair},
    {"procedure?", &is_procedure},
    {"string?", &is_string},
    {"symbol?", &is_symbol},
    {"/", &divide},
    {"abs", &abs},
    {"expt", &expt},
    {"quotient", &quotient},
    {"modulo", &modulo},
    {"remainder", &remainder},
    {"eq?", &eq},
    {"equal?", &equal},
    {"=", &equal_num},
    {"not", &Not},
    {"<", &less},
    {"<=", &less_equal},
    {">=", &greater_equal},
    {"even?", &even},
    {"odd?", &odd},
    {"zero?", &zero},
    {"append", &append},
    {"car", &car},
    {"cdr", &cdr},
    {"cons", &cons},
    {"length", &length},
    {"list", &list},
    {"map", &map},
    {"filter", &filter},
    {"reduce",&reduce}};
    
