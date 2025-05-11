#pragma once 

#include "Account.hh" 

namespace banking_system {

// Represents a savings account, inheriting from Account.
// Savings accounts have restrictions on deposits, withdrawals, and transfers.
class SavingsAccount : public Account {
public:
    
    SavingsAccount(const std::string& accountId, const std::string& ownerName, double initialBalance);

    AccountType getType() const override;

    // Savings accounts typically do not allow direct deposits or withdrawals.
 
    // Default virtual destructor is sufficient if no specific resources are managed here.
    ~SavingsAccount() override = default;

private:
};

} 
