#ifndef EXPRCALC_SHUNTING_YARD_H
#define EXPRCALC_SHUNTING_YARD_H

#include "token.h"
#include <vector>
#include <stack>
#include <map>

namespace exprcalc {

    class ShuntingYard {
    public:
        explicit ShuntingYard(const std::vector<Token>& tokens);
        std::vector<Token> to_rpn();

    private:
        std::vector<Token> tokens_;
        std::map<std::string, int> precedence_; // 运算符优先级
        bool is_operator(const std::string& value) const;
        int get_precedence(const std::string& op) const;
    };

} // namespace exprcalc

#endif // EXPRCALC_SHUNTING_YARD_H