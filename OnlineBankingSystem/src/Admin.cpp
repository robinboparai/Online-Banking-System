#include "Admin.h"

#include <utility>

Admin::Admin(std::string username, std::string password)
    : username_(std::move(username)), password_(std::move(password)) {}

bool Admin::authenticate(const std::string& username,
                         const std::string& password) const {
    return username_ == username && password_ == password;
}

void Admin::setCredentials(std::string username, std::string password) {
    username_ = std::move(username);
    password_ = std::move(password);
}
