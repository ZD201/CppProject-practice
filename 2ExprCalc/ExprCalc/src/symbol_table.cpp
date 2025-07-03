#include "symbol_table.h"
#include "error.h"

namespace exprcalc {

    void SymbolTable::set_variable(const std::string& name, double value) {
        variables_[name] = value;
    }

    double SymbolTable::get_variable(const std::string& name) const {
        auto it = variables_.find(name);
        if (it == variables_.end()) {
            throw CalculationError("Undefined variable: " + name, 0);
        }
        return it->second;
    }

    bool SymbolTable::has_variable(const std::string& name) const {
        return variables_.find(name) != variables_.end();
    }

} // namespace exprcalc