#pragma once // Header guard

#include <string>
#include <vector>
#include <memory> // For std::unique_ptr if Customer were to own Account objects

namespace banking_system {

// Forward declaration of Account class
class Account;

// File: Customer.hh
// Purpose: Defines the Customer class, which represents a bank customer.
// It stores the customer's name and a list of account IDs associated with them.
// Note: This class primarily holds customer data; account objects themselves are managed by the Bank class.
class Customer {
public:
    // Constructor: Initializes a Customer object with a name.
    // 'explicit' prevents accidental implicit conversions.
    explicit Customer(const std::string& name);

    // --- Getters ---
    // Returns the customer's name.
    const std::string& getName() const;

    // Returns a list of account IDs associated with this customer.
    const std::vector<std::string>& getAccountIds() const;

    // --- Account Management ---
    // Adds an account ID to this customer's list of accounts.
    // This is typically called by the Bank class when an account is created for the customer.
    void addAccountId(const std::string& accountId);

private:
    std::string name_;                  // Customer's name
    std::vector<std::string> accountIds_; // List of account IDs owned by the customer
};

} // namespace banking_system
