#pragma once // Header guard

#include "raylib.h"
#include <memory>
#include <string>
#include <vector>

namespace banking_system {
    class Bank;
    class Customer;
    class Account;
    class Transaction;
}

namespace banking_system {

enum class ScreenState { /* ... (states remain the same) ... */
    MAIN_MENU,
    REGISTER_CUSTOMER,
    ACCESS_CUSTOMER_SEARCH,
    CUSTOMER_VIEW,
    ACCOUNT_VIEW_SAVINGS,
    ACCOUNT_VIEW_CHECKING,
    DEPOSIT_VIEW,
    WITHDRAW_VIEW,
    TRANSFER_VIEW,
    VIEW_ALL_ACCOUNTS,
    VIEW_CUSTOMER_TRANSACTIONS,
    VIEW_ACCOUNT_TRANSACTIONS,
    SHOW_MESSAGE
};

// File: UIManager.hh
// Purpose: Defines the UIManager class, responsible for the GUI.
class UIManager {
public:
    explicit UIManager(Bank& bank);

    // --- Canonical Form ---
    ~UIManager() = default; // Default destructor is fine

    // UIManager is non-copyable due to reference member (bank_) and typically shouldn't be copied.
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    // UIManager can be movable if needed, though not typical for a manager class like this.
    // Defaulting them is usually safe if no raw resources are managed directly.
    UIManager(UIManager&&) = default;
    UIManager& operator=(UIManager&&) = default;
    // ----------------------

    void run();

private:
    Bank& bank_;
    ScreenState currentState_ = ScreenState::MAIN_MENU;
    int screenWidth_ = 1400;
    int screenHeight_ = 900;

    char customerNameInput_[64] = {0};
    bool customerNameEditMode_ = false;
    char searchCustomerNameInput_[64] = {0};
    bool searchCustomerNameEditMode_ = false;
    std::string currentCustomerName_ = "";
    Customer* currentCustomer_ = nullptr;
    std::string currentAccountId_ = "";
    Account* currentAccount_ = nullptr;
    char amountInput_[32] = {0};
    bool amountEditMode_ = false;
    char destinationAccountInput_[64] = {0};
    bool destinationAccountEditMode_ = false;
    char noteInput_[128] = {0};
    bool noteEditMode_ = false;
    float transactionAmount_ = 0.0f;
    int listViewScrollIndex_ = 0;
    int listViewActive_ = -1;
    std::string messageTitle_ = "";
    std::string messageText_ = "";
    ScreenState messageReturnState_ = ScreenState::MAIN_MENU;

    void drawMainMenu();
    void drawRegisterCustomer();
    void drawAccessCustomerSearch();
    void drawCustomerView();
    void drawAccountViewShared(const std::string& titlePrefix);
    void drawAccountViewSavings();
    void drawAccountViewChecking();
    void drawDepositView();
    void drawWithdrawView();
    void drawTransferView();
    void drawViewAllAccounts();
    void drawTransactionHistory(const std::string& title, const std::vector<Transaction>& transactions, ScreenState returnState);
    void drawTransactionHistoryWrapper();
    void drawMessageBox();

    bool processInput();
    void changeState(ScreenState newState);
    void showMessage(const std::string& title, const std::string& text, ScreenState returnState);
    void clearInputBuffers();
};

} // namespace banking_system
