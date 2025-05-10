#pragma once // Header guard

#include "Account.hh" // Include base class header

namespace banking_system {

// Represents a checking account, inheriting from Account.
// Checking accounts typically allow deposits, withdrawals, and more flexible transfers.
class CheckingAccount : public Account {
public:
    // Constructor: Initializes a CheckingAccount object.
    // Calls the base class Account constructor.
    CheckingAccount(const std::string& accountId, const std::string& ownerName, double initialBalance);

    // --- Overridden base class methods ---

    // Returns the type of the account (CHECKING).
    AccountType getType() const override;

    // Checking accounts allow deposits and withdrawals.
    // The actual operation logic is handled by the Bank class, but these overrides
    // could indicate permission if the design required it. For now, they can
    // rely on the Bank class to check type before calling Account::setBalance.
    // bool deposit(double amount, const std::string& note = "") override;
    // bool withdraw(double amount, const std::string& note = "") override;

    // Transfer logic for checking accounts is also handled by the Bank class.
    // bool transfer(Account* destinationAccount, double amount, const std::string& note = "") override;

    // Default virtual destructor.
    ~CheckingAccount() override = default;

private:
    // Specific attributes or methods for CheckingAccount can be added here if needed.
};

} // namespace banking_system
