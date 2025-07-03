#ifndef EXPRCALC_LOGGER_H
#define EXPRCALC_LOGGER_H

#include "token.h"
#include <ostream>
#include <iostream>
#include <vector>

namespace exprcalc {

    class Logger {
    public:
        explicit Logger(std::ostream& out = std::cout);
        void set_enabled(bool enabled);
        void log_tokens(const std::vector<Token>& tokens) const;
        void log_rpn(const std::vector<Token>& rpn) const;
        void log_result(double result) const;

    private:
        std::ostream& out_;
        bool enabled_;
        std::string token_type_to_string(TokenType type) const;
    };

} // namespace exprcalc

#endif // EXPRCALC_LOGGER_H