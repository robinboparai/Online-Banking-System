#ifndef ADMIN_H
#define ADMIN_H

#include <string>

class Admin {
private:
    std::string username_;
    std::string password_; // Plain text is used only for this academic simulation.

public:
    Admin(std::string username = "admin", std::string password = "admin123");
    bool authenticate(const std::string& username,
                      const std::string& password) const;
    const std::string& username() const { return username_; }
    const std::string& passwordForPersistence() const { return password_; }
    void setCredentials(std::string username, std::string password);
};

#endif
