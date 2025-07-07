#include "../src/lexer.h"
#include "../src/error.h"
#include <gtest/gtest.h>

namespace {

    using namespace exprcalc;

    TEST(LexerTest, BasicExpression) {
        Lexer lexer("2 + 3 * x");
        auto tokens = lexer.tokenize();
        ASSERT_EQ(tokens.size(), 5);
        EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
        EXPECT_EQ(tokens[0].value, "2");
        EXPECT_EQ(tokens[0].position, 0);
        EXPECT_EQ(tokens[1].type, TokenType::OPERATOR);
        EXPECT_EQ(tokens[1].value, "+");
        EXPECT_EQ(tokens[1].position, 2);
        EXPECT_EQ(tokens[2].type, TokenType::NUMBER);
        EXPECT_EQ(tokens[2].value, "3");
        EXPECT_EQ(tokens[2].position, 4);
        EXPECT_EQ(tokens[3].type, TokenType::OPERATOR);
        EXPECT_EQ(tokens[3].value, "*");
        EXPECT_EQ(tokens[3].position, 6);
        EXPECT_EQ(tokens[4].type, TokenType::VARIABLE);
        EXPECT_EQ(tokens[4].value, "x");
        EXPECT_EQ(tokens[4].position, 8);
    }

    TEST(LexerTest, Parentheses) {
        Lexer lexer("(2 + 3)");
        auto tokens = lexer.tokenize();
        ASSERT_EQ(tokens.size(), 5);
        EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
        EXPECT_EQ(tokens[0].value, "(");
        EXPECT_EQ(tokens[0].position, 0);
        EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
        EXPECT_EQ(tokens[1].value, "2");
        EXPECT_EQ(tokens[1].position, 1);
        EXPECT_EQ(tokens[2].type, TokenType::OPERATOR);
        EXPECT_EQ(tokens[2].value, "+");
        EXPECT_EQ(tokens[2].position, 3);
        EXPECT_EQ(tokens[3].type, TokenType::NUMBER);
        EXPECT_EQ(tokens[3].value, "3");
        EXPECT_EQ(tokens[3].position, 5);
        EXPECT_EQ(tokens[4].type, TokenType::RIGHT_PAREN);
        EXPECT_EQ(tokens[4].value, ")");
        EXPECT_EQ(tokens[4].position, 6);
    }

    TEST(LexerTest, InvalidCharacter) {
        Lexer lexer("2 + @");
        EXPECT_THROW(lexer.tokenize(), CalculationError);
        try {
            lexer.tokenize();
        } catch (const CalculationError& e) {
            EXPECT_EQ(e.get_position(), 4);
            EXPECT_STREQ(e.what(), "Invalid character: @");
        }
    }

} // anonymous namespace