# Demonstration and Test Record

All names, contact details, and credentials below are fictional. The values shown are for a clean first run, where the next account number is `100001`.

## Sample administrator login

```text
Administrator username: admin
Administrator password: admin123
Administrator login successful.
```

## Sample customer registration

```text
Customer name: Aarav Mehta
Age: 25
Mobile number: 9876543210
Email address: aarav@example.com
Create login password: pass1234
Choose account type: 1 (Savings Account)
Initial deposit: 1000.00

Account created successfully.
Your account number is: 100001
```

Register a second fictional customer:

```text
Customer name: Simran Kaur
Age: 24
Mobile number: 9876501234
Email address: simran@example.com
Create login password: pass5678
Choose account type: 2 (Current Account)
Initial deposit: 200.00

Account created successfully.
Your account number is: 100002
```

## Sample customer session

Log in using account number `100001` and password `pass1234`, then perform:

| Operation | Amount / account | Balance after operation |
|---|---:|---:|
| Opening deposit | INR 1,000.00 | INR 1,000.00 |
| Deposit | INR 500.50 | INR 1,500.50 |
| Withdrawal | INR 100.25 | INR 1,400.25 |
| Transfer to account 100002 | INR 250.00 | INR 1,150.25 |

The recipient's balance changes from INR 200.00 to INR 450.00. The transaction history records `TRANSFER_OUT` for account `100001` and `TRANSFER_IN` for account `100002`, with the other account number included in each record.

At the default annual savings rate of 2.50%, the estimated one-year simple interest on INR 1,150.25 is **INR 28.75**. Interest is displayed as an estimate and is not automatically added to the balance.

## Sample administrator observations

After the operations above, the administrator should see:

| Measure | Expected value |
|---|---:|
| Registered customers | 2 |
| Savings accounts | 1 |
| Current accounts | 1 |
| Account 100001 balance | INR 1,150.25 |
| Account 100002 balance | INR 450.00 |
| Total current account balances | INR 1,600.25 |

The total is the sum of current account balances, not the historical sum of every deposit transaction.

## Test cases

The following functional tests were run against the C++17 executable in this project:

| ID | Test | Expected result | Result |
|---|---|---|---|
| TC-01 | Start with empty customer/account/transaction files | Application starts and creates missing files | Pass |
| TC-02 | Register Savings and Current accounts | Unique account numbers are generated | Pass |
| TC-03 | Log in with valid customer credentials | Customer dashboard is displayed | Pass |
| TC-04 | Deposit INR 500.50 | Balance and transaction record are updated | Pass |
| TC-05 | Withdraw INR 100.25 within balance | Balance and transaction record are updated | Pass |
| TC-06 | Transfer INR 250.00 to another valid account | Both balances update; both transaction entries are saved | Pass |
| TC-07 | Withdraw INR 5,000.00 when balance is INR 1,150.25 | Insufficient-balance error; balance remains unchanged | Pass |
| TC-08 | View transaction history | IDs, type, amount, time, status, and resulting balance appear | Pass |
| TC-09 | Log in as administrator | Separate administrator dashboard is displayed | Pass |
| TC-10 | View all accounts and account statistics | Account types and current balances are listed | Pass |
| TC-11 | Submit and process a service request | Request is saved and removed from pending queue after processing | Pass |
| TC-12 | Exit and restart the application | Account balances and transaction history are loaded from files | Pass |

## Example persisted transfer records

The format is:

```text
transactionId accountNumber "type" amountPaise "dateTime" "status" balanceAfterPaise counterpartyAccount
```

Example values after the demonstration transfer (timestamps vary):

```text
5 100001 "TRANSFER_OUT" 25000 "2026-11-03 20:00:00" "SUCCESS" 115025 100002
6 100002 "TRANSFER_IN" 25000 "2026-11-03 20:00:00" "SUCCESS" 45000 100001
```

Amounts in the file are paise: `25000` represents INR 250.00.
