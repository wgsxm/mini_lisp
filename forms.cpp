#include "forms.h"
std::vector<std::string> getForm() {
    std::vector<std::string> ret;
    for (auto& i : SPECIAL_FORMS) {
        ret.push_back(i.first);
    }
    return ret;
}
    ValuePtr lambdaForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2)
        throw LispError("insufficient number of arguments");
    std::vector<std::string> str;
    for (auto& i : args[0]->toVector()) {
        if (i->asSymbol()) {
            str.push_back(*i->asSymbol());
        }else {
            throw LispError("Unvalid params");
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
    if (args.size() < 2) throw LispError("insufficient number of arguments");
    if (auto name = args[0]->asSymbol()) {
        if (args.size() > 2) throw LispError("excessive number of arguments");
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
            throw LispError("Unvalid function name");
        }
    }
    else {
        throw LispError("Unvalid argument name");
    }
    return std::make_shared<NilValue>();
}
ValuePtr quoteForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 1)
        throw LispError("insufficient number of arguments");
    else if (args.size() > 1)
        throw LispError("excessive number of arguments");
    return args[0];
}
ValuePtr ifForm(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() <3)
        throw LispError("insufficient number of arguments");
    else if (args.size() > 3)
        throw LispError("excessive number of arguments");
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


ValuePtr begin(const std::vector<ValuePtr>& args, EvalEnv& env) {
    ValuePtr result = std::make_shared<NilValue>();
    for (auto& expr : args) {
        result =env.eval(expr);
    }
    return result;
}
    ValuePtr cond(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 1) throw LispError("insufficient number of arguments");
    for (int i = 0; i < args.size();i++) {
            if (typeid(*args[i]->left()) == typeid(SymbolValue)) {
                auto& symbol =
                    static_cast<const SymbolValue&>(*args[i]->left());
                if (symbol.getName() == "else") {
                    if (i == args.size() - 1)
                        return begin(args[i]->right()->toVector(), env);
                    else
                        throw LispError("improper else");
                }
            }
          if (env.eval(args[i]->left())->isTrue()) {
                if (args[i]->right()->toVector().empty())
                    return env.eval(args[i]->left());
                return begin(args[i]->right()->toVector(), env);
            }
        //对每个子句
    }
}
    ValuePtr let(const std::vector<ValuePtr>& args, EvalEnv& env) {
    if (args.size() < 2)
        throw LispError("insufficient number of arguments");
        std::vector<ValuePtr> params, pargs;
    for (auto& i : args[0]->toVector()) {
            std::vector<ValuePtr> temp = i->toVector();
            params.push_back(temp[0]);
            pargs.push_back(temp[1]);
    }
    ValuePtr pro = std::make_shared<SymbolValue>("lambda");
    std::vector<ValuePtr> lambda;
    lambda.push_back(pro);
    lambda.push_back(vec2pair(params));
    for (int i = 1; i < args.size(); i++) {
        lambda.push_back(args[i]);
    }
    std::vector<ValuePtr> ret;
    ret.push_back(vec2pair(lambda));
    for (auto& i : pargs) {
        ret.push_back(i);
    }
    return env.eval(vec2pair(ret));
    }
    ValuePtr unquote(const std::vector<ValuePtr>& args, EvalEnv& env) {
        if (args.size() < 1)
            throw LispError("insufficient number of arguments");
        return env.eval(args[0]);
    }
    ValuePtr quasiquote(const std::vector<ValuePtr>& args, EvalEnv& env) {
        if (args.size() < 1)
            throw LispError("insufficient number of arguments");
        std::vector<ValuePtr> data = args[0]->toVector(), ret;
        for (auto& i : data) {
            if (typeid(*i) == typeid(PairValue)) {
                std::vector<ValuePtr> vec = i->toVector();
                if (vec[0]->asSymbol() && *vec[0]->asSymbol() == "unquote") {
                    vec.erase(vec.begin());
                    ret.push_back(unquote(vec,env));
                } else
                    ret.push_back(i);
            } else {
                ret.push_back(i);
            }
        }
        return vec2pair(ret);
    }
    const std::unordered_map<std::string, SpecialFormType*> SPECIAL_FORMS{
    {"define", &defineForm},
    {"quote", &quoteForm},
    {"if", &ifForm},
    {"and", &andForm},
    {"or", &orForm},
    {"lambda", &lambdaForm},
        {"cond", &cond},         {"begin", &begin},
        {"let", &let},
        {"quasiquote", &quasiquote},
        {"unquote",&unquote}
    };
