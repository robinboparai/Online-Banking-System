#ifndef MONEY_H
#define MONEY_H

#include <iomanip>
#include <sstream>
#include <string>

// All monetary values are stored as integer paise (100 paise = INR 1).
inline std::string formatMoney(long long paise) {
    const bool negative = paise < 0;
    const unsigned long long absoluteValue = negative
        ? static_cast<unsigned long long>(-(paise + 1)) + 1ULL
        : static_cast<unsigned long long>(paise);

    std::ostringstream output;
    if (negative) output << '-';
    output << "INR " << absoluteValue / 100ULL << '.'
           << std::setw(2) << std::setfill('0') << absoluteValue % 100ULL;
    return output.str();
}

#endif
