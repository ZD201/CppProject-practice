#ifndef EXPRCALC_LEXER_H
#define EXPRCALC_LEXER_H

#include "token.h"
#include <string>
#include <vector>

namespace exprcalc {

    class Lexer {
    public:
        explicit Lexer(std::string input);
        std::vector<Token> tokenize();

    private:
        std::string input_;
        size_t pos_;
        void skip_whitespace();
        Token next_token();
    };

} // namespace exprcalc

#endif // EXPRCALC_LEXER_H