#include "logger.h"
#include <iostream>

namespace exprcalc {

    Logger::Logger(std::ostream& out) : out_(out), enabled_(false) {}

    void Logger::set_enabled(bool enabled) {
        enabled_ = enabled;
    }

    void Logger::log_tokens(const std::vector<Token>& tokens) const {
        if (!enabled_) return;
        out_ << "Tokens:\n";
        for (const auto& token : tokens) {
            out_ << "  Type: " << token_type_to_string(token.type)
                 << ", Value: " << token.value
                 << ", Position: " << token.position << '\n';
        }
    }

    void Logger::log_rpn(const std::vector<Token>& rpn) const {
        if (!enabled_) return;
        out_ << "RPN:\n";
        for (const auto& token : rpn) {
            out_ << "  Type: " << token_type_to_string(token.type)
                 << ", Value: " << token.value
                 << ", Position: " << token.position << '\n';
        }
    }

    void Logger::log_result(double result) const {
        if (!enabled_) return;
        out_ << "Result: " << result << '\n';
    }

    std::string Logger::token_type_to_string(TokenType type) const {
        switch (type) {
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::OPERATOR: return "OPERATOR";
            case TokenType::VARIABLE: return "VARIABLE";
            case TokenType::LEFT_PAREN: return "LEFT_PAREN";
            case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        }
        return "UNKNOWN";
    }

} // namespace exprcalc