#include "src/lexer.h"
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
    std::cout << "ExprCalc: Enter expressions or 'exit' to quit.\n";
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "exit") break;
        try {
            exprcalc::Lexer lexer(input);
            auto tokens = lexer.tokenize();
            std::cout << "Tokens:\n";
            for (const auto& token : tokens) {
                std::cout << "  Type: " << token_type_to_string(token.type)
                          << ", Value: " << token.value
                          << ", Position: " << token.position << '\n';
            }
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << '\n';
        }
    }
    return 0;
}