#ifndef TEMPLATE_UTILS_H
#define TEMPLATE_UTILS_H

#include <iomanip>
#include <iostream>
#include <string>

// Function template used by the customer and administrator screens.
template <typename T>
void printField(const std::string& label, const T& value) {
    std::cout << "  " << std::left << std::setw(22) << label << ": "
              << value << '\n';
}

#endif
