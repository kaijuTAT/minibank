#include "Account.hh"
#include <stdexcept> 
#include <limits>    
#include <iostream>  

namespace banking_system {

// Constructor implementation
Account::Account(const std::string& accountId, const std::string& ownerName, double initialBalance)
    : accountId_(accountId), ownerName_(ownerName), balance_(0.0) {
    if (accountId.empty()) {
        throw std::invalid_argument("Account ID cannot be empty.");
    }
    if (ownerName.empty()) {
        throw std::invalid_argument("Owner name cannot be empty.");
    }
    if (initialBalance < 0.0) {
         throw std::invalid_argument("Initial balance cannot be negative.");
    }
    // Initial balance is set directly; in a real system, this might be via a transaction.
    balance_ = initialBalance;
}

// Getter for accountId_
const std::string& Account::getAccountId() const {
    return accountId_;
}

// Getter for ownerName_
const std::string& Account::getOwnerName() const {
    return ownerName_;
}

// Getter for balance_
double Account::getBalance() const {
    return balance_;
}

// Base class default implementation for deposit (not allowed)
bool Account::deposit(double amount, const std::string& note) {
    // std::cerr << "Debug: Base Account::deposit called for " << accountId_ << std::endl;
    return false; // Operation not permitted for base account type
}

// Base class default implementation for withdraw (not allowed)
bool Account::withdraw(double amount, const std::string& note) {
    // std::cerr << "Debug: Base Account::withdraw called for " << accountId_ << std::endl;
    return false; // Operation not permitted for base account type
}

// Base class default implementation for transfer (not allowed)
bool Account::transfer(Account* destinationAccount, double amount, const std::string& note) {
    // std::cerr << "Debug: Base Account::transfer called for " << accountId_ << std::endl;
    return false; // Operation not permitted for base account type
}

// Setter for balance_
void Account::setBalance(double newBalance) {
    // Basic validation; more complex rules could be added here or in Bank class.
    if (newBalance < 0.0) {
        // For this system, we don't allow negative balances directly through setBalance.
        // Overdrafts would need specific handling.
        std::cerr << "Warning: Attempted to set negative balance for account " << accountId_ << ". Operation might be rejected by business logic." << std::endl;
        // Not throwing an exception here, as the Bank class should enforce balance rules.
    }
    balance_ = newBalance;
}

// Private helper to check if an amount is valid (positive)
bool Account::isValidAmount(double amount) const {
    return amount > 0.0 &&
           amount <= std::numeric_limits<double>::max(); // Check against max value
}

} // namespace banking_system
