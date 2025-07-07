#include "shunting_yard.h"
#include "error.h"
#include <stack>
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
    bool expect_operand = true; // 初始期待操作数

    for (const auto& token : tokens_) {
        switch (token.type) {
            case TokenType::NUMBER:
            case TokenType::VARIABLE:
                if (!expect_operand) {
                    throw CalculationError("Invalid token in RPN", token.position);
                }
                output.push_back(token);
                expect_operand = false; // 操作数后期待运算符或右括号
                break;

            case TokenType::OPERATOR:
                if (expect_operand) {
                    throw CalculationError("Invalid token in RPN", token.position);
                }
                while (!operators.empty() &&
                       operators.top().type != TokenType::LEFT_PAREN &&
                       get_precedence(operators.top().value) >= get_precedence(token.value)) {
                    output.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
                expect_operand = true; // 运算符后期待操作数
                break;

            case TokenType::LEFT_PAREN:
                operators.push(token);
                expect_operand = true; // 左括号后期待操作数
                break;

            case TokenType::RIGHT_PAREN:
                if (expect_operand) {
                    throw CalculationError("Invalid token in RPN", token.position);
                }
                while (!operators.empty() && operators.top().type != TokenType::LEFT_PAREN) {
                    output.push_back(operators.top());
                    operators.pop();
                }
                if (operators.empty()) {
                    throw CalculationError("Mismatched parentheses", token.position);
                }
                operators.pop(); // 移除左括号
                expect_operand = false; // 右括号后期待运算符或结束
                break;
        }
    }

    while (!operators.empty()) {
        if (operators.top().type == TokenType::LEFT_PAREN) {
            throw CalculationError("Mismatched parentheses", operators.top().position);
        }
        output.push_back(operators.top());
        operators.pop();
    }

    if (expect_operand) {
        throw CalculationError("Invalid token in RPN", tokens_.empty() ? 0 : tokens_.back().position);
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