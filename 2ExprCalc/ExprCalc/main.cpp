#include "src/calculator.h"
#include "src/error.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "ExprCalc: Enter expressions, 'set x = value', 'debug on/off', 'help', or 'exit'\n";
    exprcalc::Calculator calc;
    std::string input;
    while (std::getline(std::cin, input)) {
        if (input.empty()) continue;
        if (input == "exit") break;
        if (input == "help") {
            std::cout << "Commands:\n"
                      << "  <expression> : Evaluate (e.g., 2 + 3 * 4)\n"
                      << "  set x = value : Set variable\n"
                      << "  debug on/off : Enable/disable debug\n"
                      << "  exit : Quit\n";
            continue;
        }
        try {
            if (input.find("set ") == 0) {
                auto pos = input.find('=');
                if (pos == std::string::npos) throw std::runtime_error("Invalid set command");
                auto name = std::string_view{input}.substr(4, pos - 5);
                auto value_str = std::string_view{input}.substr(pos + 1);
                double value = std::stod(std::string{value_str});
                calc.set_variable(std::string{name}, value);
                std::cout << "Set " << name << " = " << value << '\n';
            } else if (input == "debug on") {
                calc.set_debug_mode(true);
                std::cout << "Debug mode enabled\n";
            } else if (input == "debug off") {
                calc.set_debug_mode(false);
                std::cout << "Debug mode disabled\n";
            } else {
                double result = calc.evaluate(input);
                std::cout << "Result: " << result << '\n';
            }
        } catch (const exprcalc::CalculationError& e) {
            std::cout << "Error: " << e.what() << " at position " << e.get_position() << '\n';
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << '\n';
        }
    }
    return 0;
}