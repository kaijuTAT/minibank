#include "SavingsAccount.hh"

namespace banking_system {

SavingsAccount::SavingsAccount(const std::string& accountId, const std::string& ownerName, double initialBalance)
    : Account(accountId, ownerName, initialBalance) { // Call base class constructor
}

AccountType SavingsAccount::getType() const {
    return AccountType::SAVINGS;
}


} 