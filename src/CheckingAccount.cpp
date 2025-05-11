#include "CheckingAccount.hh"

namespace banking_system {

// Constructor implementation
CheckingAccount::CheckingAccount(const std::string& accountId, const std::string& ownerName, double initialBalance)
    : Account(accountId, ownerName, initialBalance) { 
    
}

// Overridden method to return the account type
AccountType CheckingAccount::getType() const {
    return AccountType::CHECKING;
}

// Note: As with SavingsAccount, the Bank class manages the actual deposit, withdraw, and transfer operations based on account type. This class primarily serves toidentify itself as a CheckingAccount.

}
