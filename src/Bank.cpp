#include "Bank.hh"
#include "Customer.hh"
#include "Account.hh"
#include "SavingsAccount.hh"
#include "CheckingAccount.hh"
#include "Transaction.hh"
#include "Utils.hh"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace banking_system {

// Constructor: Initializes random number distributions and seeds the engine.
// Initializes branchDist_ and accountNumDist_ in the member initializer list.
Bank::Bank() : branchDist_(0, 9999), accountNumDist_(10000000LL, 99999999LL) { // Adjusted accountNumDist_ range to ensure 8 digits
    // randomEngine_ is already initialized in-class using std::random_device
    // If you want to seed it here as well (e.g. for more variability or specific seed):
    // auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    // randomEngine_.seed(seed);
    std::cout << "Random engine initialized for Bank operations." << std::endl;
}

// --- Customer Management Implementations ---
Customer* Bank::registerCustomer(const std::string& name) {
    if (customerExists(name)) {
        std::cerr << "Error: Customer '" << name << "' already exists." << std::endl;
        return nullptr;
    }

    auto newCustomer = std::make_unique<Customer>(name);
    Customer* customerPtr = newCustomer.get();

    std::string savingsAccountId = generateUniqueAccountId(AccountType::SAVINGS);
    auto savingsAccount = std::make_unique<SavingsAccount>(savingsAccountId, name, 0.0);
    customerPtr->addAccountId(savingsAccountId);

    std::string checkingAccountId = generateUniqueAccountId(AccountType::CHECKING);
    auto checkingAccount = std::make_unique<CheckingAccount>(checkingAccountId, name, 0.0);
    customerPtr->addAccountId(checkingAccountId);

    accounts_[savingsAccountId] = std::move(savingsAccount);
    accounts_[checkingAccountId] = std::move(checkingAccount);

    customers_.push_back(std::move(newCustomer));
    customerIndex_[name] = customerPtr;

    std::cout << "Customer [" << name << "] registered. Accounts created:\n"
              << "- Savings: " << savingsAccountId << "\n"
              << "- Checking: " << checkingAccountId << std::endl;
    return customerPtr;
}

Customer* Bank::findCustomer(const std::string& name) {
    auto it = customerIndex_.find(name);
    return (it != customerIndex_.end()) ? it->second : nullptr;
}

const Customer* Bank::findCustomer(const std::string& name) const {
    auto it = customerIndex_.find(name);
    return (it != customerIndex_.end()) ? it->second : nullptr;
}

const std::vector<std::unique_ptr<Customer>>& Bank::getAllCustomers() const {
    return customers_;
}

// --- Account Management Implementations ---
Account* Bank::findAccount(const std::string& accountId) {
    auto it = accounts_.find(accountId);
    return (it != accounts_.end()) ? it->second.get() : nullptr;
}

const Account* Bank::findAccount(const std::string& accountId) const {
    auto it = accounts_.find(accountId);
    return (it != accounts_.end()) ? it->second.get() : nullptr;
}

// *** Changed to std::unordered_map to match private member and header declaration ***
const std::unordered_map<std::string, std::unique_ptr<Account>>& Bank::getAllAccounts() const {
    return accounts_;
}

std::vector<Account*> Bank::getCustomerAccounts(const std::string& customerName) {
    std::vector<Account*> customerAccountsVec;
    Customer* customer = findCustomer(customerName);
    if (customer) {
        for (const std::string& id : customer->getAccountIds()) {
            Account* acc = findAccount(id);
            if (acc) customerAccountsVec.push_back(acc);
        }
    }
    return customerAccountsVec;
}

std::vector<const Account*> Bank::getCustomerAccounts(const std::string& customerName) const {
    std::vector<const Account*> customerAccountsVec;
    const Customer* customer = findCustomer(customerName);
    if (customer) {
        for (const std::string& id : customer->getAccountIds()) {
            const Account* acc = findAccount(id);
            if (acc) customerAccountsVec.push_back(acc);
        }
    }
    return customerAccountsVec;
}


// --- Transaction Operation Implementations ---
std::optional<Transaction> Bank::performDeposit(const std::string& accountId, double amount, const std::string& note) {
    Account* account = findAccount(accountId);
    if (!account) {
        std::cerr << "Error: Deposit failed, account " << accountId << " not found." << std::endl;
        return std::nullopt;
    }
    if (account->getType() != AccountType::CHECKING) {
        std::cerr << "Error: Deposit failed, account " << accountId << " is not a checking account." << std::endl;
        return std::nullopt;
    }
    if (amount <= 0) {
         std::cerr << "Error: Deposit amount must be positive." << std::endl;
        return std::nullopt;
    }

    double newBalance = account->getBalance() + amount;
    account->setBalance(newBalance);

    std::string txId = generateUniqueTransactionId();
    Transaction depositTx(txId, TransactionType::DEPOSIT, amount, "", accountId, note);
    recordTransaction(depositTx);

    std::cout << "Deposit successful to " << accountId << ". New balance: $" << std::fixed << std::setprecision(2) << newBalance << ". TX ID: " << txId << std::endl;
    return depositTx;
}

std::optional<Transaction> Bank::performWithdraw(const std::string& accountId, double amount, const std::string& note) {
    Account* account = findAccount(accountId);
    if (!account) {
        std::cerr << "Error: Withdrawal failed, account " << accountId << " not found." << std::endl;
        return std::nullopt;
    }
    if (account->getType() != AccountType::CHECKING) {
        std::cerr << "Error: Withdrawal failed, account " << accountId << " is not a checking account." << std::endl;
        return std::nullopt;
    }
    if (amount <= 0) {
         std::cerr << "Error: Withdrawal amount must be positive." << std::endl;
        return std::nullopt;
    }
    if (account->getBalance() < amount) {
        std::cerr << "Error: Withdrawal failed, account " << accountId << " insufficient balance." << std::endl;
        return std::nullopt;
    }

    double newBalance = account->getBalance() - amount;
    account->setBalance(newBalance);

    std::string txId = generateUniqueTransactionId();
    Transaction withdrawTx(txId, TransactionType::WITHDRAWAL, amount, accountId, "", note);
    recordTransaction(withdrawTx);

    std::cout << "Withdrawal successful from " << accountId << ". New balance: $" << std::fixed << std::setprecision(2) << newBalance << ". TX ID: " << txId << std::endl;
    return withdrawTx;
}

std::optional<Transaction> Bank::performTransfer(const std::string& sourceAccountId, const std::string& destinationAccountId, double amount, const std::string& note) {
    Account* sourceAccount = findAccount(sourceAccountId);
    Account* destinationAccount = findAccount(destinationAccountId);

    if (!sourceAccount) {
        std::cerr << "Error: Transfer failed, source account " << sourceAccountId << " not found." << std::endl;
        return std::nullopt;
    }
    if (!destinationAccount) {
        std::cerr << "Error: Transfer failed, destination account " << destinationAccountId << " not found." << std::endl;
        return std::nullopt;
    }
    if (amount <= 0) {
         std::cerr << "Error: Transfer amount must be positive." << std::endl;
        return std::nullopt;
    }
    if (sourceAccount->getBalance() < amount) {
        std::cerr << "Error: Transfer failed, source account " << sourceAccountId << " insufficient balance." << std::endl;
        return std::nullopt;
    }
    if (sourceAccountId == destinationAccountId) {
         std::cerr << "Error: Cannot transfer to the same account." << std::endl;
        return std::nullopt;
    }

    if (sourceAccount->getType() == AccountType::SAVINGS) {
        if (destinationAccount->getType() != AccountType::CHECKING || sourceAccount->getOwnerName() != destinationAccount->getOwnerName()) {
            std::cerr << "Error: Transfer failed. Savings account can only transfer to own Checking account." << std::endl;
            return std::nullopt;
        }
    }

    sourceAccount->setBalance(sourceAccount->getBalance() - amount);
    destinationAccount->setBalance(destinationAccount->getBalance() + amount);

    std::string txIdOut = generateUniqueTransactionId();
    Transaction transferOutTx(txIdOut, TransactionType::TRANSFER_OUT, amount, sourceAccountId, destinationAccountId, note);
    recordTransaction(transferOutTx);

    std::string txIdIn = generateUniqueTransactionId();
    Transaction transferInTx(txIdIn, TransactionType::TRANSFER_IN, amount, sourceAccountId, destinationAccountId, note);
    recordTransaction(transferInTx);

    std::cout << "Transfer successful from " << sourceAccountId << " to " << destinationAccountId << ". Amount: $" << amount << ". TX ID (Out): " << txIdOut << std::endl;
    return transferOutTx;
}


// --- Transaction Record and Reporting Implementations ---
void Bank::recordTransaction(const Transaction& transaction) {
    transactions_.push_back(transaction);
}

std::vector<Transaction> Bank::getAllTransactionsChronological() const {
    return transactions_;
}

std::vector<Transaction> Bank::getCustomerTransactionsChronological(const std::string& customerName) const {
    std::vector<Transaction> customerTxns;
    const Customer* customer = findCustomer(customerName);
    if (!customer) return customerTxns;

    const auto& custAccountIds = customer->getAccountIds();
    for (const auto& tx : transactions_) {
        bool isSource = !tx.getSourceAccountId().empty() &&
                        (std::find(custAccountIds.begin(), custAccountIds.end(), tx.getSourceAccountId()) != custAccountIds.end());
        bool isDest = !tx.getDestinationAccountId().empty() &&
                      (std::find(custAccountIds.begin(), custAccountIds.end(), tx.getDestinationAccountId()) != custAccountIds.end());
        if (isSource || isDest) {
            customerTxns.push_back(tx);
        }
    }
    return customerTxns;
}

std::vector<Transaction> Bank::getAccountTransactionsChronological(const std::string& accountId) const {
    std::vector<Transaction> accountTxns;
    if (!accountExists(accountId)) return accountTxns;

    for (const auto& tx : transactions_) {
        if (tx.getSourceAccountId() == accountId || tx.getDestinationAccountId() == accountId) {
            accountTxns.push_back(tx);
        }
    }
    return accountTxns;
}

// Helper for writing reports (assuming it's defined or moved to Utils)
bool writeReportToFile(const std::string& filename, const std::vector<Transaction>& transactions) {
     std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot open report file " << filename << std::endl;
        return false;
    }
    outFile << "Transaction Report - Generated: " << banking_system::utils::getCurrentDateString() << "\n";
    outFile << "--------------------------------------------------\n";
    if (transactions.empty()) {
        outFile << "No transaction records.\n";
    } else {
        for (const auto& tx : transactions) {
            outFile << tx.toString() << "\n";
        }
    }
    outFile << "--------------------------------------------------\n";
    outFile.close();
    std::cout << "Report successfully generated to file: " << filename << std::endl;
    return true;
}

bool Bank::generateGlobalReport(const std::string& filename) const {
    return writeReportToFile(filename, getAllTransactionsChronological());
}

bool Bank::generateCustomerReport(const std::string& customerName, const std::string& filename) const {
    const Customer* customer = findCustomer(customerName);
    if (!customer) {
        std::cerr << "Error: Customer " << customerName << " not found. Cannot generate report." << std::endl;
        return false;
    }
    return writeReportToFile(filename, getCustomerTransactionsChronological(customerName));
}

bool Bank::generateAccountReport(const std::string& accountId, const std::string& filename) const {
    if (!accountExists(accountId)) {
        std::cerr << "Error: Account " << accountId << " not found. Cannot generate report." << std::endl;
        return false;
    }
    return writeReportToFile(filename, getAccountTransactionsChronological(accountId));
}


// --- Internal Helper Method Implementations ---
std::string Bank::generateUniqueAccountId(AccountType type) {
    std::string prefix = "62";
    std::string typeCode = (type == AccountType::SAVINGS) ? "21" : "22";
    std::string accountId;
    int maxRetries = 100;
    int retries = 0;

    do {
        int randomBranch = branchDist_(randomEngine_); // Uses member randomEngine_
        long long randomAccNum = accountNumDist_(randomEngine_); // Uses member randomEngine_

        std::stringstream ssBranch;
        ssBranch << std::setw(4) << std::setfill('0') << randomBranch;
        std::string branchCodeStr = ssBranch.str();

        std::stringstream ssAccNum;
        // *** Ensure accountNumDist_ generates 8 digits for ZZZZAAAA ***
       
        ssAccNum << std::setw(8) << std::setfill('0') << randomAccNum;
        std::string accNumStr = ssAccNum.str();

        std::string accountNumPart1 = accNumStr.substr(0, 4);
        std::string accountNumPart2 = accNumStr.substr(4, 4);

        accountId = prefix + typeCode + "-" + branchCodeStr + "-" + accountNumPart1 + "-" + accountNumPart2;

        retries++;
        if (retries > maxRetries) {
            throw std::runtime_error("Failed to generate a unique account ID after multiple attempts.");
        }
    } while (accountExists(accountId));

    return accountId;
}

std::string Bank::generateUniqueTransactionId() {
    return "T" + std::to_string(nextTransactionId_++);
}

bool Bank::accountExists(const std::string& accountId) const {
    return accounts_.count(accountId) > 0;
}

bool Bank::customerExists(const std::string& name) const {
    return customerIndex_.count(name) > 0;
}

}
