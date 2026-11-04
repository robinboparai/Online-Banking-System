#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iosfwd>
#include <string>

class Bank;

class Account {
private:
    int accountNumber_{};
    long long balancePaise_{};

protected:
    Account(int accountNumber, long long openingBalancePaise);

public:
    virtual ~Account() = default;

    int getAccountNumber() const { return accountNumber_; }
    long long getBalancePaise() const { return balancePaise_; }

    virtual std::string getAccountType() const = 0;
    virtual long long calculateInterestPaise() const = 0;
    virtual std::string accountRule() const = 0;

    void deposit(long long amountPaise);
    void deposit(long long amountPaise, const std::string& note);
    void withdraw(long long amountPaise);

    friend bool operator>(const Account& left, const Account& right);
    friend std::ostream& operator<<(std::ostream& output, const Account& account);

private:
    void restoreBalance(long long balancePaise) { balancePaise_ = balancePaise; }
    friend class Bank;
};

class SavingsAccount final : public Account {
private:
    int annualInterestBasisPoints_;

public:
    SavingsAccount(int accountNumber, long long openingBalancePaise,
                   int annualInterestBasisPoints = 250);

    std::string getAccountType() const override;
    long long calculateInterestPaise() const override;
    std::string accountRule() const override;
};

class CurrentAccount final : public Account {
public:
    CurrentAccount(int accountNumber, long long openingBalancePaise);

    std::string getAccountType() const override;
    long long calculateInterestPaise() const override;
    std::string accountRule() const override;
};

#endif
