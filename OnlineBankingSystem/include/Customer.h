#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>

class Customer {
private:
    int accountNumber_{};
    std::string name_;
    int age_{};
    std::string mobile_;
    std::string email_;
    std::string password_; // Plain text is used only for this academic simulation.

public:
    Customer() = default;
    Customer(int accountNumber, std::string name, int age,
             std::string mobile, std::string email, std::string password);

    int getAccountNumber() const { return accountNumber_; }
    const std::string& getName() const { return name_; }
    int getAge() const { return age_; }
    const std::string& getMobile() const { return mobile_; }
    const std::string& getEmail() const { return email_; }

    bool verifyPassword(const std::string& password) const;
    void changePassword(const std::string& currentPassword,
                        const std::string& newPassword);
    void updateContactDetails(const std::string& mobile,
                              const std::string& email);

    // Persistence access is deliberately limited to the Bank class.
    const std::string& passwordForPersistence() const { return password_; }
};

#endif
