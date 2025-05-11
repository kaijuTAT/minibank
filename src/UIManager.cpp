#include "raylib.h"
#include "UIManager.hh"
#include "Bank.hh"
#include "Customer.hh"
#include "Account.hh"
#include "Transaction.hh"
#include "Utils.hh"

#include "raygui.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <stdexcept>
#include <algorithm> // For std::max

// Note: RAYGUI_IMPLEMENTATION is defined in main.cpp

namespace banking_system {

UIManager::UIManager(Bank& bank)
    : bank_(bank),
      currentState_(ScreenState::MAIN_MENU),
      screenWidth_(1400),
      screenHeight_(900),
      customerNameEditMode_(false),
      searchCustomerNameEditMode_(false),
      currentCustomer_(nullptr),
      currentAccount_(nullptr),
      amountEditMode_(false),
      destinationAccountEditMode_(false),
      noteEditMode_(false),
      transactionAmount_(0.0f),
      listViewScrollIndex_(0),
      listViewActive_(-1),
      messageReturnState_(ScreenState::MAIN_MENU) {
    clearInputBuffers();
    // std::cout << "UIManager initialized." << std::endl; // MY Debug
}

void UIManager::run() {
    // std::cout << "UIManager run() called." << std::endl; //THE Debug
    InitWindow(screenWidth_, screenHeight_, "Mini Banking System");
    if (!IsWindowReady()) {
        std::cerr << "Error: Failed to initialize Raylib window!" << std::endl;
        return;
    }
    SetTargetFPS(60);

    Font font = GetFontDefault();
    GuiSetFont(font);
    int baseFontSize = 20;
    GuiSetStyle(DEFAULT, TEXT_SIZE, baseFontSize);

    while (!WindowShouldClose() && !processInput()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentState_) {
            case ScreenState::MAIN_MENU:                drawMainMenu(); break;
            case ScreenState::REGISTER_CUSTOMER:        drawRegisterCustomer(); break;
            case ScreenState::ACCESS_CUSTOMER_SEARCH:   drawAccessCustomerSearch(); break;
            case ScreenState::CUSTOMER_VIEW:            drawCustomerView(); break;
            case ScreenState::ACCOUNT_VIEW_SAVINGS:     drawAccountViewSavings(); break;
            case ScreenState::ACCOUNT_VIEW_CHECKING:    drawAccountViewChecking(); break;
            case ScreenState::DEPOSIT_VIEW:             drawDepositView(); break;
            case ScreenState::WITHDRAW_VIEW:            drawWithdrawView(); break;
            case ScreenState::TRANSFER_VIEW:            drawTransferView(); break;
            case ScreenState::VIEW_ALL_ACCOUNTS:        drawViewAllAccounts(); break;
            case ScreenState::VIEW_CUSTOMER_TRANSACTIONS: drawTransactionHistoryWrapper(); break;
            case ScreenState::VIEW_ACCOUNT_TRANSACTIONS:  drawTransactionHistoryWrapper(); break;
            case ScreenState::SHOW_MESSAGE:             drawMessageBox(); break;
            default:
                DrawTextEx(font, "Unknown State", {10, 10}, (float)baseFontSize + 4, 1.0f, RED);
                 if (GuiButton((Rectangle){(float)screenWidth_/2 - 70, (float)screenHeight_ - 60, 140, 40}, "Main Menu")) {
                    changeState(ScreenState::MAIN_MENU);
                }
                break;
        }
        EndDrawing();
    }
    // std::cout << "UIManager closing window." << std::endl; //my Debug
    CloseWindow();
}

void UIManager::changeState(ScreenState newState) {
    // std::cout << "Changing state to: " << static_cast<int>(newState) << std::endl; //my Debug
    currentState_ = newState;
    clearInputBuffers();
    listViewScrollIndex_ = 0;
    listViewActive_ = -1;
     if (newState == ScreenState::MAIN_MENU) {
        currentCustomerName_ = "";
        currentCustomer_ = nullptr;
        currentAccountId_ = "";
        currentAccount_ = nullptr;
    } else if (newState == ScreenState::ACCESS_CUSTOMER_SEARCH) {
         currentCustomerName_ = "";
         currentCustomer_ = nullptr;
         currentAccountId_ = "";
         currentAccount_ = nullptr;
     } else if (newState == ScreenState::CUSTOMER_VIEW) {
         currentAccountId_ = "";
         currentAccount_ = nullptr;
     }
}

//message window
void UIManager::showMessage(const std::string& title, const std::string& text, ScreenState returnState) {
    messageTitle_ = title;
    messageText_ = text;
    messageReturnState_ = returnState;
    changeState(ScreenState::SHOW_MESSAGE);
}

//
void UIManager::clearInputBuffers() {
    customerNameInput_[0] = '\0';
    searchCustomerNameInput_[0] = '\0';
    amountInput_[0] = '\0';
    destinationAccountInput_[0] = '\0';
    noteInput_[0] = '\0';
    customerNameEditMode_ = false;
    searchCustomerNameEditMode_ = false;
    amountEditMode_ = false;
    destinationAccountEditMode_ = false;
    noteEditMode_ = false;
    transactionAmount_ = 0.0f;
}

    ///define the return button

bool UIManager::processInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (currentState_ == ScreenState::MAIN_MENU) {
            // std::cout << "ESC pressed on Main Menu. Exiting." << std::endl; // Debug
            return true; 
        } else {
            ScreenState targetState = ScreenState::MAIN_MENU;
            switch (currentState_) {
                case ScreenState::REGISTER_CUSTOMER:
                case ScreenState::ACCESS_CUSTOMER_SEARCH:
                case ScreenState::VIEW_ALL_ACCOUNTS:
                    targetState = ScreenState::MAIN_MENU;
                    break;
                case ScreenState::CUSTOMER_VIEW:
                    targetState = ScreenState::ACCESS_CUSTOMER_SEARCH;
                    break;
                case ScreenState::ACCOUNT_VIEW_SAVINGS:
                case ScreenState::ACCOUNT_VIEW_CHECKING:
                case ScreenState::VIEW_CUSTOMER_TRANSACTIONS:
                    targetState = ScreenState::CUSTOMER_VIEW;
                     if (!currentCustomer_) targetState = ScreenState::ACCESS_CUSTOMER_SEARCH;
                    break;
                case ScreenState::DEPOSIT_VIEW:
                case ScreenState::WITHDRAW_VIEW:
                case ScreenState::TRANSFER_VIEW:
                case ScreenState::VIEW_ACCOUNT_TRANSACTIONS:
                    if (currentAccount_) {
                        targetState = (currentAccount_->getType() == AccountType::SAVINGS) ?
                                      ScreenState::ACCOUNT_VIEW_SAVINGS : ScreenState::ACCOUNT_VIEW_CHECKING;
                    } else if (currentCustomer_) {
                        targetState = ScreenState::CUSTOMER_VIEW;
                    } else {
                         targetState = ScreenState::ACCESS_CUSTOMER_SEARCH;
                    }
                    break;
                case ScreenState::SHOW_MESSAGE:
                    return false; 
                default:
                     targetState = ScreenState::MAIN_MENU;
                     break;
            }
            changeState(targetState);
            return false; // Do not exit application
        }
    }
    return false; 
}

//main menue
void UIManager::drawMainMenu() {
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    DrawTextEx(currentFont, "Mini Banking System", {(float)screenWidth_/2 - MeasureTextEx(currentFont, "Mini Banking System", (float)baseFontSize + 14, textSpacing).x/2, 80}, (float)baseFontSize + 14, textSpacing, DARKGRAY);

    float buttonWidth = 350;
    float buttonHeight = 50;
    float spacing = 25;
    float startY = 250;
    float startX = (float)screenWidth_/2 - buttonWidth/2;

    if (GuiButton((Rectangle){startX, startY, buttonWidth, buttonHeight}, "1. Register New Customer")) {
        changeState(ScreenState::REGISTER_CUSTOMER);
    }
    if (GuiButton((Rectangle){startX, startY + buttonHeight + spacing, buttonWidth, buttonHeight}, "2. Access Customer Account")) {
        changeState(ScreenState::ACCESS_CUSTOMER_SEARCH);
    }
    if (GuiButton((Rectangle){startX, startY + 2*(buttonHeight + spacing), buttonWidth, buttonHeight}, "3. Display All Accounts")) {
        changeState(ScreenState::VIEW_ALL_ACCOUNTS);
    }
    if (GuiButton((Rectangle){startX, startY + 3*(buttonHeight + spacing), buttonWidth, buttonHeight}, "4. Generate Global Report")) {
        std::string filename = "transactions_" + utils::getCurrentDateString() + ".txt";
        if (bank_.generateGlobalReport(filename)) {
             showMessage("Report Generated", "Global transaction report saved as:\n" + filename, ScreenState::MAIN_MENU);
        } else {
             showMessage("Report Failed", "Failed to generate global transaction report.", ScreenState::MAIN_MENU);
        }
    }
}

//register new customer
void UIManager::drawRegisterCustomer() {
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    DrawTextEx(currentFont, "Register New Customer", {(float)screenWidth_/2 - MeasureTextEx(currentFont, "Register New Customer", (float)baseFontSize + 8, textSpacing).x/2, 120}, (float)baseFontSize + 8, textSpacing, DARKGRAY);

    const char* labelText = "Enter customer name:";
    Vector2 labelSize = MeasureTextEx(currentFont, labelText, (float)baseFontSize, textSpacing);
    float labelWidth = labelSize.x + 20;
    float inputWidth = 400;
    float inputHeight = 40;
    float totalWidth = labelWidth + 15 + inputWidth;
    float startX = (float)screenWidth_/2 - totalWidth/2;
    float startY = 250;

    GuiLabel((Rectangle){startX, startY + 9, labelWidth, inputHeight}, labelText);
    if (GuiTextBox((Rectangle){startX + labelWidth + 15, startY, inputWidth, inputHeight},
                   customerNameInput_, sizeof(customerNameInput_), customerNameEditMode_)) {
        customerNameEditMode_ = !customerNameEditMode_;
    }

    float buttonY = startY + inputHeight + 50;
    float buttonWidth = 160;
    float buttonHeight = 40;
    float buttonSpacing = 20;
    float returnButtonWidth = 220;
    float totalButtonWidth = buttonWidth + buttonSpacing + returnButtonWidth;
    float buttonStartX = (float)screenWidth_/2 - totalButtonWidth/2;

    if (GuiButton((Rectangle){buttonStartX, buttonY, buttonWidth, buttonHeight}, "Register")) {
        std::string name = customerNameInput_;
        if (!name.empty()) {
            Customer* newCustomer = bank_.registerCustomer(name);
            if (newCustomer) {
                 std::stringstream ss;
                 ss << "Customer [" << name << "] registered successfully!\n";
                 if (newCustomer->getAccountIds().size() >= 2) {
                    Account* savAcc = bank_.findAccount(newCustomer->getAccountIds()[0]);
                    Account* chkAcc = bank_.findAccount(newCustomer->getAccountIds()[1]);
                    if (savAcc) ss << "Savings Account: " << savAcc->getAccountId() << "\n";
                    if (chkAcc) ss << "Checking Account: " << chkAcc->getAccountId();
                 } else {
                     ss << "(Account ID information incomplete)";
                 }
                showMessage("Registration Successful", ss.str(), ScreenState::MAIN_MENU);
            } else {
                showMessage("Registration Failed", "Customer '" + name + "' might already exist.", ScreenState::REGISTER_CUSTOMER);
            }
        } else {
            showMessage("Input Error", "Customer name cannot be empty.", ScreenState::REGISTER_CUSTOMER);
        }
        clearInputBuffers();
    }
    if (GuiButton((Rectangle){buttonStartX + buttonWidth + buttonSpacing, buttonY, returnButtonWidth, buttonHeight}, "Return to Main Menu")) {
        changeState(ScreenState::MAIN_MENU);
    }
}

//input the customer number
void UIManager::drawAccessCustomerSearch() {
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    DrawTextEx(currentFont, "Access Customer Account", {(float)screenWidth_/2 - MeasureTextEx(currentFont, "Access Customer Account", (float)baseFontSize + 8, textSpacing).x/2, 120}, (float)baseFontSize + 8, textSpacing, DARKGRAY);

    const char* labelText = "Enter customer name:";
    Vector2 labelSize = MeasureTextEx(currentFont, labelText, (float)baseFontSize, textSpacing);
    float labelWidth = labelSize.x + 20;
    float inputWidth = 400;
    float inputHeight = 40;
    float totalWidth = labelWidth + 15 + inputWidth;
    float startX = (float)screenWidth_/2 - totalWidth/2;
    float startY = 250;

    GuiLabel((Rectangle){startX, startY + 9, labelWidth, inputHeight}, labelText);
    if (GuiTextBox((Rectangle){startX + labelWidth + 15, startY, inputWidth, inputHeight},
                   searchCustomerNameInput_, sizeof(searchCustomerNameInput_), searchCustomerNameEditMode_)) {
        searchCustomerNameEditMode_ = !searchCustomerNameEditMode_;
    }

    float buttonY = startY + inputHeight + 50;
    float buttonWidth = 160;
    float buttonHeight = 40;
    float buttonSpacing = 20;
    float returnButtonWidth = 220;
    float totalButtonWidth = buttonWidth + buttonSpacing + returnButtonWidth;
    float buttonStartX = (float)screenWidth_/2 - totalButtonWidth/2;

    if (GuiButton((Rectangle){buttonStartX, buttonY, buttonWidth, buttonHeight}, "Search")) {
        std::string name = searchCustomerNameInput_;
        if (!name.empty()) {
            currentCustomer_ = bank_.findCustomer(name);
            if (currentCustomer_) {
                currentCustomerName_ = name;
                changeState(ScreenState::CUSTOMER_VIEW);
            } else {
                showMessage("Search Failed", "Customer '" + name + "' not found.", ScreenState::ACCESS_CUSTOMER_SEARCH);
                currentCustomerName_ = "";
            }
        } else {
             showMessage("Input Error", "Customer name cannot be empty.", ScreenState::ACCESS_CUSTOMER_SEARCH);
             currentCustomerName_ = "";
        }
        clearInputBuffers();
    }
    if (GuiButton((Rectangle){buttonStartX + buttonWidth + buttonSpacing, buttonY, returnButtonWidth, buttonHeight}, "Return to Main Menu")) {
        changeState(ScreenState::MAIN_MENU);
    }
}

//customer view(after succed find the cutomer number)
void UIManager::drawCustomerView() {
     if (!currentCustomer_) {
        showMessage("Error", "Cannot display customer view. No customer selected.", ScreenState::ACCESS_CUSTOMER_SEARCH);
        return;
    }
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    std::string title = "Customer: " + currentCustomerName_;
    DrawTextEx(currentFont, title.c_str(), {(float)screenWidth_/2 - MeasureTextEx(currentFont, title.c_str(), (float)baseFontSize + 8, textSpacing).x/2, 60}, (float)baseFontSize + 8, textSpacing, DARKGRAY);
    DrawTextEx(currentFont, "This customer has the following accounts:", {80, 120}, (float)baseFontSize + 2, textSpacing, GRAY);

    std::vector<Account*> accounts = bank_.getCustomerAccounts(currentCustomerName_);
    float startY = 170;
    float lineHeight = (float)baseFontSize + 11;
    float itemHeight = 50;
    float accountInfoX = 90;
    float balanceX = 550;
    float buttonX = 850;
    float buttonWidth = 300;
    float buttonHeight = 40;

    for (size_t i = 0; i < accounts.size(); ++i) {
        Account* acc = accounts[i];
        if (!acc) continue;
        float currentY = startY + i * itemHeight;
        std::string accTypeStr = (acc->getType() == AccountType::SAVINGS) ? "Savings Account" : "Checking Account";
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << acc->getBalance();
        std::string balanceStr = "Balance: $" + ss.str();
        std::string accInfo = std::to_string(i + 1) + ". " + accTypeStr + ": " + acc->getAccountId();

        DrawTextEx(currentFont, accInfo.c_str(), {accountInfoX, currentY + (itemHeight - lineHeight)/2 + 5}, (float)baseFontSize + 2, textSpacing, BLACK);
        DrawTextEx(currentFont, balanceStr.c_str(), {balanceX, currentY + (itemHeight - lineHeight)/2 + 5}, (float)baseFontSize + 2, textSpacing, DARKGREEN);

        if (GuiButton((Rectangle){buttonX, currentY + (itemHeight - buttonHeight)/2, buttonWidth, buttonHeight}, "Access Account")) {
            currentAccount_ = acc;
            currentAccountId_ = acc->getAccountId();
            changeState(acc->getType() == AccountType::SAVINGS ? ScreenState::ACCOUNT_VIEW_SAVINGS : ScreenState::ACCOUNT_VIEW_CHECKING);
            return;
        }
    }

    float optionsY = startY + accounts.size() * itemHeight + 50;
    float optionsWidth = 520;
    float optionsX = (float)screenWidth_/2 - optionsWidth/2;
    float optionsSpacing = 20;
    buttonHeight = 45;

    if (GuiButton((Rectangle){optionsX, optionsY, optionsWidth, buttonHeight}, "3. View All Transactions for this Customer")) {
         changeState(ScreenState::VIEW_CUSTOMER_TRANSACTIONS);
    }
    if (GuiButton((Rectangle){optionsX, optionsY + buttonHeight + optionsSpacing, optionsWidth, buttonHeight}, "4. Generate Customer Report")) {
         std::string filename = "transactions_" + currentCustomerName_ + "_" + utils::getCurrentDateString() + ".txt";
         if (bank_.generateCustomerReport(currentCustomerName_, filename)) {
             showMessage("Report Generated", "Customer transaction report saved as:\n" + filename, ScreenState::CUSTOMER_VIEW);
         } else {
             showMessage("Report Failed", "Failed to generate customer transaction report.", ScreenState::CUSTOMER_VIEW);
         }
    }
    if (GuiButton((Rectangle){optionsX, optionsY + 2*(buttonHeight + optionsSpacing), optionsWidth, buttonHeight}, "5. Back to Customer Search")) {
         changeState(ScreenState::ACCESS_CUSTOMER_SEARCH);
    }
}

//showing the account when customers get into his/her/their accouts. and the options they made
void UIManager::drawAccountViewShared(const std::string& titlePrefix) {
     if (!currentAccount_) {
        ScreenState returnState = currentCustomer_ ? ScreenState::CUSTOMER_VIEW : ScreenState::ACCESS_CUSTOMER_SEARCH;
        showMessage("Error", "Cannot display account view. No account selected.", returnState);
        return;
    }
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    std::string title = titlePrefix + currentAccount_->getAccountId();
    DrawTextEx(currentFont, title.c_str(), {(float)screenWidth_/2 - MeasureTextEx(currentFont, title.c_str(), (float)baseFontSize + 8, textSpacing).x/2, 60}, (float)baseFontSize + 8, textSpacing, DARKGRAY);

    float infoY = 120;
    DrawTextEx(currentFont, ("Account Holder: " + currentAccount_->getOwnerName()).c_str(), {80, infoY}, (float)baseFontSize + 2, textSpacing, GRAY);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << currentAccount_->getBalance();
    std::string balanceStr = "Current Balance: $" + ss.str();
    DrawTextEx(currentFont, balanceStr.c_str(), {80, infoY + 35}, (float)baseFontSize + 6, textSpacing, DARKGREEN);
}


void UIManager::drawAccountViewSavings() {
    drawAccountViewShared("Savings Account: ");
     if (!currentAccount_) return;
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    float buttonWidth = 320;
    float buttonHeight = 45;
    float startY = 220;
    float startX = (float)screenWidth_/2 - buttonWidth/2;
    float spacing = 25;
    int buttonIndex = 0;

     if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "Transfer (to own Checking)")) {
        changeState(ScreenState::TRANSFER_VIEW);
    }
    buttonIndex++;
    if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "View Transaction History")) {
        changeState(ScreenState::VIEW_ACCOUNT_TRANSACTIONS);
    }
     buttonIndex++;
    if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "Generate Account Report")) {
         std::string filename = "transactions_" + currentAccountId_ + "_" + utils::getCurrentDateString() + ".txt";
         if (bank_.generateAccountReport(currentAccountId_, filename)) {
             showMessage("Report Generated", "Account transaction report saved as:\n" + filename, ScreenState::ACCOUNT_VIEW_SAVINGS);
         } else {
             showMessage("Report Failed", "Failed to generate account transaction report.", ScreenState::ACCOUNT_VIEW_SAVINGS);
         }
    }
     buttonIndex++;
     if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "Return to Customer Page")) {
         changeState(ScreenState::CUSTOMER_VIEW);
     }
}

//cheking the account status
void UIManager::drawAccountViewChecking() {
    drawAccountViewShared("Checking Account: ");
     if (!currentAccount_) return;
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    float buttonWidth = 280;
    float buttonHeight = 45;
    float startY = 220;
    float startX = (float)screenWidth_/2 - buttonWidth/2;
    float spacing = 20;
    int buttonIndex = 0;

    if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "1. Deposit")) {
        changeState(ScreenState::DEPOSIT_VIEW);
    }
    buttonIndex++;
    if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "2. Withdraw")) {
        changeState(ScreenState::WITHDRAW_VIEW);
    }
    buttonIndex++;
    if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "3. Transfer")) {
        changeState(ScreenState::TRANSFER_VIEW);
    }
    buttonIndex++;
    if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "4. View Transaction History")) {
        changeState(ScreenState::VIEW_ACCOUNT_TRANSACTIONS);
    }
     buttonIndex++;
    if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "5. Generate Account Report")) {
        std::string filename = "transactions_" + currentAccountId_ + "_" + utils::getCurrentDateString() + ".txt";
         if (bank_.generateAccountReport(currentAccountId_, filename)) {
             showMessage("Report Generated", "Account transaction report saved as:\n" + filename, ScreenState::ACCOUNT_VIEW_CHECKING);
         } else {
             showMessage("Report Failed", "Failed to generate account transaction report.", ScreenState::ACCOUNT_VIEW_CHECKING);
         }
    }
     buttonIndex++;
     if (GuiButton((Rectangle){startX, startY + buttonIndex*(buttonHeight + spacing), buttonWidth, buttonHeight}, "6. Return to Customer Page")) {
         changeState(ScreenState::CUSTOMER_VIEW);
     }
}

//showing the deposit status
void UIManager::drawDepositView() {
     if (!currentAccount_ || currentAccount_->getType() != AccountType::CHECKING) {
        showMessage("Error", "Cannot deposit. Invalid account state.", ScreenState::CUSTOMER_VIEW);
        return;
    }
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    std::string title = "Deposit to Account: " + currentAccountId_;
    DrawTextEx(currentFont, title.c_str(), {(float)screenWidth_/2 - MeasureTextEx(currentFont, title.c_str(), (float)baseFontSize + 8, textSpacing).x/2, 60}, (float)baseFontSize + 8, textSpacing, DARKGRAY);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << currentAccount_->getBalance();
    std::string balanceStr = "Current Balance: $" + ss.str();
    DrawTextEx(currentFont, balanceStr.c_str(), {80, 130}, (float)baseFontSize + 4, textSpacing, DARKGREEN);

    float inputWidth = 350;
    float inputHeight = 40;
    const char* label1Text = "Enter amount to deposit:";
    Vector2 label1Size = MeasureTextEx(currentFont, label1Text, (float)baseFontSize, textSpacing);
    float label1Width = label1Size.x + 20;
    const char* label2Text = "Add note (optional):";
    Vector2 label2Size = MeasureTextEx(currentFont, label2Text, (float)baseFontSize, textSpacing);
    float label2Width = label2Size.x + 20;
    float labelWidth = std::max(label1Width, label2Width);
    float startX = (float)screenWidth_/2 - (labelWidth + inputWidth + 20)/2;
    float startY = 220;
    float spacing = 20;

    GuiLabel((Rectangle){startX, startY + 9, labelWidth, inputHeight}, label1Text);
    if (GuiTextBox((Rectangle){startX + labelWidth + spacing, startY, inputWidth, inputHeight},
                   amountInput_, sizeof(amountInput_), amountEditMode_)) {
        amountEditMode_ = !amountEditMode_;
    }

    startY += inputHeight + spacing;
    GuiLabel((Rectangle){startX, startY + 9, labelWidth, inputHeight}, label2Text);
     if (GuiTextBox((Rectangle){startX + labelWidth + spacing, startY, inputWidth, inputHeight},
                   noteInput_, sizeof(noteInput_), noteEditMode_)) {
        noteEditMode_ = !noteEditMode_;
    }

    startY += inputHeight + 50;
    float buttonY = startY;
    float buttonWidth = 200;
    float buttonHeight = 45;
    float buttonSpacing = 20;
    float returnButtonWidth = 200;
    float totalButtonWidth = buttonWidth + buttonSpacing + returnButtonWidth;
    float buttonStartX = (float)screenWidth_/2 - totalButtonWidth/2;

     if (GuiButton((Rectangle){buttonStartX, buttonY, buttonWidth, buttonHeight}, "Confirm Deposit")) {
        try {
            transactionAmount_ = std::stof(amountInput_);
             if (transactionAmount_ <= 0) throw std::invalid_argument("Amount must be positive");
             std::string note = noteInput_;
             auto result = bank_.performDeposit(currentAccountId_, transactionAmount_, note);
             if (result) {
                 std::stringstream successMsg;
                 successMsg << "Deposit successful!\n";
                 Account* updatedAccount = bank_.findAccount(currentAccountId_);
                 if(updatedAccount) {
                    successMsg << "New balance: $" << std::fixed << std::setprecision(2) << updatedAccount->getBalance() << "\n";
                 } else {
                     successMsg << "(Could not fetch new balance)\n";
                 }
                 successMsg << "Transaction ID: " << result->getTransactionId();
                 showMessage("Deposit Successful", successMsg.str(), ScreenState::ACCOUNT_VIEW_CHECKING);
             } else {
                 showMessage("Deposit Failed", "Deposit operation failed. Please check input or contact support.", ScreenState::DEPOSIT_VIEW);
             }
        } catch (const std::invalid_argument& e) {
            showMessage("Input Error", "Invalid deposit amount. Please enter numbers.", ScreenState::DEPOSIT_VIEW);
        } catch (const std::out_of_range& e) {
             showMessage("Input Error", "Deposit amount is too large or too small.", ScreenState::DEPOSIT_VIEW);
        }
        clearInputBuffers();
    }
    if (GuiButton((Rectangle){buttonStartX + buttonWidth + buttonSpacing, buttonY, returnButtonWidth, buttonHeight}, "Return to Account")) {
        changeState(ScreenState::ACCOUNT_VIEW_CHECKING);
    }
}

//the option in withdraw and showing the satus

void UIManager::drawWithdrawView() {
     if (!currentAccount_ || currentAccount_->getType() != AccountType::CHECKING) {
        showMessage("Error", "Cannot withdraw. Invalid account state.", ScreenState::CUSTOMER_VIEW);
        return;
    }
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    std::string title = "Withdraw from Account: " + currentAccountId_;
    DrawTextEx(currentFont, title.c_str(), {(float)screenWidth_/2 - MeasureTextEx(currentFont, title.c_str(), (float)baseFontSize + 8, textSpacing).x/2, 60}, (float)baseFontSize + 8, textSpacing, DARKGRAY);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << currentAccount_->getBalance();
    std::string balanceStr = "Current Balance: $" + ss.str();
    DrawTextEx(currentFont, balanceStr.c_str(), {80, 130}, (float)baseFontSize + 4, textSpacing, DARKGREEN);

    float inputWidth = 350;
    float inputHeight = 40;
    const char* label1Text = "Enter amount to withdraw:";
    Vector2 label1Size = MeasureTextEx(currentFont, label1Text, (float)baseFontSize, textSpacing);
    float label1Width = label1Size.x + 20;
    const char* label2Text = "Add note (optional):";
    Vector2 label2Size = MeasureTextEx(currentFont, label2Text, (float)baseFontSize, textSpacing);
    float label2Width = label2Size.x + 20;
    float labelWidth = std::max(label1Width, label2Width);
    float startX = (float)screenWidth_/2 - (labelWidth + inputWidth + 20)/2;
    float startY = 220;
    float spacing = 20;

    GuiLabel((Rectangle){startX, startY + 9, labelWidth, inputHeight}, label1Text);
    if (GuiTextBox((Rectangle){startX + labelWidth + spacing, startY, inputWidth, inputHeight},
                   amountInput_, sizeof(amountInput_), amountEditMode_)) {
        amountEditMode_ = !amountEditMode_;
    }

    startY += inputHeight + spacing;
    GuiLabel((Rectangle){startX, startY + 9, labelWidth, inputHeight}, label2Text);
     if (GuiTextBox((Rectangle){startX + labelWidth + spacing, startY, inputWidth, inputHeight},
                   noteInput_, sizeof(noteInput_), noteEditMode_)) {
        noteEditMode_ = !noteEditMode_;
    }

    startY += inputHeight + 50;
    float buttonY = startY;
    float buttonWidth = 200;
    float buttonHeight = 45;
    float buttonSpacing = 20;
    float returnButtonWidth = 200;
    float totalButtonWidth = buttonWidth + buttonSpacing + returnButtonWidth;
    float buttonStartX = (float)screenWidth_/2 - totalButtonWidth/2;

     if (GuiButton((Rectangle){buttonStartX, buttonY, buttonWidth, buttonHeight}, "Confirm Withdrawal")) {
        try {
            transactionAmount_ = std::stof(amountInput_);
            if (transactionAmount_ <= 0) throw std::invalid_argument("Amount must be positive");
            std::string note = noteInput_;
            auto result = bank_.performWithdraw(currentAccountId_, transactionAmount_, note);
            if (result) {
                std::stringstream successMsg;
                successMsg << "Withdrawal successful!\n";
                 Account* updatedAccount = bank_.findAccount(currentAccountId_);
                 if(updatedAccount) {
                    successMsg << "New balance: $" << std::fixed << std::setprecision(2) << updatedAccount->getBalance() << "\n";
                 } else {
                     successMsg << "(Could not fetch new balance)\n";
                 }
                successMsg << "Transaction ID: " << result->getTransactionId();
                showMessage("Withdrawal Successful", successMsg.str(), ScreenState::ACCOUNT_VIEW_CHECKING);
            } else {
                showMessage("Withdrawal Failed", "Withdrawal failed. Please check amount or balance.", ScreenState::WITHDRAW_VIEW);
            }
        } catch (const std::invalid_argument& e) {
            showMessage("Input Error", "Invalid withdrawal amount. Please enter numbers.", ScreenState::WITHDRAW_VIEW);
        } catch (const std::out_of_range& e) {
             showMessage("Input Error", "Withdrawal amount is too large or too small.", ScreenState::WITHDRAW_VIEW);
        }
        clearInputBuffers();
    }
    if (GuiButton((Rectangle){buttonStartX + buttonWidth + buttonSpacing, buttonY, returnButtonWidth, buttonHeight}, "Return to Account")) {
        changeState(ScreenState::ACCOUNT_VIEW_CHECKING);
    }
}

//the transfer opition, and culculate
void UIManager::drawTransferView() {
    if (!currentAccount_) {
        showMessage("Error", "Cannot transfer. Invalid source account state.", ScreenState::CUSTOMER_VIEW);
        return;
    }
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    std::string title = "Transfer from Account: " + currentAccountId_;
    DrawTextEx(currentFont, title.c_str(), {(float)screenWidth_/2 - MeasureTextEx(currentFont, title.c_str(), (float)baseFontSize + 8, textSpacing).x/2, 60}, (float)baseFontSize + 8, textSpacing, DARKGRAY);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << currentAccount_->getBalance();
    std::string balanceStr = "Current Balance: $" + ss.str();
    DrawTextEx(currentFont, balanceStr.c_str(), {80, 130}, (float)baseFontSize + 4, textSpacing, DARKGREEN);

    float inputWidth = 450;
    float inputHeight = 40;
    const char* label1Text = "Enter destination account ID:";
    Vector2 label1Size = MeasureTextEx(currentFont, label1Text, (float)baseFontSize, textSpacing);
    float label1Width = label1Size.x + 20;
    const char* label2Text = "Enter amount to transfer:";
    Vector2 label2Size = MeasureTextEx(currentFont, label2Text, (float)baseFontSize, textSpacing);
    float label2Width = label2Size.x + 20;
     const char* label3Text = "Add note (optional):";
    Vector2 label3Size = MeasureTextEx(currentFont, label3Text, (float)baseFontSize, textSpacing);
    float label3Width = label3Size.x + 20;
    float labelWidth = std::max({label1Width, label2Width, label3Width});
    float startX = (float)screenWidth_/2 - (labelWidth + inputWidth + 20)/2;
    float startY = 220;
    float spacing = 20;

    GuiLabel((Rectangle){startX, startY + 9, labelWidth, inputHeight}, label1Text);
    if (GuiTextBox((Rectangle){startX + labelWidth + spacing, startY, inputWidth, inputHeight},
                   destinationAccountInput_, sizeof(destinationAccountInput_), destinationAccountEditMode_)) {
        destinationAccountEditMode_ = !destinationAccountEditMode_;
    }

    startY += inputHeight + spacing;
    GuiLabel((Rectangle){startX, startY + 9, labelWidth, inputHeight}, label2Text);
    if (GuiTextBox((Rectangle){startX + labelWidth + spacing, startY, inputWidth - 200, inputHeight},
                   amountInput_, sizeof(amountInput_), amountEditMode_)) {
        amountEditMode_ = !amountEditMode_;
    }

    startY += inputHeight + spacing;
    GuiLabel((Rectangle){startX, startY + 9, labelWidth, inputHeight}, label3Text);
     if (GuiTextBox((Rectangle){startX + labelWidth + spacing, startY, inputWidth, inputHeight},
                   noteInput_, sizeof(noteInput_), noteEditMode_)) {
        noteEditMode_ = !noteEditMode_;
    }

    startY += inputHeight + 50;
    float buttonY = startY;
    float buttonWidth = 200;
    float buttonHeight = 45;
    float buttonSpacing = 20;
    float returnButtonWidth = 200;
    float totalButtonWidth = buttonWidth + buttonSpacing + returnButtonWidth;
    float buttonStartX = (float)screenWidth_/2 - totalButtonWidth/2;

     if (GuiButton((Rectangle){buttonStartX, buttonY, buttonWidth, buttonHeight}, "Confirm Transfer")) {
        try {
            transactionAmount_ = std::stof(amountInput_);
            if (transactionAmount_ <= 0) throw std::invalid_argument("Amount must be positive");
            std::string destAccId = destinationAccountInput_;
            std::string note = noteInput_;
            if (destAccId.empty()) {
                 showMessage("Input Error", "Destination account ID cannot be empty.", ScreenState::TRANSFER_VIEW);
            } else {
                auto result = bank_.performTransfer(currentAccountId_, destAccId, transactionAmount_, note);
                if (result) {
                    std::stringstream successMsg;
                    successMsg << "Transfer successful!\n";
                     Account* updatedAccount = bank_.findAccount(currentAccountId_);
                     if(updatedAccount) {
                        successMsg << "Your new balance: $" << std::fixed << std::setprecision(2) << updatedAccount->getBalance() << "\n";
                     } else {
                         successMsg << "(Could not fetch new balance)\n";
                     }
                    successMsg << "Transaction ID: " << result->getTransactionId();
                    ScreenState returnState = (currentAccount_->getType() == AccountType::SAVINGS) ?
                                              ScreenState::ACCOUNT_VIEW_SAVINGS : ScreenState::ACCOUNT_VIEW_CHECKING;
                    showMessage("Transfer Successful", successMsg.str(), returnState);
                } else {
                    showMessage("Transfer Failed", "Transfer failed. Check input, balance, or transfer rules.", ScreenState::TRANSFER_VIEW);
                }
            }
        } catch (const std::invalid_argument& e) {
            showMessage("Input Error", "Invalid transfer amount. Please enter numbers.", ScreenState::TRANSFER_VIEW);
        } catch (const std::out_of_range& e) {
             showMessage("Input Error", "Transfer amount is too large or too small.", ScreenState::TRANSFER_VIEW);
        }
        clearInputBuffers();
    }
     ScreenState returnStateBtn = (currentAccount_->getType() == AccountType::SAVINGS) ?
                                ScreenState::ACCOUNT_VIEW_SAVINGS : ScreenState::ACCOUNT_VIEW_CHECKING;
    if (GuiButton((Rectangle){buttonStartX + buttonWidth + buttonSpacing, buttonY, returnButtonWidth, buttonHeight}, "Return to Account")) {
        changeState(returnStateBtn);
    }
}

//the whole status of the APP and showing the history
void UIManager::drawViewAllAccounts() {
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    DrawTextEx(currentFont, "All Bank Accounts", {(float)screenWidth_/2 - MeasureTextEx(currentFont, "All Bank Accounts", (float)baseFontSize + 8, textSpacing).x/2, 60}, (float)baseFontSize + 8, textSpacing, DARKGRAY);

    const auto& allAccountsMap = bank_.getAllAccounts();
    std::vector<const Account*> savingsAccounts;
    std::vector<const Account*> checkingAccounts;
    double totalBalance = 0.0;

    for (const auto& pair : allAccountsMap) {
        if (!pair.second) continue;
        if (pair.second->getType() == AccountType::SAVINGS) {
            savingsAccounts.push_back(pair.second.get());
        } else {
            checkingAccounts.push_back(pair.second.get());
        }
        totalBalance += pair.second->getBalance();
    }

    float startY = 120;
    float col1X = 80;
    float col2X = 550;
    float col3X = 900;
    float lineHeight = (float)baseFontSize + 8;
    float sectionSpacing = 40;
    float listFontSize = (float)baseFontSize;

    DrawTextEx(currentFont, "SAVINGS ACCOUNTS:", {col1X, startY}, (float)baseFontSize + 4, textSpacing, BLUE);
    startY += lineHeight * 1.5;
    if (savingsAccounts.empty()) {
        DrawTextEx(currentFont, "  (None)", {col1X, startY}, (float)baseFontSize, textSpacing, GRAY);
        startY += lineHeight;
    } else {
        for (size_t i = 0; i < savingsAccounts.size(); ++i) {
            const Account* acc = savingsAccounts[i];
            if (!acc) continue;
            std::string idStr = std::to_string(i + 1) + ". ID: " + acc->getAccountId();
            std::string ownerStr = "Owner: " + acc->getOwnerName();
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << acc->getBalance();
            std::string balanceStr = "Balance: $" + ss.str();

            DrawTextEx(currentFont, idStr.c_str(), {col1X, startY}, listFontSize, textSpacing, BLACK);
            DrawTextEx(currentFont, ownerStr.c_str(), {col2X, startY}, listFontSize, textSpacing, BLACK);
            DrawTextEx(currentFont, balanceStr.c_str(), {col3X, startY}, listFontSize, textSpacing, DARKGREEN);
            startY += lineHeight;
        }
    }

    startY += sectionSpacing;
    DrawTextEx(currentFont, "CHECKING ACCOUNTS:", {col1X, startY}, (float)baseFontSize + 4, textSpacing, DARKPURPLE);
    startY += lineHeight * 1.5;
     if (checkingAccounts.empty()) {
        DrawTextEx(currentFont, "  (None)", {col1X, startY}, (float)baseFontSize, textSpacing, GRAY);
        startY += lineHeight;
    } else {
        for (size_t i = 0; i < checkingAccounts.size(); ++i) {
            const Account* acc = checkingAccounts[i];
             if (!acc) continue;
            std::string idStr = std::to_string(i + 1) + ". ID: " + acc->getAccountId();
            std::string ownerStr = "Owner: " + acc->getOwnerName();
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << acc->getBalance();
            std::string balanceStr = "Balance: $" + ss.str();

            DrawTextEx(currentFont, idStr.c_str(), {col1X, startY}, listFontSize, textSpacing, BLACK);
            DrawTextEx(currentFont, ownerStr.c_str(), {col2X, startY}, listFontSize, textSpacing, BLACK);
            DrawTextEx(currentFont, balanceStr.c_str(), {col3X, startY}, listFontSize, textSpacing, DARKGREEN);
            startY += lineHeight;
        }
    }

    startY += sectionSpacing * 1.5;
    DrawLine(col1X, startY, screenWidth_ - col1X, startY, LIGHTGRAY);
    startY += 15;

    std::string totalAccountsStr = "Total Accounts: " + std::to_string(allAccountsMap.size());
    std::string totalCustomersStr = "Total Customers: " + std::to_string(bank_.getAllCustomers().size());
     std::stringstream ssTotal;
    ssTotal << std::fixed << std::setprecision(2) << totalBalance;
    std::string totalBalanceStr = "Total Balance: $" + ssTotal.str();

    DrawTextEx(currentFont, totalAccountsStr.c_str(), {col1X, startY}, (float)baseFontSize, textSpacing, DARKBLUE);
    DrawTextEx(currentFont, totalCustomersStr.c_str(), {col1X, startY + lineHeight}, (float)baseFontSize, textSpacing, DARKBLUE);
    DrawTextEx(currentFont, totalBalanceStr.c_str(), {col1X, startY + 2 * lineHeight}, (float)baseFontSize, textSpacing, DARKBLUE);

    if (GuiButton((Rectangle){(float)screenWidth_/2 - 80, (float)screenHeight_ - 80, 160, 45}, "Return to Main Menu")) {
        changeState(ScreenState::MAIN_MENU);
    }
}

//only showing the history of transaction
void UIManager::drawTransactionHistoryWrapper() {
    std::string title;
    std::vector<Transaction> transactions;
    ScreenState returnState = ScreenState::MAIN_MENU;

    if (currentState_ == ScreenState::VIEW_CUSTOMER_TRANSACTIONS && currentCustomer_) {
        title = "Transaction History for Customer [" + currentCustomerName_ + "]";
        transactions = bank_.getCustomerTransactionsChronological(currentCustomerName_);
        returnState = ScreenState::CUSTOMER_VIEW;
    } else if (currentState_ == ScreenState::VIEW_ACCOUNT_TRANSACTIONS && currentAccount_) {
        title = "Transaction History for Account [" + currentAccountId_ + "]";
        transactions = bank_.getAccountTransactionsChronological(currentAccountId_);
        returnState = (currentAccount_->getType() == AccountType::SAVINGS) ?
                      ScreenState::ACCOUNT_VIEW_SAVINGS : ScreenState::ACCOUNT_VIEW_CHECKING;
    } else {
         showMessage("Error", "Cannot display transaction history. Invalid state.", ScreenState::MAIN_MENU);
         return;
    }
    drawTransactionHistory(title, transactions, returnState);
}

//design of transaction history option
void UIManager::drawTransactionHistory(const std::string& title, const std::vector<Transaction>& transactions, ScreenState returnState) {
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;
    DrawTextEx(currentFont, title.c_str(), {(float)screenWidth_/2 - MeasureTextEx(currentFont, title.c_str(), (float)baseFontSize + 6, textSpacing).x/2, 60}, (float)baseFontSize + 6, textSpacing, DARKGRAY);

    Rectangle viewRec = { 40, 120, (float)screenWidth_ - 80, (float)screenHeight_ - 220 };
    int itemHeight = baseFontSize + 8;
    int visibleItems = (int)viewRec.height / itemHeight;
    int totalItems = transactions.size();
    float listFontSize = (float)baseFontSize;

    std::vector<std::string> transactionStrings;
    transactionStrings.reserve(transactions.size());
    for (const auto& tx : transactions) {
        transactionStrings.push_back(tx.toString());
    }

    DrawRectangleLinesEx(viewRec, 1, LIGHTGRAY);

    if (totalItems > 0) {
        BeginScissorMode((int)viewRec.x, (int)viewRec.y, (int)viewRec.width, (int)viewRec.height);
        for (int i = 0; i < totalItems; ++i) {
            if ((i >= listViewScrollIndex_) && (i < listViewScrollIndex_ + visibleItems)) {
                float itemPosY = viewRec.y + (i - listViewScrollIndex_) * itemHeight;
                Color textColor = BLACK;
                if (transactions[i].getType() == TransactionType::DEPOSIT || transactions[i].getType() == TransactionType::TRANSFER_IN) {
                    textColor = DARKGREEN;
                } else if (transactions[i].getType() == TransactionType::WITHDRAWAL || transactions[i].getType() == TransactionType::TRANSFER_OUT) {
                    textColor = MAROON;
                }
                 int maxChars = (int)(viewRec.width / (listFontSize * 0.5f));
                 DrawTextEx(currentFont, TextSubtext(transactionStrings[i].c_str(), 0, maxChars), {(float)viewRec.x + 15, itemPosY + 4}, listFontSize, textSpacing, textColor);
            }
        }
        EndScissorMode();
    } else {
        DrawTextEx(currentFont, "No transaction records found.", {(float)(viewRec.x + viewRec.width / 2 - MeasureTextEx(currentFont, "No transaction records found.", (float)baseFontSize, textSpacing).x/2), (float)(viewRec.y + viewRec.height / 2 - 8)}, (float)baseFontSize, textSpacing, GRAY);
    }


    float mouseWheelMove = GetMouseWheelMove();
    if (CheckCollisionPointRec(GetMousePosition(), viewRec)) {
        listViewScrollIndex_ -= (int)mouseWheelMove * 2;
        if (listViewScrollIndex_ < 0) listViewScrollIndex_ = 0;
        int maxScroll = totalItems - visibleItems;
        if (maxScroll < 0) maxScroll = 0;
        if (listViewScrollIndex_ > maxScroll) listViewScrollIndex_ = maxScroll;
    }

    if (GuiButton((Rectangle){(float)screenWidth_/2 - 80, (float)screenHeight_ - 80, 160, 45}, "Return")) {
        changeState(returnState);
    }
}

//message box and message windows
void UIManager::drawMessageBox() {
    DrawRectangle(0, 0, screenWidth_, screenHeight_, Fade(RAYWHITE, 0.8f));
    float msgBoxWidth = 550;
    float msgBoxHeight = 350;
    float msgBoxX = (float)screenWidth_/2 - msgBoxWidth/2;
    float msgBoxY = (float)screenHeight_/2 - msgBoxHeight/2;
    Font currentFont = GuiGetFont();
    int baseFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    float textSpacing = 1.0f;

    DrawRectangle((int)msgBoxX, (int)msgBoxY, (int)msgBoxWidth, (int)msgBoxHeight, LIGHTGRAY);
    DrawRectangleLinesEx((Rectangle){msgBoxX, msgBoxY, msgBoxWidth, msgBoxHeight}, 2, DARKGRAY);
    DrawTextEx(currentFont, messageTitle_.c_str(), {msgBoxX + msgBoxWidth/2 - MeasureTextEx(currentFont, messageTitle_.c_str(), (float)baseFontSize + 6, textSpacing).x/2, msgBoxY + 25}, (float)baseFontSize + 6, textSpacing, BLACK);

    std::string remainingText = messageText_;
    float currentTextY = msgBoxY + 80;
    int linesDrawn = 0;
    int maxLines = 6;
    size_t newlinePos;
    while (((newlinePos = remainingText.find('\n')) != std::string::npos) && (linesDrawn < maxLines)) {
        std::string line = remainingText.substr(0, newlinePos);
        DrawTextEx(currentFont, line.c_str(), {msgBoxX + 30, currentTextY}, (float)baseFontSize, textSpacing, BLACK);
        remainingText = remainingText.substr(newlinePos + 1);
        currentTextY += (float)baseFontSize + 8;
        linesDrawn++;
    }
    if (!remainingText.empty() && linesDrawn < maxLines) {
         DrawTextEx(currentFont, remainingText.c_str(), {msgBoxX + 30, currentTextY}, (float)baseFontSize, textSpacing, BLACK);
    }


    float buttonWidth = 120;
    float buttonHeight = 40;
    if (GuiButton((Rectangle){msgBoxX + msgBoxWidth/2 - buttonWidth/2, msgBoxY + msgBoxHeight - buttonHeight - 30, buttonWidth, buttonHeight}, "OK")) {
        changeState(messageReturnState_);
    }
}


} 
