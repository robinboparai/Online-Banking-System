#include "Account.h"
#include "BankingException.h"

#include <limits>
#include <iomanip>
#include <sstream>
#include <ostream>
#include <utility>

Account::Account(int accountNumber, long long openingBalancePaise)
    : accountNumber_(accountNumber), balancePaise_(openingBalancePaise) {
    if (accountNumber_ <= 0 || openingBalancePaise < 0) {
        throw BankingException("Invalid account number or opening balance.");
    }
}

void Account::deposit(long long amountPaise) {
    if (amountPaise <= 0) {
        throw BankingException("Deposit amount must be greater than zero.");
    }
    if (amountPaise > std::numeric_limits<long long>::max() - balancePaise_) {
        throw BankingException("Deposit amount is too large.");
    }
    balancePaise_ += amountPaise;
}

// Overloaded deposit function: the note is accepted for a more descriptive operation.
void Account::deposit(long long amountPaise, const std::string& note) {
    if (note.empty()) {
        throw BankingException("A deposit note cannot be empty.");
    }
    deposit(amountPaise);
}

void Account::withdraw(long long amountPaise) {
    if (amountPaise <= 0) {
        throw BankingException("Withdrawal amount must be greater than zero.");
    }
    if (amountPaise > balancePaise_) {
        throw BankingException("Insufficient account balance.");
    }
    balancePaise_ -= amountPaise;
}

bool operator>(const Account& left, const Account& right) {
    return left.balancePaise_ > right.balancePaise_;
}

std::ostream& operator<<(std::ostream& output, const Account& account) {
    output << "Account " << account.getAccountNumber()
           << " (" << account.getAccountType() << ")";
    return output;
}

SavingsAccount::SavingsAccount(int accountNumber, long long openingBalancePaise,
                               int annualInterestBasisPoints)
    : Account(accountNumber, openingBalancePaise),
      annualInterestBasisPoints_(annualInterestBasisPoints) {
    if (annualInterestBasisPoints_ < 0 || annualInterestBasisPoints_ > 10000) {
        throw BankingException("Savings interest rate must be between 0% and 100%.");
    }
}

std::string SavingsAccount::getAccountType() const {
    return "Savings";
}

long long SavingsAccount::calculateInterestPaise() const {
    const long long balance = getBalancePaise();
    // Basis points: 250 means 2.50%. The split calculation avoids multiplication overflow.
    return (balance / 10000LL) * annualInterestBasisPoints_
         + ((balance % 10000LL) * annualInterestBasisPoints_) / 10000LL;
}

std::string SavingsAccount::accountRule() const {
    std::ostringstream rate;
    rate << std::fixed << std::setprecision(2)
         << static_cast<double>(annualInterestBasisPoints_) / 100.0;
    return "Annual simple interest: " + rate.str() + "%";
}

CurrentAccount::CurrentAccount(int accountNumber, long long openingBalancePaise)
    : Account(accountNumber, openingBalancePaise) {}

std::string CurrentAccount::getAccountType() const {
    return "Current";
}

long long CurrentAccount::calculateInterestPaise() const {
    return 0;
}

std::string CurrentAccount::accountRule() const {
    return "No interest; withdrawals are limited to the available balance";
}
