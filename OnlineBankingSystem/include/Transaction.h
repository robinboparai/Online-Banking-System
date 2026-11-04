#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

struct Transaction {
    long long transactionId{};
    int accountNumber{};
    std::string type;
    long long amountPaise{};
    std::string dateTime;
    std::string status;
    long long balanceAfterPaise{};
    int counterpartyAccount{};
};

struct ServiceRequest {
    long long requestId{};
    int accountNumber{};
    std::string description;
    std::string submittedAt;
};

#endif
