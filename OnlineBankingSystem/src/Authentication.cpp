#include "Authentication.h"

bool Authentication::verifyCustomer(const Customer& customer,
                                     const std::string& password) {
    return customer.verifyPassword(password);
}

bool Authentication::verifyAdministrator(const Admin& admin,
                                          const std::string& username,
                                          const std::string& password) {
    return admin.authenticate(username, password);
}
