# DSA Miniproject — Bank Management System

A small command-line bank system implemented in C. The project demonstrates basic data structures and operations: a hash-table-backed account store, transaction history stack, a service request queue, and simple loan processing.

## Files

- `helper.h` — Project data structures and function prototypes (accounts, transactions, queue, etc.).
- `main.c` — Program entry point and user interaction (CLI). 
- `source.c` — Core implementation: bank initialization, account management, deposits/withdrawals/transfers, transaction history stack, loan requests and processing, queue operations.

## Features

- Create and store accounts in a hash table (by account number).
- Deposit and withdraw money; operations are recorded in a per-account transaction history (stack).
- Transfer funds between accounts with logging.
- Request and process loans via a FIFO queue with a very simple approval rule.
- Calculate simple interest and append it to account history.
- Basic cleanup utilities to free allocated memory.

## Build (macOS, zsh)

From the project root compile with GCC:

```sh
gcc main.c source.c -o bank
```

If `helper.h` declares any additional modules you need to compile, add them to the gcc command above.

## Run

After building, run the CLI binary:

```sh
./bank
```

The program is interactive (see `main.c`) — use the menu options there to create accounts, deposit, withdraw, transfer, view history, apply for loans, and process requests.

## Example usage

- Create two accounts with distinct numbers.
- Deposit to account A, withdraw from account B.
- Transfer from A to B and then use history functions to verify logs.
- Apply for a loan for an account and run the request processor to approve/reject based on the account balance.

## Notes & suggestions

- The implementation stores transaction timestamps using `time()` and prints them with `ctime`.
- The loan approval logic is intentionally simple (checks a small proportion of balance). Consider adding a safer and more realistic approval workflow.
- Add unit tests for core functions (find_acc, add_acc, transfer, queue operations). Consider using a small test harness or CUnit.
- Consider adding input validation and better error handling in `main.c` (currently basic messages are printed).

## Next steps

- Add a small test suite and CI (e.g., GitHub Actions) to compile and run tests automatically.
- Add more robust persistence (save/load accounts to/from a file).

---
