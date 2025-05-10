#include "Transaction.hh"
#include <sstream>   // For std::stringstream
#include <iomanip>   // For std::fixed, std::setprecision, std::put_time
#include <stdexcept> // For std::invalid_argument

namespace banking_system {

// Constructor implementation
Transaction::Transaction(const std::string& transactionId,
                         TransactionType type,
                         double amount,
                         const std::string& sourceAccountId,
                         const std::string& destinationAccountId,
                         const std::string& note)
    : transactionId_(transactionId),
      type_(type),
      amount_(amount),
      sourceAccountId_(sourceAccountId),
      destinationAccountId_(destinationAccountId),
      note_(note),
      timestamp_(std::chrono::system_clock::now()) { // Record current time as timestamp
    if (transactionId.empty()) {
        throw std::invalid_argument("Transaction ID cannot be empty.");
    }
    if (amount <= 0.0) {
        throw std::invalid_argument("Transaction amount must be positive.");
    }
    if (type == TransactionType::TRANSFER_OUT || type == TransactionType::TRANSFER_IN) {
        if (sourceAccountId.empty() || destinationAccountId.empty()) {
            throw std::invalid_argument("Transfer transactions must have source and destination account IDs.");
        }
    } else if (type == TransactionType::DEPOSIT && destinationAccountId.empty()) {
         throw std::invalid_argument("Deposit transactions must have a destination account ID.");
    } else if (type == TransactionType::WITHDRAWAL && sourceAccountId.empty()) {
         throw std::invalid_argument("Withdrawal transactions must have a source account ID.");
    }
}

// --- Getters ---
const std::string& Transaction::getTransactionId() const { return transactionId_; }
TransactionType Transaction::getType() const { return type_; }
double Transaction::getAmount() const { return amount_; }
const std::string& Transaction::getSourceAccountId() const { return sourceAccountId_; }
const std::string& Transaction::getDestinationAccountId() const { return destinationAccountId_; }
const std::string& Transaction::getNote() const { return note_; }

std::time_t Transaction::getTimestamp() const {
    return std::chrono::system_clock::to_time_t(timestamp_);
}

// Helper function to convert TransactionType to string
std::string transactionTypeToString(TransactionType type) {
    switch (type) {
        case TransactionType::DEPOSIT: return "Deposit";
        case TransactionType::WITHDRAWAL: return "Withdrawal";
        case TransactionType::TRANSFER_OUT: return "Transfer Out";
        case TransactionType::TRANSFER_IN: return "Transfer In";
        default: return "Unknown";
    }
}

// Helper function to format timestamp
std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm local_tm;
    #ifdef _WIN32
        localtime_s(&local_tm, &time); // Thread-safe on Windows
    #else
        localtime_r(&time, &local_tm); // Thread-safe on POSIX
    #endif
    std::stringstream ss;
    ss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// toString method to format transaction details
std::string Transaction::toString() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Time: " << formatTimestamp(timestamp_) << " | "
       << "ID: " << transactionId_ << " | "
       << "Type: " << transactionTypeToString(type_) << " | "
       << "Amount: $" << amount_;

    if (!sourceAccountId_.empty()) {
        ss << " | Source: " << sourceAccountId_;
    }
    if (!destinationAccountId_.empty()) {
        ss << " | Destination: " << destinationAccountId_;
    }
    if (!note_.empty()) {
        ss << " | Note: " << note_;
    }
    return ss.str();
}

} // namespace banking_system
