# Online Banking System Using C++

A menu-driven, console-based banking simulation built with **C++17**. It is designed as a BCA academic mini-project and demonstrates object-oriented programming, inheritance, runtime and compile-time polymorphism, exception handling, dynamic memory allocation, file handling, templates, and the STL.

> **Academic-use notice:** This is a teaching project, not real banking software. Customer and administrator passwords are stored in plain text in the local data files. Do not use real personal information, real passwords, or real money.

## Features

### Customer
- Create a Savings or Current account; account numbers are generated automatically.
- Log in with an account number and password.
- View account details and available balance.
- Deposit, withdraw, and transfer funds.
- View transaction history, including date/time and balance after each transaction.
- Change a password and update contact details.
- Calculate the estimated one-year simple interest for a Savings Account.
- Submit a service request to the administrator.

### Administrator
- Log in separately from customers.
- View registered customers, search by account number, and inspect accounts.
- View all transaction records and total current account balances.
- View account statistics and process the next service request.

### Implementation notes
- Monetary values are stored as integer paise; floating-point values are not used for money calculations.
- Account types use an abstract base class, virtual functions, and overriding.
- Account objects are dynamically allocated with `new` and owned by `std::unique_ptr`, which releases them with `delete` automatically.
- A `map` indexes customers and accounts; a `vector` stores transactions; a `queue` stores service requests.
- Data is saved in text files under `data/` and loaded on the next run.
- Transfer operations create one debit-side and one credit-side transaction record.

## Project structure

```text
OnlineBankingSystem/
├── include/
│   ├── Account.h
│   ├── Admin.h
│   ├── Authentication.h
│   ├── Bank.h
│   ├── BankingException.h
│   ├── Customer.h
│   ├── Money.h
│   ├── TemplateUtils.h
│   └── Transaction.h
├── src/
│   ├── Account.cpp
│   ├── Admin.cpp
│   ├── Authentication.cpp
│   ├── Bank.cpp
│   ├── Customer.cpp
│   └── main.cpp
├── data/
│   ├── accounts.txt
│   ├── admin.txt
│   ├── customers.txt
│   ├── service_requests.txt
│   └── transactions.txt
├── docs/
│   ├── PROJECT_REPORT.md
│   └── DEMONSTRATION.md
├── CMakeLists.txt
├── Makefile
├── build.bat
└── README.md
```

## Requirements

- GCC or MinGW-w64 with C++17 support, or another C++17-compatible compiler.
- Windows, Linux, or macOS.
- No external libraries or database server is required.

## Build and run

**Important:** Run the executable with the project root as its working directory. The application uses the relative path `data/` for persistence.

### Windows (MinGW-w64)

Open Command Prompt or the VS Code terminal in the project folder:

```bat
build.bat
```

Or compile manually:

```bat
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src\main.cpp src\Account.cpp src\Authentication.cpp src\Admin.cpp src\Bank.cpp src\Customer.cpp -o OnlineBankingSystem.exe
OnlineBankingSystem.exe
```

### Linux / macOS

```bash
make
./OnlineBankingSystem
```

Or compile manually:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude \
  src/main.cpp src/Account.cpp src/Authentication.cpp src/Admin.cpp \
  src/Bank.cpp src/Customer.cpp -o OnlineBankingSystem
./OnlineBankingSystem
```

### CMake

```bash
cmake -S . -B build
cmake --build build --config Release
```

Run the generated executable from the project root so it can find `data/`.

## First login

The starter administrator credentials are:

| Field | Value |
|---|---|
| Username | `admin` |
| Password | `admin123` |

They are stored in `data/admin.txt` for demonstration. Change the values in that file before sharing the project. The program does not provide production-grade password hashing or access control.

## Quick demonstration

On a clean data folder, use these fictional details:

1. Select **Create New Account**. Register `Aarav Mehta`, age `25`, mobile `9876543210`, email `aarav@example.com`, password `pass1234`, Savings Account, opening deposit `1000.00`. The first account number is `100001`.
2. Create `Simran Kaur`, age `24`, mobile `9876501234`, email `simran@example.com`, password `pass5678`, Current Account, opening deposit `200.00`. The second account number is `100002`.
3. Log in as account `100001` with `pass1234`. Deposit `500.50`, withdraw `100.25`, then transfer `250.00` to account `100002`.
4. View transaction history. The sender balance should be `INR 1150.25`; the recipient balance should be `INR 450.00`.
5. Log out and log in as administrator using `admin` / `admin123` to inspect the customer, account, and transaction records.

The full demonstration sequence and test results are in [`docs/DEMONSTRATION.md`](docs/DEMONSTRATION.md). The project report is in [`docs/PROJECT_REPORT.md`](docs/PROJECT_REPORT.md).

## Data files

| File | Purpose |
|---|---|
| `customers.txt` | Customer details and academic-demo passwords |
| `accounts.txt` | Account type, balance in paise, and savings interest rate |
| `transactions.txt` | Transaction ID, account, type, amount, timestamp, status, resulting balance, and counterparty |
| `admin.txt` | Administrator username and password |
| `service_requests.txt` | Pending customer service requests |

The first four files named in the project brief are included. `service_requests.txt` is an additional file used to persist the STL queue between runs. The application creates missing data files at startup.

## Resetting the demonstration data

1. Close the application.
2. Keep `data/admin.txt` if you want to retain the administrator credentials.
3. Empty `customers.txt`, `accounts.txt`, `transactions.txt`, and `service_requests.txt`.
4. Start the application again.

Do not delete or edit the data files while the application is running.
