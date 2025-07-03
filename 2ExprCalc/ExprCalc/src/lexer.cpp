#include "lexer.h"
#include "error.h"
#include <cctype>

namespace exprcalc {

    Lexer::Lexer(std::string input) : input_(std::move(input)), pos_(0) {}

    std::vector<Token> Lexer::tokenize() {
        std::vector<Token> tokens;
        while (pos_ < input_.size()) {
            skip_whitespace();
            if (pos_ >= input_.size()) break;
            tokens.push_back(next_token());
        }
        return tokens;
    }

    void Lexer::skip_whitespace() {
        while (pos_ < input_.size() && std::isspace(input_[pos_])) {
            ++pos_;
        }
    }

    Token Lexer::next_token() {
        char current = input_[pos_];

        // 数字
        if (std::isdigit(current) || current == '.') {
            std::string number;
            size_t start_pos = pos_;
            while (pos_ < input_.size() && (std::isdigit(input_[pos_]) || input_[pos_] == '.')) {
                number += input_[pos_++];
            }
            return Token(TokenType::NUMBER, number, start_pos);
        }

        // 运算符
        if (current == '+' || current == '-' || current == '*' || current == '/') {
            std::string op(1, current);
            return Token(TokenType::OPERATOR, op, pos_++);
        }

        // 括号
        if (current == '(') {
            return Token(TokenType::LEFT_PAREN, "(", pos_++);
        }
        if (current == ')') {
            return Token(TokenType::RIGHT_PAREN, ")", pos_++);
        }

        // 变量
        if (std::isalpha(current)) {
            std::string var;
            size_t start_pos = pos_;
            while (pos_ < input_.size() && std::isalnum(input_[pos_])) {
                var += input_[pos_++];
            }
            return Token(TokenType::VARIABLE, var, start_pos);
        }

        throw CalculationError("Invalid character: " + std::string(1, current), pos_);
    }

} // namespace exprcalc