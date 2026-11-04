#include "Bank.h"
#include "Authentication.h"
#include "BankingException.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <system_error>
#include <utility>

namespace {
std::string currentDateTime() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    const std::tm* local = std::localtime(&nowTime);
    if (local == nullptr) return "Unknown date/time";

    std::ostringstream output;
    output << std::put_time(local, "%Y-%m-%d %H:%M:%S");
    return output.str();
}

void replaceFile(const std::filesystem::path& temporary,
                 const std::filesystem::path& destination) {
    std::error_code error;
    std::filesystem::remove(destination, error);
    error.clear();
    std::filesystem::rename(temporary, destination, error);
    if (error) {
        throw BankingException("Unable to replace data file: " + destination.string());
    }
}
} // namespace

Bank::Bank(std::string dataDirectory)
    : dataDirectory_(std::move(dataDirectory)) {}

void Bank::load() {
    std::error_code error;
    std::filesystem::create_directories(dataDirectory_, error);
    if (error) {
        throw BankingException("Unable to create data directory: " + dataDirectory_);
    }

    loadCustomers();
    loadAccounts();
    loadTransactions();
    loadAdmin();
    loadServiceRequests();
    saveAll(); // Creates any missing data files on the first run.
}

void Bank::loadCustomers() {
    std::ifstream input(std::filesystem::path(dataDirectory_) / "customers.txt");
    if (!input) return;

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        std::istringstream row(line);
        int accountNumber = 0;
        int age = 0;
        std::string name, mobile, email, password;
        if (row >> accountNumber >> std::quoted(name) >> age
                >> std::quoted(mobile) >> std::quoted(email) >> std::quoted(password)) {
            try {
                customers_.emplace(accountNumber,
                    Customer(accountNumber, name, age, mobile, email, password));
                if (accountNumber >= nextAccountNumber_) nextAccountNumber_ = accountNumber + 1;
            } catch (const BankingException&) {
                // Ignore malformed records so one damaged line does not stop startup.
            }
        }
    }
}

void Bank::loadAccounts() {
    std::ifstream input(std::filesystem::path(dataDirectory_) / "accounts.txt");
    if (!input) return;

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        std::istringstream row(line);
        int accountNumber = 0;
        std::string type;
        long long balance = 0;
        int interestBasisPoints = 250;
        if (!(row >> accountNumber >> type >> balance >> interestBasisPoints)) continue;

        try {
            if (type == "Savings") {
                accounts_[accountNumber] = std::unique_ptr<Account>(
                    new SavingsAccount(accountNumber, balance, interestBasisPoints));
            } else if (type == "Current") {
                accounts_[accountNumber] = std::unique_ptr<Account>(
                    new CurrentAccount(accountNumber, balance));
            }
            if (accountNumber >= nextAccountNumber_) nextAccountNumber_ = accountNumber + 1;
        } catch (const BankingException&) {
            // Ignore malformed account records.
        }
    }
}

void Bank::loadTransactions() {
    std::ifstream input(std::filesystem::path(dataDirectory_) / "transactions.txt");
    if (!input) return;

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        std::istringstream row(line);
        Transaction transaction;
        if (row >> transaction.transactionId >> transaction.accountNumber
                >> std::quoted(transaction.type) >> transaction.amountPaise
                >> std::quoted(transaction.dateTime) >> std::quoted(transaction.status)
                >> transaction.balanceAfterPaise >> transaction.counterpartyAccount) {
            transactions_.push_back(transaction);
            nextTransactionId_ = std::max(nextTransactionId_, transaction.transactionId + 1);
        }
    }
}

void Bank::loadAdmin() {
    std::ifstream input(std::filesystem::path(dataDirectory_) / "admin.txt");
    if (!input) return;

    std::string username, password;
    if (input >> std::quoted(username) >> std::quoted(password)) {
        admin_.setCredentials(username, password);
    }
}

void Bank::loadServiceRequests() {
    std::ifstream input(std::filesystem::path(dataDirectory_) / "service_requests.txt");
    if (!input) return;

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) continue;
        std::istringstream row(line);
        ServiceRequest request;
        if (row >> request.requestId >> request.accountNumber
                >> std::quoted(request.description) >> std::quoted(request.submittedAt)) {
            serviceRequests_.push(request);
            nextRequestId_ = std::max(nextRequestId_, request.requestId + 1);
        }
    }
}

void Bank::saveAll() const {
    const std::filesystem::path directory(dataDirectory_);

    auto writeFile = [&](const std::string& filename, const auto& writer) {
        const auto destination = directory / filename;
        const auto temporary = directory / (filename + ".tmp");
        std::ofstream output(temporary, std::ios::trunc);
        if (!output) {
            throw BankingException("Unable to open data file for writing: " + destination.string());
        }
        writer(output);
        output.close();
        if (!output) {
            throw BankingException("Unable to write data file: " + destination.string());
        }

        // Reopen with fstream (read/write mode) to verify that the saved file is readable.
        std::fstream verification(temporary, std::ios::in | std::ios::out);
        if (!verification) {
            throw BankingException("Unable to verify saved data file: " + destination.string());
        }
        std::string firstLine;
        if (!std::getline(verification, firstLine) && verification.bad()) {
            throw BankingException("Unable to read back data file: " + destination.string());
        }
        verification.close();
        replaceFile(temporary, destination);
    };

    writeFile("customers.txt", [&](std::ostream& output) {
        for (const auto& [number, customer] : customers_) {
            output << number << ' ' << std::quoted(customer.getName()) << ' '
                   << customer.getAge() << ' ' << std::quoted(customer.getMobile()) << ' '
                   << std::quoted(customer.getEmail()) << ' '
                   << std::quoted(customer.passwordForPersistence()) << '\n';
        }
    });

    writeFile("accounts.txt", [&](std::ostream& output) {
        for (const auto& [number, account] : accounts_) {
            int interestBasisPoints = 0;
            if (const auto* savings = dynamic_cast<const SavingsAccount*>(account.get())) {
                // The project uses a fixed default rate of 2.50% for savings accounts.
                (void)savings;
                interestBasisPoints = 250;
            }
            output << number << ' ' << account->getAccountType() << ' '
                   << account->getBalancePaise() << ' ' << interestBasisPoints << '\n';
        }
    });

    writeFile("transactions.txt", [&](std::ostream& output) {
        for (const auto& transaction : transactions_) {
            output << transaction.transactionId << ' ' << transaction.accountNumber << ' '
                   << std::quoted(transaction.type) << ' ' << transaction.amountPaise << ' '
                   << std::quoted(transaction.dateTime) << ' '
                   << std::quoted(transaction.status) << ' '
                   << transaction.balanceAfterPaise << ' '
                   << transaction.counterpartyAccount << '\n';
        }
    });

    writeFile("admin.txt", [&](std::ostream& output) {
        output << std::quoted(admin_.username()) << ' '
               << std::quoted(admin_.passwordForPersistence()) << '\n';
    });

    writeFile("service_requests.txt", [&](std::ostream& output) {
        auto requests = serviceRequests_; // Copy: std::queue has no const iterator.
        while (!requests.empty()) {
            const ServiceRequest request = requests.front();
            requests.pop();
            output << request.requestId << ' ' << request.accountNumber << ' '
                   << std::quoted(request.description) << ' '
                   << std::quoted(request.submittedAt) << '\n';
        }
    });
}

int Bank::createAccount(const std::string& name, int age,
                        const std::string& mobile, const std::string& email,
                        const std::string& password, const std::string& accountType,
                        long long openingDepositPaise) {
    if (name.empty() || mobile.empty() || email.empty() || password.size() < 4) {
        throw BankingException("Enter all customer details; password must have at least 4 characters.");
    }
    if (age <= 0 || openingDepositPaise < 0) {
        throw BankingException("Age must be positive and opening deposit cannot be negative.");
    }
    const std::vector<std::string> allowedTypes{"Savings", "Current"};
    if (std::find(allowedTypes.begin(), allowedTypes.end(), accountType) == allowedTypes.end()) {
        throw BankingException("Choose Savings or Current account.");
    }

    const int accountNumber = nextAccountNumber_++;
    customers_.emplace(accountNumber,
        Customer(accountNumber, name, age, mobile, email, password));

    if (accountType == "Savings") {
        accounts_[accountNumber] = std::unique_ptr<Account>(
            new SavingsAccount(accountNumber, openingDepositPaise));
    } else {
        accounts_[accountNumber] = std::unique_ptr<Account>(
            new CurrentAccount(accountNumber, openingDepositPaise));
    }

    if (openingDepositPaise > 0) {
        addTransaction(accountNumber, "OPENING_DEPOSIT", openingDepositPaise, "SUCCESS");
    }

    try {
        saveAll();
    } catch (...) {
        customers_.erase(accountNumber);
        accounts_.erase(accountNumber);
        if (!transactions_.empty() && transactions_.back().accountNumber == accountNumber) {
            transactions_.pop_back();
        }
        --nextAccountNumber_;
        try { saveAll(); } catch (...) { /* Keep the original persistence error. */ }
        throw;
    }
    return accountNumber;
}

bool Bank::authenticateCustomer(int accountNumber, const std::string& password) const {
    const auto found = customers_.find(accountNumber);
    return found != customers_.end() && Authentication::verifyCustomer(found->second, password);
}

bool Bank::authenticateAdmin(const std::string& username,
                             const std::string& password) const {
    return Authentication::verifyAdministrator(admin_, username, password);
}

const Customer* Bank::findCustomer(int accountNumber) const {
    const auto found = customers_.find(accountNumber);
    return found == customers_.end() ? nullptr : &found->second;
}

Account* Bank::findAccount(int accountNumber) {
    const auto found = accounts_.find(accountNumber);
    return found == accounts_.end() ? nullptr : found->second.get();
}

const Account* Bank::findAccount(int accountNumber) const {
    const auto found = accounts_.find(accountNumber);
    return found == accounts_.end() ? nullptr : found->second.get();
}

void Bank::updateCustomer(int accountNumber, const std::string& mobile,
                          const std::string& email) {
    auto found = customers_.find(accountNumber);
    if (found == customers_.end()) throw BankingException("Account number was not found.");
    const Customer previous = found->second;
    found->second.updateContactDetails(mobile, email);
    try {
        saveAll();
    } catch (...) {
        found->second = previous;
        try { saveAll(); } catch (...) { /* Keep the original persistence error. */ }
        throw;
    }
}

void Bank::changePassword(int accountNumber, const std::string& currentPassword,
                          const std::string& newPassword) {
    auto found = customers_.find(accountNumber);
    if (found == customers_.end()) throw BankingException("Account number was not found.");
    const Customer previous = found->second;
    found->second.changePassword(currentPassword, newPassword);
    try {
        saveAll();
    } catch (...) {
        found->second = previous;
        try { saveAll(); } catch (...) { /* Keep the original persistence error. */ }
        throw;
    }
}

void Bank::addTransaction(int accountNumber, const std::string& type,
                          long long amountPaise, const std::string& status,
                          int counterpartyAccount) {
    const Account* account = findAccount(accountNumber);
    if (account == nullptr) throw BankingException("Account number was not found.");

    transactions_.push_back(Transaction{
        nextTransactionId_++, accountNumber, type, amountPaise,
        currentDateTime(), status, account->getBalancePaise(), counterpartyAccount
    });
}

void Bank::deposit(int accountNumber, long long amountPaise, const std::string& note) {
    Account* account = findAccount(accountNumber);
    if (account == nullptr) throw BankingException("Account number was not found.");

    const long long previousBalance = account->getBalancePaise();
    const std::size_t previousTransactionCount = transactions_.size();
    account->deposit(amountPaise, note);
    addTransaction(accountNumber, "DEPOSIT", amountPaise, "SUCCESS");
    try {
        saveAll();
    } catch (...) {
        account->restoreBalance(previousBalance);
        transactions_.resize(previousTransactionCount);
        try { saveAll(); } catch (...) { /* Keep the original persistence error. */ }
        throw;
    }
}

void Bank::withdraw(int accountNumber, long long amountPaise) {
    Account* account = findAccount(accountNumber);
    if (account == nullptr) throw BankingException("Account number was not found.");

    const long long previousBalance = account->getBalancePaise();
    const std::size_t previousTransactionCount = transactions_.size();
    account->withdraw(amountPaise);
    addTransaction(accountNumber, "WITHDRAWAL", amountPaise, "SUCCESS");
    try {
        saveAll();
    } catch (...) {
        account->restoreBalance(previousBalance);
        transactions_.resize(previousTransactionCount);
        try { saveAll(); } catch (...) { /* Keep the original persistence error. */ }
        throw;
    }
}

void Bank::transfer(int senderAccount, int recipientAccount, long long amountPaise) {
    if (senderAccount == recipientAccount) {
        throw BankingException("Sender and recipient accounts must be different.");
    }
    Account* sender = findAccount(senderAccount);
    Account* recipient = findAccount(recipientAccount);
    if (sender == nullptr || recipient == nullptr) {
        throw BankingException("Sender or recipient account number was not found.");
    }

    const long long senderBalance = sender->getBalancePaise();
    const long long recipientBalance = recipient->getBalancePaise();
    const std::size_t previousTransactionCount = transactions_.size();

    sender->withdraw(amountPaise);
    try {
        recipient->deposit(amountPaise, "Fund transfer");
        addTransaction(senderAccount, "TRANSFER_OUT", amountPaise, "SUCCESS", recipientAccount);
        addTransaction(recipientAccount, "TRANSFER_IN", amountPaise, "SUCCESS", senderAccount);
        saveAll();
    } catch (...) {
        sender->restoreBalance(senderBalance);
        recipient->restoreBalance(recipientBalance);
        transactions_.resize(previousTransactionCount);
        try { saveAll(); } catch (...) { /* Preserve the original banking error. */ }
        throw;
    }
}

long long Bank::calculateInterest(int accountNumber) const {
    const Account* account = findAccount(accountNumber);
    if (account == nullptr) throw BankingException("Account number was not found.");
    return account->calculateInterestPaise();
}

std::vector<Transaction> Bank::transactionHistory(int accountNumber) const {
    std::vector<Transaction> result;
    std::copy_if(transactions_.begin(), transactions_.end(), std::back_inserter(result),
        [accountNumber](const Transaction& transaction) {
            return transaction.accountNumber == accountNumber;
        });
    std::sort(result.begin(), result.end(),
        [](const Transaction& left, const Transaction& right) {
            return left.transactionId < right.transactionId;
        });
    return result;
}

long long Bank::totalDepositsPaise() const {
    long long total = 0;
    for (const auto& [number, account] : accounts_) {
        (void)number;
        if (account->getBalancePaise() > std::numeric_limits<long long>::max() - total) {
            throw BankingException("Total balance is too large to calculate.");
        }
        total += account->getBalancePaise();
    }
    return total;
}

std::pair<std::size_t, std::size_t> Bank::accountStatistics() const {
    std::size_t savings = 0;
    std::size_t current = 0;
    for (const auto& [number, account] : accounts_) {
        (void)number;
        if (account->getAccountType() == "Savings") ++savings;
        else if (account->getAccountType() == "Current") ++current;
    }
    return {savings, current};
}

long long Bank::submitServiceRequest(int accountNumber,
                                     const std::string& description) {
    if (findCustomer(accountNumber) == nullptr) {
        throw BankingException("Account number was not found.");
    }
    if (description.empty()) throw BankingException("Request description cannot be empty.");

    const long long requestId = nextRequestId_++;
    serviceRequests_.push(ServiceRequest{requestId, accountNumber,
                                         description, currentDateTime()});
    try {
        saveAll();
    } catch (...) {
        std::queue<ServiceRequest> restored;
        while (serviceRequests_.size() > 1) {
            restored.push(serviceRequests_.front());
            serviceRequests_.pop();
        }
        if (!serviceRequests_.empty()) serviceRequests_.pop();
        serviceRequests_ = std::move(restored);
        --nextRequestId_;
        try { saveAll(); } catch (...) { /* Keep the original persistence error. */ }
        throw;
    }
    return requestId;
}

ServiceRequest Bank::processNextServiceRequest() {
    if (serviceRequests_.empty()) throw BankingException("There are no pending service requests.");

    const auto previousQueue = serviceRequests_;
    const ServiceRequest request = serviceRequests_.front();
    serviceRequests_.pop();
    try {
        saveAll();
    } catch (...) {
        serviceRequests_ = previousQueue;
        try { saveAll(); } catch (...) { /* Keep the original persistence error. */ }
        throw;
    }
    return request;
}
