#ifndef EXPRCALC_TOKEN_H
#define EXPRCALC_TOKEN_H

#include <string>

namespace exprcalc {

    enum class TokenType {
        NUMBER,      // 数字（例如 3.14）
        OPERATOR,    // 运算符（+、-、*、/）
        VARIABLE,    // 变量（例如 x、y）
        LEFT_PAREN,  // 左括号 (
        RIGHT_PAREN  // 右括号 )
    };

    struct Token {
        TokenType type;
        std::string value; // 存储具体值（如 "3.14"、"+"、"x"）
        size_t position;   // 标记在输入中的位置，用于错误报告

        Token(TokenType t, std::string v, size_t pos) : type(t), value(std::move(v)), position(pos) {}
    };

} // namespace exprcalc

#endif // EXPRCALC_TOKEN_H