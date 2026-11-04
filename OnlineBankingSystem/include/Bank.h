#ifndef BANK_H
#define BANK_H

#include "Account.h"
#include "Admin.h"
#include "Customer.h"
#include "Transaction.h"

#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

class Bank {
private:
    std::map<int, Customer> customers_;
    std::map<int, std::unique_ptr<Account>> accounts_;
    std::vector<Transaction> transactions_;
    std::queue<ServiceRequest> serviceRequests_;
    Admin admin_;
    std::string dataDirectory_;
    int nextAccountNumber_{100001};
    long long nextTransactionId_{1};
    long long nextRequestId_{1};

    void saveAll() const;
    void loadCustomers();
    void loadAccounts();
    void loadTransactions();
    void loadAdmin();
    void loadServiceRequests();
    void addTransaction(int accountNumber, const std::string& type,
                        long long amountPaise, const std::string& status,
                        int counterpartyAccount = 0);

public:
    explicit Bank(std::string dataDirectory = "data");
    void load();

    int createAccount(const std::string& name, int age,
                      const std::string& mobile, const std::string& email,
                      const std::string& password, const std::string& accountType,
                      long long openingDepositPaise);

    bool authenticateCustomer(int accountNumber,
                              const std::string& password) const;
    bool authenticateAdmin(const std::string& username,
                           const std::string& password) const;

    const Customer* findCustomer(int accountNumber) const;
    Account* findAccount(int accountNumber);
    const Account* findAccount(int accountNumber) const;

    void updateCustomer(int accountNumber, const std::string& mobile,
                        const std::string& email);
    void changePassword(int accountNumber, const std::string& currentPassword,
                        const std::string& newPassword);
    void deposit(int accountNumber, long long amountPaise,
                 const std::string& note = "Cash deposit");
    void withdraw(int accountNumber, long long amountPaise);
    void transfer(int senderAccount, int recipientAccount,
                  long long amountPaise);
    long long calculateInterest(int accountNumber) const;

    std::vector<Transaction> transactionHistory(int accountNumber) const;
    const std::map<int, Customer>& customers() const { return customers_; }
    const std::map<int, std::unique_ptr<Account>>& accounts() const { return accounts_; }
    const std::vector<Transaction>& transactions() const { return transactions_; }
    long long totalDepositsPaise() const;
    std::pair<std::size_t, std::size_t> accountStatistics() const;

    long long submitServiceRequest(int accountNumber,
                                   const std::string& description);
    bool hasServiceRequests() const { return !serviceRequests_.empty(); }
    ServiceRequest processNextServiceRequest();
    std::size_t pendingServiceRequestCount() const { return serviceRequests_.size(); }
};

#endif
