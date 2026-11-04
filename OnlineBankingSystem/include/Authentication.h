#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "Admin.h"
#include "Customer.h"
#include <string>

class Authentication {
public:
    static bool verifyCustomer(const Customer& customer,
                               const std::string& password);
    static bool verifyAdministrator(const Admin& admin,
                                    const std::string& username,
                                    const std::string& password);
};

#endif
