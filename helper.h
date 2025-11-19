#ifndef BANK_H
#define BANK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 100

// --- Data Structures ---

// for transaction history
struct trans {
    char type[50];
    double amt;
    time_t ts;
};

struct trans_node {
    struct trans t;
    struct trans_node* next;
};

struct trans_stack {
    struct trans_node* top;
};

// for account details
struct account {
    long acc_num;
    char name[100];
    double bal;
    struct trans_stack history;
    // loan stuff
    double loan_amt;
    double emi;
};

// node for hash table list
struct acc_node {
    struct account* acc;
    struct acc_node* next;
};

// for service requests
struct req {
    char type[50];
    long acc_num;
    double amt;
};

struct q_node {
    struct req r;
    struct q_node* next;
};

struct queue {
    struct q_node* front;
    struct q_node* rear;
};

// main bank struct
struct bank {
    struct acc_node* table[SIZE];
    struct queue req_q;
};


// --- Function Prototypes ---

void init_bank(struct bank* b);
void cleanup(struct bank* b);

unsigned long hash(long num);
struct account* new_acc(long num, const char* name, double bal);
int add_acc(struct bank* b, struct account* a);
struct account* find_acc(struct bank* b, long num);

void deposit(struct account* a, double amt);
int withdraw(struct account* a, double amt);
int transfer(struct bank* b, long from_n, long to_n, double amt);
void show_balance(struct account* a);
void calc_interest(struct account* a, double r, int t);

// stack functions
void push_trans(struct trans_stack* s, const char* type, double amt);
void print_history(struct account* a);

// queue and loan functions
void apply_loan(struct bank* b, long num, double amt);
void enqueue(struct queue* q, struct req r);
int dequeue(struct queue* q, struct req* r);
void process_req(struct bank* b);
void show_loan(struct account* a);

#endif // BANK_H