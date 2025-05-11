#pragma once 

#include <string>
#include <vector>
#include <memory> 
#include <ctime> 

namespace banking_system {

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

    virtual ~Account() = default;

    // --- Getters for account details ---
    const std::string& getAccountId() const;
    const std::string& getOwnerName() const; 
    double getBalance() const;       

    // --- virtual function for account type(and it`s pure jaja) ---
    virtual AccountType getType() const = 0;

    // --- Virtual functions for account operations  ---
    virtual bool deposit(double amount, const std::string& note = "");
    virtual bool withdraw(double amount, const std::string& note = "");
    virtual bool transfer(Account* destinationAccount, double amount, const std::string& note = "");

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

} 
