#pragma once // Header guard

#include <string>
#include <vector>
#include <unordered_map> // Using unordered_map
#include <memory>
#include <optional>
#include <random>

#include "Transaction.hh"
#include "Customer.hh"
#include "Account.hh"

namespace banking_system {

// File: Bank.hh
// Purpose: Defines the Bank class, the central orchestrator of the banking system.
class Bank {
public:
    Bank();
    ~Bank() = default; // Default destructor is fine

    // --- Canonical Form: Non-copyable and Non-movable ---
    // By deleting the copy operations, and not declaring move operations,
    // the class becomes non-copyable and non-movable by default compiler behavior.
    Bank(const Bank&) = delete;            // Delete copy constructor
    Bank& operator=(const Bank&) = delete; // Delete copy assignment operator
    // Move constructor and move assignment operator are implicitly not generated
    // or are considered deleted due to the user-declared copy operations.
    // No '&&' syntax will appear.
    // ----------------------------------------------------

    // Customer Management
    Customer* registerCustomer(const std::string& name);
    Customer* findCustomer(const std::string& name);
    const Customer* findCustomer(const std::string& name) const;
    const std::vector<std::unique_ptr<Customer>>& getAllCustomers() const;

    // Account Management
    Account* findAccount(const std::string& accountId);
    const Account* findAccount(const std::string& accountId) const;
    const std::unordered_map<std::string, std::unique_ptr<Account>>& getAllAccounts() const;
    std::vector<Account*> getCustomerAccounts(const std::string& customerName);
    std::vector<const Account*> getCustomerAccounts(const std::string& customerName) const;

    // Transaction Operations
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
    std::unordered_map<std::string, std::unique_ptr<Account>> accounts_;
    std::vector<Transaction> transactions_;
    std::unordered_map<std::string, Customer*> customerIndex_;

    std::mt19937 randomEngine_{std::random_device{}()};
    std::uniform_int_distribution<int> branchDist_;
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
