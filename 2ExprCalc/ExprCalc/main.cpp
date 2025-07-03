#include "src/lexer.h"
#include "src/shunting_yard.h"
#include "src/evaluator.h"
#include "src/symbol_table.h"
#include <iostream>
#include <string>

namespace {

std::string token_type_to_string(exprcalc::TokenType type) {
    switch (type) {
        case exprcalc::TokenType::NUMBER: return "NUMBER";
        case exprcalc::TokenType::OPERATOR: return "OPERATOR";
        case exprcalc::TokenType::VARIABLE: return "VARIABLE";
        case exprcalc::TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case exprcalc::TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
    }
    return "UNKNOWN";
}

} // anonymous namespace

int main() {
    std::cout << "ExprCalc: Enter expressions, 'set x = value', or 'exit' to quit.\n";
    exprcalc::SymbolTable symbols;
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "exit") break;
        try {
            if (input.find("set ") == 0) {
                auto pos = input.find('=');
                if (pos == std::string::npos) throw std::runtime_error("Invalid set command");
                auto name = std::string_view{input}.substr(4, pos - 5);
                auto value_str = std::string_view{input}.substr(pos + 1);
                double value = std::stod(std::string{value_str});
                symbols.set_variable(std::string{name}, value);
                std::cout << "Set " << name << " = " << value << '\n';
            } else {
                // 词法分析
                exprcalc::Lexer lexer(input);
                auto tokens = lexer.tokenize();
                std::cout << "Tokens:\n";
                for (const auto& token : tokens) {
                    std::cout << "  Type: " << token_type_to_string(token.type)
                              << ", Value: " << token.value
                              << ", Position: " << token.position << '\n';
                }

                // Shunting Yard 转换
                exprcalc::ShuntingYard shunting_yard(tokens);
                auto rpn = shunting_yard.to_rpn();
                std::cout << "RPN:\n";
                for (const auto& token : rpn) {
                    std::cout << "  Type: " << token_type_to_string(token.type)
                              << ", Value: " << token.value
                              << ", Position: " << token.position << '\n';
                }

                // 求值
                exprcalc::Evaluator evaluator(rpn, symbols);
                double result = evaluator.evaluate();
                std::cout << "Result: " << result << '\n';
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << '\n';
        }
    }
    return 0;
}