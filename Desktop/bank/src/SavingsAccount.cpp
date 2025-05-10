#include "SavingsAccount.hh"

namespace banking_system {

// Constructor implementation
SavingsAccount::SavingsAccount(const std::string& accountId, const std::string& ownerName, double initialBalance)
    : Account(accountId, ownerName, initialBalance) { // Call base class constructor
    // Any SavingsAccount-specific initialization logic would go here.
}

// Overridden method to return the account type
AccountType SavingsAccount::getType() const {
    return AccountType::SAVINGS;
}

// Note: Deposit, withdraw, and transfer operations are primarily managed by the Bank class,
// which checks account types and enforces rules. This class mainly identifies itself
// and relies on the base Account class for default (disallowed) operation behaviors
// if they were to be called directly (which they typically aren't for these actions).

} // namespace banking_system
