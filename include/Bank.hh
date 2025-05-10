#pragma once // Header guard

#include <string>
#include <vector>
#include <unordered_map> // Changed from std::map
#include <memory>
#include <optional>
#include <random>

#include "Transaction.hh" // Transaction, TransactionType
#include "Customer.hh"    // Customer
#include "Account.hh"     // Account, AccountType

namespace banking_system {

// File: Bank.hh
// Purpose: Defines the Bank class, the central orchestrator of the banking system.
class Bank {
public:
    Bank();
    ~Bank() = default;

    Bank(const Bank&) = delete;
    Bank& operator=(const Bank&) = delete;
    Bank(Bank&&) = default;
    Bank& operator=(Bank&&) = default;

    // Customer Management
    Customer* registerCustomer(const std::string& name);
    Customer* findCustomer(const std::string& name);
    const Customer* findCustomer(const std::string& name) const;
    const std::vector<std::unique_ptr<Customer>>& getAllCustomers() const;

    // Account Management
    Account* findAccount(const std::string& accountId);
    const Account* findAccount(const std::string& accountId) const;
    // *** Changed to std::unordered_map to match private member ***
    const std::unordered_map<std::string, std::unique_ptr<Account>>& getAllAccounts() const;
    std::vector<Account*> getCustomerAccounts(const std::string& customerName);
    std::vector<const Account*> getCustomerAccounts(const std::string& customerName) const;

    // Transaction Operations (returns empty optional on failure)
    std::optional<Transaction> performDeposit(const std::string& accountId,
                                              double amount,
                                              const std::string& note = "");
    std::optional<Transaction> performWithdraw(const std::string& accountId,
                                               double amount,
                                               const std::string& note = "");
    std::optional<Transaction> performTransfer(const std::string& srcAccountId,
                                               const std::string& dstAccountId,
                                               double amount,
                                               const std::string& note = "");

    // Reporting
    std::vector<Transaction> getAllTransactionsChronological() const;
    std::vector<Transaction> getCustomerTransactionsChronological(const std::string& customerName) const;
    std::vector<Transaction> getAccountTransactionsChronological(const std::string& accountId) const;

    bool generateGlobalReport(const std::string& filename) const;
    bool generateCustomerReport(const std::string& customerName,
                                const std::string& filename) const;
    bool generateAccountReport(const std::string& accountId,
                               const std::string& filename) const;

private:
    std::vector<std::unique_ptr<Customer>> customers_;
    std::unordered_map<std::string, std::unique_ptr<Account>> accounts_; // Now unordered_map
    std::vector<Transaction> transactions_;
    std::unordered_map<std::string, Customer*> customerIndex_; // Now unordered_map, raw pointers are fine as customers_ owns them

    std::mt19937 randomEngine_{std::random_device{}()}; // In-class initialization for random engine
    // *** Added branchDist_ declaration ***
    std::uniform_int_distribution<int> branchDist_;
    // *** Removed in-class initializer for accountNumDist_ to be initialized in constructor ***
    std::uniform_int_distribution<long long> accountNumDist_;

    long long nextTransactionId_ = 1;

    // Helpers
    std::string generateUniqueAccountId(AccountType type);
    std::string generateUniqueTransactionId();
    void recordTransaction(const Transaction& transaction);
    bool accountExists(const std::string& accountId) const;
    bool customerExists(const std::string& name) const;
};

} // namespace banking_system
