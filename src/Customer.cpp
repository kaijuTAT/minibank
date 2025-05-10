#include "Customer.hh"
#include <stdexcept> // For std::invalid_argument
#include <algorithm> // For std::find

namespace banking_system {

// Constructor implementation
Customer::Customer(const std::string& name) : name_(name) {
    if (name.empty()) {
        throw std::invalid_argument("Customer name cannot be empty.");
    }
}

// Getter for customer's name
const std::string& Customer::getName() const {
    return name_;
}

// Getter for the list of account IDs
const std::vector<std::string>& Customer::getAccountIds() const {
    return accountIds_;
}

// Adds an account ID to the customer's list
void Customer::addAccountId(const std::string& accountId) {
    if (accountId.empty()) {
        throw std::invalid_argument("Account ID cannot be empty when adding to customer.");
    }
    // Prevent duplicate account IDs for the same customer
    if (std::find(accountIds_.begin(), accountIds_.end(), accountId) == accountIds_.end()) {
        accountIds_.push_back(accountId);
    }
}

} // namespace banking_system
