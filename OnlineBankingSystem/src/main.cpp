#include "Bank.h"
#include "BankingException.h"
#include "Money.h"
#include "TemplateUtils.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string trim(std::string value) {
    auto notSpace = [](unsigned char character) { return !std::isspace(character); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
    value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
    return value;
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    if (!std::getline(std::cin, value)) {
        throw BankingException("Input stream closed. Exiting the application.");
    }
    return trim(value);
}

int readInteger(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        const std::string value = readLine(prompt);
        try {
            std::size_t consumed = 0;
            const long long number = std::stoll(value, &consumed);
            if (consumed == value.size() && number >= minimum && number <= maximum) {
                return static_cast<int>(number);
            }
        } catch (const std::exception&) {
            // Show the same validation message for malformed and out-of-range input.
        }
        std::cout << "Enter a whole number between " << minimum
                  << " and " << maximum << ".\n";
    }
}

long long parseMoney(const std::string& rawValue) {
    const std::string value = trim(rawValue);
    if (value.empty() || value.front() == '-') {
        throw BankingException("Enter a valid non-negative amount, for example 1250.50.");
    }

    const std::size_t decimalPoint = value.find('.');
    if (decimalPoint != std::string::npos && value.find('.', decimalPoint + 1) != std::string::npos) {
        throw BankingException("Amount can contain only one decimal point.");
    }

    const std::string wholeText = decimalPoint == std::string::npos
        ? value : value.substr(0, decimalPoint);
    std::string fractionText = decimalPoint == std::string::npos
        ? "" : value.substr(decimalPoint + 1);
    if (wholeText.empty() && fractionText.empty()) {
        throw BankingException("Enter a valid amount.");
    }
    if (fractionText.size() > 2) {
        throw BankingException("Enter no more than two digits after the decimal point.");
    }

    auto allDigits = [](const std::string& text) {
        return std::all_of(text.begin(), text.end(), [](unsigned char character) {
            return std::isdigit(character);
        });
    };
    if ((!wholeText.empty() && !allDigits(wholeText)) ||
        (!fractionText.empty() && !allDigits(fractionText))) {
        throw BankingException("Amount must contain digits only, with an optional decimal point.");
    }

    const std::string safeWhole = wholeText.empty() ? "0" : wholeText;
    const long long whole = std::stoll(safeWhole);
    if (whole > (std::numeric_limits<long long>::max() / 100LL)) {
        throw BankingException("Amount is too large.");
    }
    if (fractionText.size() == 1) fractionText += '0';
    const long long fraction = fractionText.empty() ? 0 : std::stoll(fractionText);
    const long long base = whole * 100LL;
    if (fraction > std::numeric_limits<long long>::max() - base) {
        throw BankingException("Amount is too large.");
    }
    return base + fraction;
}

long long readMoney(const std::string& prompt) {
    while (true) {
        try {
            return parseMoney(readLine(prompt));
        } catch (const BankingException& error) {
            std::cout << error.what() << '\n';
        } catch (const std::exception&) {
            std::cout << "Amount is too large or incorrectly formatted.\n";
        }
    }
}

void printHeading(const std::string& heading) {
    std::cout << "\n==============================================\n"
              << "  " << heading << "\n"
              << "==============================================" << std::endl;
}

void showCustomerDetails(const Bank& bank, int accountNumber) {
    const Customer* customer = bank.findCustomer(accountNumber);
    const Account* account = bank.findAccount(accountNumber);
    if (customer == nullptr || account == nullptr) {
        throw BankingException("Customer or account record was not found.");
    }

    printHeading("ACCOUNT DETAILS");
    printField("Account number", customer->getAccountNumber());
    printField("Customer name", customer->getName());
    printField("Age", customer->getAge());
    printField("Mobile number", customer->getMobile());
    printField("Email address", customer->getEmail());
    printField("Account type", account->getAccountType());
    printField("Available balance", formatMoney(account->getBalancePaise()));
    printField("Account rule", account->accountRule());
}

void showTransactionRows(const std::vector<Transaction>& transactions) {
    if (transactions.empty()) {
        std::cout << "No transaction records found.\n";
        return;
    }

    std::cout << '\n' << std::left << std::setw(8) << "ID"
              << std::setw(18) << "TYPE" << std::setw(18) << "AMOUNT"
              << std::setw(21) << "DATE / TIME" << std::setw(12) << "STATUS"
              << "BALANCE AFTER\n";
    std::cout << std::string(95, '-') << '\n';
    for (const auto& transaction : transactions) {
        std::cout << std::left << std::setw(8) << transaction.transactionId
                  << std::setw(18) << transaction.type
                  << std::setw(18) << formatMoney(transaction.amountPaise)
                  << std::setw(21) << transaction.dateTime
                  << std::setw(12) << transaction.status
                  << formatMoney(transaction.balanceAfterPaise);
        if (transaction.counterpartyAccount != 0) {
            std::cout << "  (A/c " << transaction.counterpartyAccount << ')';
        }
        std::cout << '\n';
    }
}

void customerMenu(Bank& bank, int accountNumber) {
    while (true) {
        printHeading("CUSTOMER DASHBOARD");
        std::cout << "1. View Account Details\n"
                  << "2. Check Available Balance\n"
                  << "3. Deposit Money\n"
                  << "4. Withdraw Money\n"
                  << "5. Transfer Funds\n"
                  << "6. View Transaction History\n"
                  << "7. Change Account Password\n"
                  << "8. Calculate Interest (Savings Account)\n"
                  << "9. Logout\n"
                  << "10. Update Contact Details\n"
                  << "11. Submit a Service Request\n";

        const int choice = readInteger("Enter your choice: ", 1, 11);
        try {
            switch (choice) {
                case 1:
                    showCustomerDetails(bank, accountNumber);
                    break;
                case 2: {
                    const Account* account = bank.findAccount(accountNumber);
                    if (account == nullptr) throw BankingException("Account record was not found.");
                    std::cout << "Available balance: "
                              << formatMoney(account->getBalancePaise()) << '\n';
                    break;
                }
                case 3: {
                    const long long amount = readMoney("Enter deposit amount (INR): ");
                    bank.deposit(accountNumber, amount);
                    std::cout << "Deposit completed.\n";
                    break;
                }
                case 4: {
                    const long long amount = readMoney("Enter withdrawal amount (INR): ");
                    bank.withdraw(accountNumber, amount);
                    std::cout << "Withdrawal completed.\n";
                    break;
                }
                case 5: {
                    const int recipient = readInteger("Enter recipient account number: ", 1,
                                                      std::numeric_limits<int>::max());
                    const long long amount = readMoney("Enter transfer amount (INR): ");
                    bank.transfer(accountNumber, recipient, amount);
                    std::cout << "Fund transfer completed successfully.\n";
                    break;
                }
                case 6:
                    showTransactionRows(bank.transactionHistory(accountNumber));
                    break;
                case 7: {
                    const std::string current = readLine("Enter current password: ");
                    const std::string next = readLine("Enter new password: ");
                    bank.changePassword(accountNumber, current, next);
                    std::cout << "Password changed successfully.\n";
                    break;
                }
                case 8: {
                    const Account* account = bank.findAccount(accountNumber);
                    if (account == nullptr) throw BankingException("Account record was not found.");
                    if (account->getAccountType() != "Savings") {
                        std::cout << "Interest calculation is available only for Savings Accounts.\n";
                    } else {
                        std::cout << "Estimated one-year simple interest: "
                                  << formatMoney(bank.calculateInterest(accountNumber)) << '\n';
                    }
                    break;
                }
                case 9:
                    std::cout << "You have been logged out.\n";
                    return;
                case 10: {
                    const std::string mobile = readLine("Enter updated mobile number: ");
                    const std::string email = readLine("Enter updated email address: ");
                    bank.updateCustomer(accountNumber, mobile, email);
                    std::cout << "Contact details updated.\n";
                    break;
                }
                case 11: {
                    const std::string description = readLine("Describe your service request: ");
                    const long long requestId = bank.submitServiceRequest(accountNumber, description);
                    std::cout << "Service request submitted. Request ID: " << requestId << '\n';
                    break;
                }
                default:
                    break;
            }
        } catch (const BankingException& error) {
            std::cout << "Operation could not be completed: " << error.what() << '\n';
        }
    }
}

void administratorMenu(Bank& bank) {
    while (true) {
        printHeading("ADMINISTRATOR DASHBOARD");
        std::cout << "1. View All Registered Customers\n"
                  << "2. Search Customer by Account Number\n"
                  << "3. View All Bank Accounts\n"
                  << "4. View Transaction Records\n"
                  << "5. Display Total Bank Deposits\n"
                  << "6. View Account Statistics\n"
                  << "7. Process Next Service Request\n"
                  << "8. Logout\n";

        const int choice = readInteger("Enter your choice: ", 1, 8);
        try {
            switch (choice) {
                case 1: {
                    printHeading("REGISTERED CUSTOMERS");
                    if (bank.customers().empty()) {
                        std::cout << "No customers are registered.\n";
                        break;
                    }
                    for (const auto& [number, customer] : bank.customers()) {
                        printField("Account number", number);
                        printField("Customer name", customer.getName());
                        printField("Mobile number", customer.getMobile());
                        printField("Email address", customer.getEmail());
                        std::cout << "----------------------------------------------\n";
                    }
                    break;
                }
                case 2: {
                    const int number = readInteger("Enter account number: ", 1,
                                                   std::numeric_limits<int>::max());
                    showCustomerDetails(bank, number);
                    break;
                }
                case 3: {
                    printHeading("ALL BANK ACCOUNTS");
                    if (bank.accounts().empty()) {
                        std::cout << "No accounts are registered.\n";
                        break;
                    }
                    const Account* highestBalance = nullptr;
                    for (const auto& [number, accountOwner] : bank.accounts()) {
                        const Account* account = accountOwner.get(); // Base-class pointer; virtual dispatch follows.
                        std::cout << *account << '\n';
                        printField("Account number", number);
                        printField("Balance", formatMoney(account->getBalancePaise()));
                        printField("Account rule", account->accountRule());
                        if (highestBalance == nullptr || *account > *highestBalance) {
                            highestBalance = account;
                        }
                        std::cout << "----------------------------------------------\n";
                    }
                    if (highestBalance != nullptr) {
                        std::cout << "Highest balance: Account "
                                  << highestBalance->getAccountNumber() << " - "
                                  << formatMoney(highestBalance->getBalancePaise()) << '\n';
                    }
                    break;
                }
                case 4:
                    printHeading("ALL TRANSACTION RECORDS");
                    showTransactionRows(bank.transactions());
                    break;
                case 5:
                    std::cout << "Total current bank deposits: "
                              << formatMoney(bank.totalDepositsPaise()) << '\n';
                    break;
                case 6: {
                    const auto [savings, current] = bank.accountStatistics();
                    printField("Total customers", bank.customers().size());
                    printField("Total accounts", bank.accounts().size());
                    printField("Savings accounts", savings);
                    printField("Current accounts", current);
                    printField("Recorded transactions", bank.transactions().size());
                    printField("Pending service requests", bank.pendingServiceRequestCount());
                    break;
                }
                case 7: {
                    const ServiceRequest request = bank.processNextServiceRequest();
                    printHeading("SERVICE REQUEST PROCESSED");
                    printField("Request ID", request.requestId);
                    printField("Account number", request.accountNumber);
                    printField("Description", request.description);
                    printField("Submitted at", request.submittedAt);
                    break;
                }
                case 8:
                    std::cout << "Administrator logged out.\n";
                    return;
                default:
                    break;
            }
        } catch (const BankingException& error) {
            std::cout << "Operation could not be completed: " << error.what() << '\n';
        }
    }
}

void showAbout() {
    printHeading("ABOUT THE SYSTEM");
    std::cout << "A C++17 console simulation of basic banking operations.\n"
              << "It demonstrates OOP, inheritance, polymorphism, exception handling,\n"
              << "file persistence, templates, pointers, and STL containers.\n"
              << "This academic program is not intended for real financial use.\n";
}

void registerCustomer(Bank& bank) {
    printHeading("CREATE A NEW ACCOUNT");
    const std::string name = readLine("Customer name: ");
    const int age = readInteger("Age: ", 1, 120);
    const std::string mobile = readLine("Mobile number: ");
    const std::string email = readLine("Email address: ");
    const std::string password = readLine("Create login password (minimum 4 characters): ");
    std::cout << "Account type\n1. Savings Account\n2. Current Account\n";
    const int typeChoice = readInteger("Choose account type: ", 1, 2);
    const long long openingDeposit = readMoney("Initial deposit (INR; enter 0 if none): ");

    const std::string accountType = typeChoice == 1 ? "Savings" : "Current";
    const int accountNumber = bank.createAccount(name, age, mobile, email,
        password, accountType, openingDeposit);

    std::cout << "\nAccount created successfully.\n"
              << "Your account number is: " << accountNumber << '\n'
              << "Please keep this number for future login.\n";
}
} // namespace

int main() {
    try {
        Bank bank("data");
        bank.load();
        std::cout << "\nWelcome to the Online Banking System.\n";

        while (true) {
            printHeading("ONLINE BANKING SYSTEM");
            std::cout << "1. Create New Account\n"
                      << "2. Customer Login\n"
                      << "3. Administrator Login\n"
                      << "4. About the System\n"
                      << "5. Exit\n";

            const int choice = readInteger("Enter your choice: ", 1, 5);
            try {
                switch (choice) {
                    case 1:
                        registerCustomer(bank);
                        break;
                    case 2: {
                        const int accountNumber = readInteger("Account number: ", 1,
                                                              std::numeric_limits<int>::max());
                        const std::string password = readLine("Password: ");
                        if (!bank.authenticateCustomer(accountNumber, password)) {
                            throw BankingException("Invalid account number or password.");
                        }
                        std::cout << "Login successful. Welcome, "
                                  << bank.findCustomer(accountNumber)->getName() << ".\n";
                        customerMenu(bank, accountNumber);
                        break;
                    }
                    case 3: {
                        const std::string username = readLine("Administrator username: ");
                        const std::string password = readLine("Administrator password: ");
                        if (!bank.authenticateAdmin(username, password)) {
                            throw BankingException("Invalid administrator credentials.");
                        }
                        std::cout << "Administrator login successful.\n";
                        administratorMenu(bank);
                        break;
                    }
                    case 4:
                        showAbout();
                        break;
                    case 5:
                        std::cout << "Thank you for using the Online Banking System.\n";
                        return 0;
                }
            } catch (const BankingException& error) {
                std::cout << "Error: " << error.what() << '\n';
            }
        }
    } catch (const BankingException& error) {
        std::cerr << "Startup error: " << error.what() << '\n';
        return 1;
    }
}
