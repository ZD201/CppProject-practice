#ifndef EXPRCALC_ERROR_H
#define EXPRCALC_ERROR_H

#include <stdexcept>
#include <string>

namespace exprcalc {

    class CalculationError : public std::runtime_error {
    public:
        CalculationError(const std::string& message, size_t position)
            : std::runtime_error(message), position_(position) {}
        size_t get_position() const { return position_; }

    private:
        size_t position_;
    };

} // namespace exprcalc

#endif // EXPRCALC_ERROR_H