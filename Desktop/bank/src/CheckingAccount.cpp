#include "CheckingAccount.hh"

namespace banking_system {

// Constructor implementation
CheckingAccount::CheckingAccount(const std::string& accountId, const std::string& ownerName, double initialBalance)
    : Account(accountId, ownerName, initialBalance) { // Call base class constructor
    // Any CheckingAccount-specific initialization logic would go here.
}

// Overridden method to return the account type
AccountType CheckingAccount::getType() const {
    return AccountType::CHECKING;
}

// Note: As with SavingsAccount, the Bank class manages the actual deposit, withdraw,
// and transfer operations based on account type. This class primarily serves to
// identify itself as a CheckingAccount.

} // namespace banking_system
