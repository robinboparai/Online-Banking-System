#include "Customer.h"
#include "BankingException.h"

#include <utility>

Customer::Customer(int accountNumber, std::string name, int age,
                   std::string mobile, std::string email, std::string password)
    : accountNumber_(accountNumber), name_(std::move(name)), age_(age),
      mobile_(std::move(mobile)), email_(std::move(email)),
      password_(std::move(password)) {
    if (accountNumber_ <= 0 || name_.empty() || age_ <= 0 || password_.empty()) {
        throw BankingException("Customer details are incomplete or invalid.");
    }
}

bool Customer::verifyPassword(const std::string& password) const {
    return password_ == password;
}

void Customer::changePassword(const std::string& currentPassword,
                              const std::string& newPassword) {
    if (!verifyPassword(currentPassword)) {
        throw BankingException("Current password is incorrect.");
    }
    if (newPassword.size() < 4) {
        throw BankingException("New password must contain at least 4 characters.");
    }
    password_ = newPassword;
}

void Customer::updateContactDetails(const std::string& mobile,
                                    const std::string& email) {
    if (mobile.empty() || email.empty()) {
        throw BankingException("Mobile number and email address cannot be empty.");
    }
    mobile_ = mobile;
    email_ = email;
}
