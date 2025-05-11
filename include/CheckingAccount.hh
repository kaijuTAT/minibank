#pragma once 

#include "Account.hh" 

namespace banking_system {

// this header file Represents a checking account, inheriting from Account.
// Checking accounts typically allow deposits, withdrawals, and more flexible transfers.
class CheckingAccount : public Account {
public:

    CheckingAccount(const std::string& accountId, const std::string& ownerName, double initialBalance);


    AccountType getType() const override;

    
    ~CheckingAccount() override = default;

private:
};

} 
