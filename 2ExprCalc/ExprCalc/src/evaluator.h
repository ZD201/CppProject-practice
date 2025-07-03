#ifndef EXPRCALC_EVALUATOR_H
#define EXPRCALC_EVALUATOR_H

#include "token.h"
#include "symbol_table.h"
#include <vector>
#include <stack>

namespace exprcalc {

    class Evaluator {
    public:
        Evaluator(const std::vector<Token>& rpn, const SymbolTable& symbols);
        double evaluate();

    private:
        const std::vector<Token>& rpn_;
        const SymbolTable& symbols_;
        double apply_operator(double a, double b, const std::string& op) const;
    };

} // namespace exprcalc

#endif // EXPRCALC_EVALUATOR_H