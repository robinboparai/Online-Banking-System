#ifndef BANKING_EXCEPTION_H
#define BANKING_EXCEPTION_H

#include <stdexcept>
#include <string>

class BankingException : public std::runtime_error {
public:
    explicit BankingException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif
