#include "helper.h"

int main() {
    struct bank b; // b for bank
    init_bank(&b);
    int ch; // ch for choice

    printf("Welcome to Bank System\n");

    while (1) {
        printf("\n--- Menu ---\n");
        printf("1. New Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Transfer\n");
        printf("5. Mini-Statement\n");
        printf("6. Check Balance\n");
        printf("7. Apply for Loan\n");
        printf("8. Calc Interest\n");
        printf("9. Process Request (Admin)\n");
        printf("10. Exit\n");
        printf("------------\n");
        printf("Enter choice: ");
        
        if(scanf("%d", &ch) != 1) {
            while(getchar() != '\n'); // clear buffer
            printf("Invalid. Enter num.\n");
            continue;
        }

        switch (ch) {
            case 1: { 
                char name[100];
                long num;
                double amt;
                printf("Enter acc num: ");
                scanf("%ld", &num);
                printf("Enter name: ");
                scanf(" %[^\n]", name);
                printf("Enter deposit: ");
                scanf("%lf", &amt);
                
                struct account* a = new_acc(num, name, amt);
                if (add_acc(&b, a)) {
                    printf("Account created: %ld\n", num);
                } else {
                    free(a); 
                }
                break;
            }
            case 2: { // Deposit
                long num;
                printf("Enter acc num: ");
                scanf("%ld", &num);
                struct account* a = find_acc(&b, num);

                if (a != NULL) {
                    double amt;
                    printf("Enter amount: ");
                    scanf("%lf", &amt);
                    deposit(a, amt);
                } else {
                    printf("acc not found\n");
                }
                break;
            }
            case 3: { // Withdraw
                long num;
                printf("Enter acc num: ");
                scanf("%ld", &num);
                struct account* a = find_acc(&b, num);
                
                if (a != NULL) {
                    double amt;
                    printf("Enter amount: ");
                    scanf("%lf", &amt);
                    withdraw(a, amt);
                } else {
                    printf("acc not found\n");
                }
                break;
            }
            case 4: { // Transfer
                long from_n, to_n;
                double amt;
                printf("Enter YOUR acc num: ");
                scanf("%ld", &from_n);
                printf("Enter dest acc num: ");
                scanf("%ld", &to_n);
                printf("Enter amount: ");
                scanf("%lf", &amt);
                transfer(&b, from_n, to_n, amt);
                break;
            }
            case 5: { // Mini-Statement
                long num;
                printf("Enter acc num: ");
                scanf("%ld", &num);
                struct account* a = find_acc(&b, num);
                
                if (a != NULL) {
                    print_history(a);
                } else {
                    printf("acc not found\n");
                }
                break;
            }
            case 6: { // Check Balance
                long num;
                printf("Enter acc num: ");
                scanf("%ld", &num);
                struct account* a = find_acc(&b, num);
                
                if (a != NULL) {
                    show_balance(a);
                } else {
                    printf("acc not found\n");
                }
                break;
            }
            case 7: { // Apply for Loan
                long num;
                double amt;
                printf("Enter acc num: ");
                scanf("%ld", &num);
                printf("Enter loan amount: ");
                scanf("%lf", &amt);
                apply_loan(&b, num, amt);
                break;
            }
            case 8: { // Calculate Interest
                long num;
                printf("Enter acc num: ");
                scanf("%ld", &num);
                struct account* a = find_acc(&b, num);

                if (a != NULL) {
                    double r;
                    int t;
                    printf("Enter rate (e.g., 5): ");
                    scanf("%lf", &r);
                    printf("Enter time (years): ");
                    scanf("%d", &t);
                    calc_interest(a, r, t);
                } else {
                    printf("acc not found\n");
                }
                break;
            }
            case 9: { // Process Request
                process_req(&b);
                break;
            }
            case 10: { // Exit
                printf("Bye.\n");
                cleanup(&b); // free memory
                return 0;
            }
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0; 
}