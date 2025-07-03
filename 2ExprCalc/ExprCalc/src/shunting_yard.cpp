#include "shunting_yard.h"
#include <stdexcept>

namespace exprcalc {

ShuntingYard::ShuntingYard(const std::vector<Token>& tokens) : tokens_(tokens) {
    precedence_ = {
        {"+", 1},
        {"-", 1},
        {"*", 2},
        {"/", 2}
    };
}

std::vector<Token> ShuntingYard::to_rpn() {
    std::vector<Token> output;
    std::stack<Token> operators;

    for (const auto& token : tokens_) {
        switch (token.type) {
            case TokenType::NUMBER:
            case TokenType::VARIABLE:
                output.push_back(token);
                break;

            case TokenType::OPERATOR:
                while (!operators.empty() &&
                       operators.top().type != TokenType::LEFT_PAREN &&
                       get_precedence(operators.top().value) >= get_precedence(token.value)) {
                    output.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
                break;

            case TokenType::LEFT_PAREN:
                operators.push(token);
                break;

            case TokenType::RIGHT_PAREN:
                while (!operators.empty() && operators.top().type != TokenType::LEFT_PAREN) {
                    output.push_back(operators.top());
                    operators.pop();
                }
                if (operators.empty()) {
                    throw std::runtime_error("Mismatched parentheses at position " + std::to_string(token.position));
                }
                operators.pop(); // 移除左括号
                break;
        }
    }

    while (!operators.empty()) {
        if (operators.top().type == TokenType::LEFT_PAREN) {
            throw std::runtime_error("Mismatched parentheses at position " + std::to_string(operators.top().position));
        }
        output.push_back(operators.top());
        operators.pop();
    }

    return output;
}

bool ShuntingYard::is_operator(const std::string& value) const {
    return precedence_.find(value) != precedence_.end();
}

int ShuntingYard::get_precedence(const std::string& op) const {
    auto it = precedence_.find(op);
    return it != precedence_.end() ? it->second : 0;
}

} // namespace exprcalc