#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ANSI color codes for better CLI UI
#define COLOR_GREEN "\033[1;32m"
#define COLOR_RED "\033[1;31m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_CYAN "\033[1;36m"
#define COLOR_RESET "\033[0m"

#define MAX_ADDRESS_LEN 20
#define MAX_UTXOS 100
#define MAX_TXID_LEN 10
#define MAX_USERS 50

// UTXO structure
typedef struct {
    char txid[MAX_TXID_LEN];
    char address[MAX_ADDRESS_LEN];
    float amount;
    bool spent;
} UTXO;

// User structure
typedef struct {
    char name[MAX_ADDRESS_LEN];
    char address[MAX_ADDRESS_LEN];
} User;

// Globals
UTXO utxo_list[MAX_UTXOS];
int utxo_count = 0;
User user_list[MAX_USERS];
int user_count = 0;

// Generate address
void generate_address(char *address, int index) {
    snprintf(address, MAX_ADDRESS_LEN, "ADDRESS%d", index + 1);
}

// Get user by address
const char* get_user_name_by_address(const char *address) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_list[i].address, address) == 0) {
            return user_list[i].name;
        }
    }
    return "Unknown";
}

bool is_valid_address(const char *address) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(user_list[i].address, address) == 0) {
            return true;
        }
    }
    return false;
}

// Add user interactively
void add_user() {
    if (user_count >= MAX_USERS) {
        printf(COLOR_RED "User limit reached.\n" COLOR_RESET);
        return;
    }

    char name[MAX_ADDRESS_LEN];
    printf(COLOR_YELLOW "Enter new user name: " COLOR_RESET);
    scanf("%s", name);

    strcpy(user_list[user_count].name, name);
    generate_address(user_list[user_count].address, user_count);
    printf(COLOR_GREEN "✅ User '%s' created with address '%s'\n" COLOR_RESET,
           name, user_list[user_count].address);
    user_count++;
}

// Show all users
void display_users() {
    printf(COLOR_CYAN "\n=== 👥 Users ===\n" COLOR_RESET);
    printf("Name               | Address\n");
    printf("------------------------------\n");
    for (int i = 0; i < user_count; i++) {
        printf("%-18s | %s\n", user_list[i].name, user_list[i].address);
    }
}

// Show unspent UTXOs
void display_utxos() {
    printf(COLOR_CYAN "\n=== 💰 Unspent UTXOs ===\n" COLOR_RESET);
    printf("Index | TXID  | User (Address)         | Amount  \n");
    printf("------------------------------------------------\n");
    for (int i = 0; i < utxo_count; i++) {
        if (!utxo_list[i].spent) {
            printf("%-5d | %-5s | %-18s (%s) | %.2f\n", i, utxo_list[i].txid,
                   get_user_name_by_address(utxo_list[i].address),
                   utxo_list[i].address, utxo_list[i].amount);
        }
    }
}

// Show all UTXOs
void display_all_utxos() {
    printf(COLOR_CYAN "\n=== 📦 All UTXOs ===\n" COLOR_RESET);
    printf("Index | TXID  | User (Address)         | Amount  | Spent\n");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < utxo_count; i++) {
        printf("%-5d | %-5s | %-18s (%s) | %.2f | %s\n", i, utxo_list[i].txid,
               get_user_name_by_address(utxo_list[i].address),
               utxo_list[i].address, utxo_list[i].amount,
               utxo_list[i].spent ? "Yes" : "No");
    }
}

void generate_txid(char *txid) {
    static int counter = 1;
    snprintf(txid, MAX_TXID_LEN, "TX%d", counter++);
}

void initialize_user_utxos(float amount_per_user) {
    for (int i = 0; i < user_count; i++) {
        char txid[MAX_TXID_LEN];
        generate_txid(txid);
        strcpy(utxo_list[utxo_count].txid, txid);
        strcpy(utxo_list[utxo_count].address, user_list[i].address);
        utxo_list[utxo_count].amount = amount_per_user;
        utxo_list[utxo_count].spent = false;
        utxo_count++;
    }
    printf(COLOR_GREEN "✅ Each user initialized with %.2f UTXO.\n" COLOR_RESET, amount_per_user);
}

// Transaction process
void perform_transaction() {
    char sender[MAX_ADDRESS_LEN], receiver[MAX_ADDRESS_LEN];
    float amount;

    display_users();
    printf(COLOR_YELLOW "Enter sender's address: " COLOR_RESET); scanf("%s", sender);
    if (!is_valid_address(sender)) {
        printf(COLOR_RED "Invalid sender address.\n" COLOR_RESET); return;
    }

    printf(COLOR_YELLOW "Enter receiver's address: " COLOR_RESET); scanf("%s", receiver);
    if (!is_valid_address(receiver)) {
        printf(COLOR_RED "Invalid receiver address.\n" COLOR_RESET); return;
    }

    if (strcmp(sender, receiver) == 0) {
        printf(COLOR_RED "Sender and receiver must be different.\n" COLOR_RESET);
        return;
    }

    printf(COLOR_YELLOW "Enter amount to send: " COLOR_RESET); scanf("%f", &amount);
    if (amount <= 0) {
        printf(COLOR_RED "Amount must be positive.\n" COLOR_RESET); return;
    }

    // Display sender's UTXOs
    float total = 0.0;
    int selected[MAX_UTXOS], sel_count = 0;
    printf(COLOR_CYAN "\nAvailable UTXOs for sender:\n" COLOR_RESET);
    for (int i = 0; i < utxo_count; i++) {
        if (!utxo_list[i].spent && strcmp(utxo_list[i].address, sender) == 0) {
            printf("[%d] TXID: %s - %.2f\n", i, utxo_list[i].txid, utxo_list[i].amount);
            total += utxo_list[i].amount;
        }
    }

    if (total < amount) {
        printf(COLOR_RED "Insufficient funds. Available: %.2f\n" COLOR_RESET, total);
        return;
    }

    printf(COLOR_YELLOW "\nSelect UTXOs by index to spend (-1 to stop):\n" COLOR_RESET);
    float collected = 0.0;
    while (collected < amount) {
        int idx;
        printf("Enter UTXO index: "); scanf("%d", &idx);
        if (idx == -1) break;
        if (idx < 0 || idx >= utxo_count || utxo_list[idx].spent ||
            strcmp(utxo_list[idx].address, sender) != 0) {
            printf(COLOR_RED "Invalid UTXO.\n" COLOR_RESET); continue;
        }

        collected += utxo_list[idx].amount;
        selected[sel_count++] = idx;
    }

    if (collected < amount) {
        printf(COLOR_RED "Selected UTXOs (%.2f) do not cover %.2f\n" COLOR_RESET, collected, amount);
        return;
    }

    for (int i = 0; i < sel_count; i++) {
        utxo_list[selected[i]].spent = true;
    }

    char txid1[MAX_TXID_LEN], txid2[MAX_TXID_LEN];
    generate_txid(txid1);
    strcpy(utxo_list[utxo_count].txid, txid1);
    strcpy(utxo_list[utxo_count].address, receiver);
    utxo_list[utxo_count].amount = amount;
    utxo_list[utxo_count].spent = false;
    utxo_count++;

    float change = collected - amount;
    if (change > 0) {
        generate_txid(txid2);
        strcpy(utxo_list[utxo_count].txid, txid2);
        strcpy(utxo_list[utxo_count].address, sender);
        utxo_list[utxo_count].amount = change;
        utxo_list[utxo_count].spent = false;
        utxo_count++;
    }

    printf(COLOR_GREEN "\n✅ Transaction Complete!\n" COLOR_RESET);
    printf("Sender: %s → Receiver: %s | Amount: %.2f | Change: %.2f\n",
           get_user_name_by_address(sender), get_user_name_by_address(receiver), amount, change);
}

// Main Menu
int main() {
    printf(COLOR_GREEN "=====================================\n" COLOR_RESET);
    printf(COLOR_GREEN "   💸 UTXO Blockchain Simulator      \n" COLOR_RESET);
    printf(COLOR_GREEN "=====================================\n" COLOR_RESET);

    // Create users at runtime
    int initial_users;
    printf(COLOR_YELLOW "How many users to create? " COLOR_RESET);
    scanf("%d", &initial_users);
    for (int i = 0; i < initial_users; i++) {
        printf("Enter name for user %d: ", i + 1);
        char name[MAX_ADDRESS_LEN];
        scanf("%s", name);
        strcpy(user_list[user_count].name, name);
        generate_address(user_list[user_count].address, user_count);
        user_count++;
    }

    initialize_user_utxos(60.0);

    while (1) {
        printf(COLOR_CYAN "\n===== 📋 Main Menu =====\n" COLOR_RESET);
        printf("1. View Unspent UTXOs\n");
        printf("2. View All UTXOs\n");
        printf("3. View Users\n");
        printf("4. Perform Transaction\n");
        printf("5. Add New User\n");
        printf("6. Exit\n");
        printf(COLOR_YELLOW "Select option: " COLOR_RESET);
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1: display_utxos(); break;
            case 2: display_all_utxos(); break;
            case 3: display_users(); break;
            case 4: perform_transaction(); break;
            case 5: add_user(); break;
            case 6:
                printf(COLOR_GREEN "Goodbye!\n" COLOR_RESET);
                return 0;
            default:
                printf(COLOR_RED "Invalid choice.\n" COLOR_RESET);
        }
    }

    return 0;
}
