#pragma once 

#include <string>
#include <chrono> 
#include <ctime>  

namespace banking_system {

enum class TransactionType {
    DEPOSIT,
    WITHDRAWAL,
    TRANSFER_OUT, // From the perspective of the source account
    TRANSFER_IN   // From the perspective of the destination account
};

// File: Transaction.hh
// Purpose: Defines the Transaction class, which represents a single financial transaction.
// It stores details like ID, type, amount, involved accounts, timestamp, and an optional note.
class Transaction {
public:
    // Constructor: Initializes a Transaction object.
    Transaction(const std::string& transactionId,
                TransactionType type,
                double amount,
                const std::string& sourceAccountId,      // Can be empty for deposits
                const std::string& destinationAccountId, // Can be empty for withdrawals
                const std::string& note = "");           // Optional note for the transaction

    // --- Getters for transaction details ---
    const std::string& getTransactionId() const;
    TransactionType getType() const;
    double getAmount() const;
    const std::string& getSourceAccountId() const;
    const std::string& getDestinationAccountId() const;
    const std::string& getNote() const;
    std::time_t getTimestamp() const; // Returns a std::time_t timestamp

    // Formats the transaction details into a human-readable string.
    std::string toString() const;

private:
    std::string transactionId_;
    TransactionType type_;
    double amount_;
    std::string sourceAccountId_;
    std::string destinationAccountId_;
    std::string note_;
    std::chrono::system_clock::time_point timestamp_; // High-resolution timestamp
};

// Helper function to convert TransactionType enum to a string.
std::string transactionTypeToString(TransactionType type);

// Helper function to format a std::chrono::system_clock::time_point to a readable string.
std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);

} // namespace banking_system
