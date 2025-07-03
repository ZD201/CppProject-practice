#include "calculator.h"
#include "error.h"

namespace exprcalc {

    Calculator::Calculator() : symbols_(), logger_() {}

    double Calculator::evaluate(const std::string& expression) {
        Lexer lexer(expression);
        auto tokens = lexer.tokenize();
        logger_.log_tokens(tokens);

        ShuntingYard shunting_yard(tokens);
        auto rpn = shunting_yard.to_rpn();
        logger_.log_rpn(rpn);

        Evaluator evaluator(rpn, symbols_);
        double result = evaluator.evaluate();
        logger_.log_result(result);

        return result;
    }

    void Calculator::set_variable(const std::string& name, double value) {
        symbols_.set_variable(name, value);
    }

    void Calculator::set_debug_mode(bool enabled) {
        logger_.set_enabled(enabled);
    }

} // namespace exprcalc