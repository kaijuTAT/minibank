#pragma once 

#include <string>
#include <vector>
#include <memory> 

namespace banking_system {

// Forward declaration of Account class
class Account;

// File: Customer.hh
// Purpose: Defines the Customer class, which represents a bank customer.
// It stores the customer's name and a list of account IDs associated with them.
// Note: This class primarily holds customer data; account objects themselves are managed by the Bank class.
class Customer {
public: 
    explicit Customer(const std::string& name);

    // --- Getters ---
    const std::string& getName() const;

    // Returns a list of account IDs associated with this customer.
    const std::vector<std::string>& getAccountIds() const;

    // --- Account Management ---
    void addAccountId(const std::string& accountId);

private:
    std::string name_;                  // Customer's name
    std::vector<std::string> accountIds_; 
};

} 
