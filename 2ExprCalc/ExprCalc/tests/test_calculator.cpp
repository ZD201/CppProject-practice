#include "../src/calculator.h"
#include "../src/error.h"
#include <gtest/gtest.h>

namespace {

    using namespace exprcalc;

    TEST(CalculatorTest, BasicArithmetic) {
        Calculator calc;
        EXPECT_DOUBLE_EQ(calc.evaluate("2 + 3"), 5.0);
        EXPECT_DOUBLE_EQ(calc.evaluate("3 * 4 + 2"), 14.0);
        EXPECT_DOUBLE_EQ(calc.evaluate("(2 + 3) * 4"), 20.0);
    }

    TEST(CalculatorTest, VariableEvaluation) {
        Calculator calc;
        calc.set_variable("x", 5.0);
        calc.set_variable("y", 2.0);
        EXPECT_DOUBLE_EQ(calc.evaluate("x + y"), 7.0);
        EXPECT_DOUBLE_EQ(calc.evaluate("x * y + 3"), 13.0);
    }

    TEST(CalculatorTest, DivisionByZero) {
        Calculator calc;
        EXPECT_THROW(calc.evaluate("5 / 0"), CalculationError);
        try {
            calc.evaluate("5 / 0");
        } catch (const CalculationError& e) {
            EXPECT_STREQ(e.what(), "Division by zero");
        }
    }

    TEST(CalculatorTest, UndefinedVariable) {
        Calculator calc;
        EXPECT_THROW(calc.evaluate("x + 1"), CalculationError);
        try {
            calc.evaluate("x + 1");
        } catch (const CalculationError& e) {
            EXPECT_STREQ(e.what(), "Undefined variable: x");
        }
    }

    TEST(CalculatorTest, InvalidExpression) {
        Calculator calc;
        EXPECT_THROW(calc.evaluate("2 + + 3"), CalculationError);
        try {
            calc.evaluate("2 + + 3");
        } catch (const CalculationError& e) {
            EXPECT_STREQ(e.what(), "Invalid token in RPN");
        }
    }

} // anonymous namespace