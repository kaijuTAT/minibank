#pragma once // Prevents multiple inclusions of this header file

#include <string>
#include <vector>
#include <memory> // For smart pointers like std::unique_ptr
#include <ctime>  // For time-related functionalities

namespace banking_system {

// Forward declaration for Transaction class
class Transaction;

// Enum to represent different types of accounts
enum class AccountType {
    SAVINGS,
    CHECKING
};

// File: Account.hh
// Purpose: Defines the Account class, which serves as an abstract base class
// for all types of bank accounts in the system. It outlines common
// properties (like account ID, owner, balance) and behaviors (like deposit,
// withdraw, transfer) that specific account types will implement or override.
class Account {
public:
    // Constructor: Initializes an Account object.
    // accountId: Unique identifier for the account.
    // ownerName: Name of the account holder.
    // initialBalance: The starting balance of the account.
    Account(const std::string& accountId, const std::string& ownerName, double initialBalance);

    // Virtual destructor: Essential for proper cleanup in derived classes.
    virtual ~Account() = default;

    // --- Getters for account details ---
    const std::string& getAccountId() const; // Returns the account ID.
    const std::string& getOwnerName() const; // Returns the owner's name.
    double getBalance() const;             // Returns the current balance.

    // --- Pure virtual function for account type ---
    // Derived classes must implement this to specify their type.
    virtual AccountType getType() const = 0;

    // --- Virtual functions for account operations (defaulted to not allowed) ---
    // These are overridden by derived classes (e.g., CheckingAccount) if the operation is permitted.
    virtual bool deposit(double amount, const std::string& note = "");
    virtual bool withdraw(double amount, const std::string& note = "");
    virtual bool transfer(Account* destinationAccount, double amount, const std::string& note = "");

    // Public method to update the account balance.
    // Typically called by the Bank class after validating an operation.
    void setBalance(double newBalance);

protected:
    // Data members accessible by derived classes
    std::string accountId_;
    std::string ownerName_;
    double balance_;

private:
    // Helper function to validate if an amount is positive.
    bool isValidAmount(double amount) const;

    // Disable copy operations to prevent slicing and unintended copies.
    Account(const Account&) = delete;
    Account& operator=(const Account&) = delete;
};

} // namespace banking_system
