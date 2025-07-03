#include "evaluator.h"
#include <stdexcept>
#include <cmath>

namespace exprcalc {

    Evaluator::Evaluator(const std::vector<Token>& rpn, const SymbolTable& symbols)
        : rpn_(rpn), symbols_(symbols) {}

    double Evaluator::evaluate() {
        std::stack<double> stack;

        for (const auto& token : rpn_) {
            switch (token.type) {
                case TokenType::NUMBER:
                    stack.push(std::stod(token.value));
                    break;

                case TokenType::VARIABLE:
                    stack.push(symbols_.get_variable(token.value));
                    break;

                case TokenType::OPERATOR: {
                    if (stack.size() < 2) {
                        throw std::runtime_error("Insufficient operands for operator " + token.value +
                                                 " at position " + std::to_string(token.position));
                    }
                    double b = stack.top(); stack.pop();
                    double a = stack.top(); stack.pop();
                    stack.push(apply_operator(a, b, token.value));
                    break;
                }

                default:
                    throw std::runtime_error("Invalid token in RPN at position " + std::to_string(token.position));
            }
        }

        if (stack.size() != 1) {
            throw std::runtime_error("Invalid RPN expression: too many operands");
        }
        return stack.top();
    }

    double Evaluator::apply_operator(double a, double b, const std::string& op) const {
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        if (op == "/") {
            if (b == 0) throw std::runtime_error("Division by zero");
            return a / b;
        }
        throw std::runtime_error("Unknown operator: " + op);
    }

} // namespace exprcalc


111