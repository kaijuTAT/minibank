# Mini Banking System

My ID:2023905417

My Chinese name: 林俊锴

My English name: Elvis

---

## 1. Project Goal

This project simulates a banking system with a graphical user interface (GUI) built in C++. It features:

- Multiple account types (Savings and Checking)

- Customer management

- Financial transactions (deposit, withdrawal, transfer)

- Transaction reporting functionality

The goal is to strengthen understanding and application of object-oriented programming (OOP) principles in C++, including classes, inheritance, abstraction, encapsulation, multi-file organization, and namespace management.


---

## 2. Features Implemented

### Customer Management

- Register new customers.

- Automatically create one Savings and one Checking account for each customer.

- Search and access customer accounts by name.

### Account Management

- Each account receives a unique bank account number in the format `62XX-YYYY-ZZZZ-AAAA`:
  
  - `62`: Fixed prefix
  
  - `XX`: `21` for Savings, `22` for Checking
  
  - `YYYY`: Random 4-digit branch code
  
  - `ZZZZ-AAAA`: Random 8-digit account number

- Display account details: ID, type, owner, balance.

- Display all accounts in the system with summaries (total accounts, total customers, total balance).

### Transaction Operations

- **Deposit**: Allowed only for Checking accounts.

- **Withdrawal**: Allowed only for Checking accounts, with balance validation.

- **Transfer**:
  
  - Savings accounts can transfer only to the same customer's Checking account.
  
  - Checking accounts can transfer to any valid account (Savings or Checking).

- All operations generate and store a `Transaction` record.

### Transaction Reporting

- **Global Report**: Lists all transactions system-wide, ordered by time. Saved as `transactions_YYYY-MM-DD.txt`.

- **Customer Report**: Lists all transactions related to a specific customer. Saved as `transactions_<CUSTOMER_NAME>_YYYY-MM-DD.txt`.

- **Account Report**: Lists all transactions of a specific account. Saved as `transactions_<ACCOUNT_ID>_YYYY-MM-DD.txt`.

### User Interface

- GUI built using Raylib and raygui.

- Menu-driven interface to handle all system operations.

---

## 3. Architecture and Design

### Core Logic Layer

- `Bank` class: Main orchestrator that manages customers, accounts, and transactions.

- `Account` abstract class: Base class defining common attributes (ID, owner, balance) and virtual functions.

- `SavingsAccount` / `CheckingAccount`: Inherit from `Account`, implement specific behaviors and constraints.

- `Customer` class: Represents a customer and maintains a list of their account IDs.

- `Transaction` class: Represents a single financial transaction, storing all relevant details.

- `Utils` namespace: Contains helper functions, e.g., for date formatting.

### User Interface Layer

- `UIManager` class: Handles UI rendering and interaction using Raylib, delegates operations to the `Bank` object.

### Data Management

- Customers and accounts managed with `std::vector<std::unique_ptr<Customer>>` and `std::map<std::string, std::unique_ptr<Account>>`.

- Transactions stored in `std::vector<Transaction>`.

### Key Design Principles

- **Encapsulation**: Most data members are private, accessed through public methods.

- **Inheritance & Polymorphism**: `Account` is an abstract base class with polymorphic methods.

- **Abstraction**: The `Account` class defines an interface to be implemented by derived classes.

- **Separation of Concerns**: UI logic is separated from business logic.

### Project Structure

- Each class is defined in its own `.hh` and `.cpp` file.

- All project-specific code is under the `banking_system` namespace.

- Header files use `#pragma once` for protection.

---

## 4. Class Descriptions

- `Account`: Abstract base class, defines account ID, owner, balance, and a pure virtual `getType()` method.

- `SavingsAccount`: Inherits from `Account`, restricts transfer behavior.

- `CheckingAccount`: Inherits from `Account`, allows deposits, withdrawals, and transfers.

- `Transaction`: Stores transaction ID, type, amount, source and destination accounts, timestamp, and notes.

- `Customer`: Represents a customer with a name and a list of their account IDs.

- `Bank`: Main logic controller that manages all customers, accounts, and transactions.

- `UIManager`: Manages GUI interaction and communicates with the `Bank` object.

- `Utils`: Provides utility functions.

---

## 5. Compilation and Running Instructions

### Prerequisites

- C++17 compatible compiler (e.g., GCC, Clang, MSVC)

- CMake >= 3.12

- Git (optional, for cloning Raylib submodule)

### Compilation Steps

1. **Clone Repository** (if applicable):

```bash
git clone [your-gitee-repo-url]
cd MiniBankingSystem
```

2. **Initialize Submodules (Raylib)**

```bash
git submodule init
git submodule update --recursive
```

(Skip this step if Raylib source is already placed under `external/raylib`.)

3. **Create Build Directory**

```bash
mkdir build
cd build
```

4. **Configure with CMake**
- For MinGW Makefiles:

```bash
cmake .. -G "MinGW Makefiles"
```

- For Visual Studio:

```bash
cmake .. -G "Visual Studio 17 2022"
```

5. **Build the Project**

```bash
cmake --build .
```

or

```bash
mingw32-make
```

### Running the Application

- On Windows: `./MiniBankingApp.exe`

- On Linux/macOS: `./MiniBankingApp`

> Note: This version uses Raylib's default font. No external font files are needed for basic English rendering.

---
