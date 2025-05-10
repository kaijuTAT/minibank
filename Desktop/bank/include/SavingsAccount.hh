#pragma once // Header guard

#include "Account.hh" // Include base class header

namespace banking_system {

// Represents a savings account, inheriting from Account.
// Savings accounts have restrictions on deposits, withdrawals, and transfers.
class SavingsAccount : public Account {
public:
    // Constructor: Initializes a SavingsAccount object.
    // Calls the base class Account constructor.
    SavingsAccount(const std::string& accountId, const std::string& ownerName, double initialBalance);

    // --- Overridden base class methods ---

    // Returns the type of the account (SAVINGS).
    AccountType getType() const override;

    // Savings accounts typically do not allow direct deposits or withdrawals.
    // These will use the base class's default (disallowed) behavior.
 

   
    // bool transfer(Account* destinationAccount, double amount, const std::string& note = "") override;

    // Default virtual destructor is sufficient if no specific resources are managed here.
    ~SavingsAccount() override = default;

private:
    // Specific attributes or methods for SavingsAccount can be added here if needed.
};

} // namespace banking_system
