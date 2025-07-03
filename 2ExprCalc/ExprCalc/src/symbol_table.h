#ifndef EXPRCALC_SYMBOL_TABLE_H
#define EXPRCALC_SYMBOL_TABLE_H

#include <map>
#include <string>

namespace exprcalc {

    class SymbolTable {
    public:
        void set_variable(const std::string& name, double value);
        double get_variable(const std::string& name) const;
        bool has_variable(const std::string& name) const;

    private:
        std::map<std::string, double> variables_;
    };

} // namespace exprcalc

#endif // EXPRCALC_SYMBOL_TABLE_H