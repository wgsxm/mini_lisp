#include "forms.h"
ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    std::vector<std::string> str;
    for (auto& i : args[0]->toVector()) {
        if (i->asSymbol()) {
            str.push_back(*i->asSymbol());
        }else {
            throw LispError("Unimplemented");
        }
    }
    std::vector<ValuePtr> body;
    for (int i = 1; i < args.size(); i++) {
        body.push_back(args[i]);
    }
    return std::make_shared<LambdaValue>(str,
                                        body,env.shared_from_this());
}
ValuePtr defineForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (auto name = args[0]->asSymbol()) {
        env.defineBinding(*name, env.eval(args[1]));
    } else if (args[0]->isPair()) {
        if (auto name = args[0]->left()->asSymbol()) {
            std::vector<ValuePtr> target;
            target.push_back(args[0]->right());
            for (int i = 1; i < args.size(); i++) {
                target.push_back(args[i]);
            }
            env.defineBinding(*name, lambdaForm(target, env));
        } else {
            throw LispError("Unimplemented");
        }
    }
    else {
        throw LispError("Unimplemented");
    }
    return std::make_shared<NilValue>();
}
ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    return args[0];
}
ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (env.eval(args[0])->isTrue()) {
        return env.eval(args[1]);
    } else {
        return env.eval(args[2]);
    }
}
ValuePtr andForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.empty()) return std::make_shared<BooleanValue>(true);
    ValuePtr ret = nullptr;
    for (auto& i : args) {
        ret = env.eval(i);
        if (!ret->isTrue()) {
            return std::make_shared<BooleanValue>(false);
        }
    }
    return ret;
}
ValuePtr orForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    ValuePtr ret = nullptr;
    for (auto& i : args) {
        ret = env.eval(i);
        if (ret->isTrue()) {
            return ret;
        }
    }
    return std::make_shared<BooleanValue>(false);
}
    const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", &defineForm},
    {"quote", &quoteForm},
    {"if", &ifForm},
    {"and", &andForm},
    {"or", &orForm},
    {"lambda", &lambdaForm}
    };
