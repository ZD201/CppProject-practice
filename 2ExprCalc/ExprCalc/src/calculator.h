#ifndef EXPRCALC_CALCULATOR_H
#define EXPRCALC_CALCULATOR_H

#include "lexer.h"
#include "shunting_yard.h"
#include "evaluator.h"
#include "symbol_table.h"
#include "logger.h"
#include <string>

namespace exprcalc {

    class Calculator {
    public:
        Calculator();
        double evaluate(const std::string& expression);
        void set_variable(const std::string& name, double value);
        void set_debug_mode(bool enabled);

    private:
        SymbolTable symbols_;
        Logger logger_;
    };

} // namespace exprcalc

#endif // EXPRCALC_CALCULATOR_H