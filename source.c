#include "helper.h"

// --- Bank System ---

void init_bank(struct bank* b) {
    // init hash table
    for (int i = 0; i < SIZE; i++) {
        b->table[i] = NULL;
    }
    // init queue
    b->req_q.front = NULL;
    b->req_q.rear = NULL;
}

// helper to free stack
void free_stack(struct trans_stack* s) {
    struct trans_node* curr = s->top;
    while (curr != NULL) {
        struct trans_node* temp = curr;
        curr = curr->next;
        free(temp);
    }
}

void cleanup(struct bank* b) {
    for (int i = 0; i < SIZE; i++) {
        struct acc_node* curr = b->table[i];
        while (curr != NULL) {
            struct acc_node* temp = curr;
            curr = curr->next;
            
            free_stack(&(temp->acc->history)); // free history
            free(temp->acc); // free account
            free(temp); // free node
        }
    }
    // free queue
    struct req r;
    while(dequeue(&(b->req_q), &r));
}


// --- Hash Table & Account ---

unsigned long hash(long num) {
    return num % SIZE;
}

struct account* new_acc(long num, const char* name, double bal) {
    struct account* a = (struct account*)malloc(sizeof(struct account));
    if (a == NULL) {
        printf("mem alloc error\n");
        return NULL;
    }
    a->acc_num = num;
    strcpy(a->name, name);
    a->bal = bal;
    a->history.top = NULL;
    a->loan_amt = 0.0;
    a->emi = 0.0;
    
    if(bal > 0) {
        push_trans(&(a->history), "Deposit", bal);
    }
    return a;
}

int add_acc(struct bank* b, struct account* a) {
    if (a == NULL) return 0;
    
    if (find_acc(b, a->acc_num) != NULL) {
        printf("Error: acc num %ld exists.\n", a->acc_num);
        return 0; 
    }

    unsigned long i = hash(a->acc_num);
    
    struct acc_node* new_node = (struct acc_node*)malloc(sizeof(struct acc_node));
    if (new_node == NULL) {
        printf("mem alloc error\n");
        return 0;
    }
    
    new_node->acc = a;
    new_node->next = b->table[i];
    b->table[i] = new_node;
    
    return 1; // success
}

struct account* find_acc(struct bank* b, long num) {
    unsigned long i = hash(num);
    struct acc_node* curr = b->table[i];
    
    while (curr != NULL) {
        if (curr->acc->acc_num == num) {
            return curr->acc;
        }
        curr = curr->next;
    }
    return NULL; // not found
}

// --- Core Functions ---

void deposit(struct account* a, double amt) {
    if (amt <= 0) {
        printf("amt must be > 0\n");
        return;
    }
    a->bal += amt;
    push_trans(&(a->history), "Deposit", amt);
    printf("Deposited $%.2f. New bal: $%.2f\n", amt, a->bal);
}

int withdraw(struct account* a, double amt) {
    if (amt <= 0) {
        printf("amt must be > 0\n");
        return 0;
    }
    if (a->bal < amt) {
        printf("Error: Insufficient funds.\n");
        return 0;
    }
    a->bal -= amt;
    push_trans(&(a->history), "Withdrawal", amt);
    printf("Withdrew $%.2f. New bal: $%.2f\n", amt, a->bal);
    return 1;
}

int transfer(struct bank* b, long from_n, long to_n, double amt) {
    struct account* a_from = find_acc(b, from_n);
    struct account* a_to = find_acc(b, to_n);

    if (a_from == NULL) {
        printf("Error: Source acc %ld not found.\n", from_n);
        return 0;
    }
    if (a_to == NULL) {
        printf("Error: Dest acc %ld not found.\n", to_n);
        return 0;
    }
    if (a_from->bal < amt) {
        printf("Error: Insufficient funds.\n");
        return 0;
    }

    // do transfer
    a_from->bal -= amt;
    a_to->bal += amt;

    // log it
    char log[100];
    sprintf(log, "Transfer from %ld", from_n);
    push_trans(&(a_to->history), log, amt);

    sprintf(log, "Transfer to %ld", to_n);
    push_trans(&(a_from->history), log, -amt); 

    printf("Success! Transferred $%.2f.\n", amt);
    return 1;
}

void show_balance(struct account* a) {
    printf("\n--- Details for %ld ---\n", a->acc_num);
    printf("Name:    %s\n", a->name);
    printf("Balance: $%.2f\n", a->bal);
    if(a->loan_amt > 0) {
        printf("Loan:    $%.2f (EMI: $%.2f)\n", a->loan_amt, a->emi);
    }
    printf("-------------------------\n");
}

void calc_interest(struct account* a, double r, int t) {
    // simple interest
    double si = (a->bal * r * t) / 100.0;
    printf("Interest: $%.2f\n", si);
    deposit(a, si);
    
    // fix log
    free(a->history.top); 
    push_trans(&(a->history), "Interest", si);
}


// --- Transaction History (Stack) ---

void push_trans(struct trans_stack* s, const char* type, double amt) {
    struct trans_node* new_node = (struct trans_node*)malloc(sizeof(struct trans_node));
    if (new_node == NULL) {
        printf("Error: failed to log trans\n");
        return;
    }
    strcpy(new_node->t.type, type);
    new_node->t.amt = amt;
    new_node->t.ts = time(NULL);
    
    new_node->next = s->top;
    s->top = new_node;
}

void print_history(struct account* a) {
    printf("\n--- History for %ld ---\n", a->acc_num);
    struct trans_node* curr = a->history.top;
    if (curr == NULL) {
        printf("No transactions.\n");
    }
    while (curr != NULL) {
        char* time_str = ctime(&(curr->t.ts));
        time_str[strlen(time_str) - 1] = '\0'; // remove \n
        
        printf("[%s] %-20s $%.2f\n",
            time_str,
            curr->t.type,
            curr->t.amt
        );
        curr = curr->next;
    }
    printf("--------------------------------------\n");
    printf("Current Balance: $%.2f\n", a->bal);
}

// --- Loan & Service Queue ---

void apply_loan(struct bank* b, long num, double amt) {
    if (find_acc(b, num) == NULL) {
        printf("Error: Acc %ld not exist.\n", num);
        return;
    }
    struct req r;
    strcpy(r.type, "Loan");
    r.acc_num = num;
    r.amt = amt;
    
    enqueue(&(b->req_q), r);
    printf("Loan app submitted for $%.2f.\n", amt);
}

void enqueue(struct queue* q, struct req r) {
    struct q_node* new_node = (struct q_node*)malloc(sizeof(struct q_node));
    if(new_node == NULL) {
        printf("Error\n");
        return;
    }
    new_node->r = r;
    new_node->next = NULL;

    if (q->rear == NULL) { // empty
        q->front = new_node;
        q->rear = new_node;
        return;
    }
    
    q->rear->next = new_node;
    q->rear = new_node;
}

int dequeue(struct queue* q, struct req* r) {
    if (q->front == NULL) {
        return 0; // empty
    }
    
    struct q_node* temp = q->front;
    *r = temp->r;
    
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    return 1; // success
}

void process_req(struct bank* b) {
    struct req r;
    if (dequeue(&(b->req_q), &r) == 0) {
        printf("No pending requests.\n");
        return;
    }

    printf("Processing req for %ld...\n", r.acc_num);
    struct account* a = find_acc(b, r.acc_num);
    if (a == NULL) {
        printf("Error: Acc %ld not found.\n", r.acc_num);
        return;
    }

    if (strcmp(r.type, "Loan") == 0) {
        // simple approval
        if (a->bal < r.amt * 0.1) {
             printf("Loan REJECTED for $%.2f (low bal).\n", r.amt);
        } else {
            printf("Loan APPROVED for $%.2f.\n", r.amt);
            a->loan_amt += r.amt;
            a->emi = (r.amt * 1.10) / 12.0; // 10% interest / 12 months
            deposit(a, r.amt);
            // fix log
            free(a->history.top); 
            push_trans(&(a->history), "Loan Disbursed", r.amt);
        }
    }
}

void show_loan(struct account* a) {
     if (a->loan_amt > 0) {
        printf("Outstanding Loan: $%.2f\n", a->loan_amt);
        printf("Next EMI: $%.2f\n", a->emi);
     } else {
        printf("No active loans.\n");
     }
}