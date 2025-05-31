#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_ACCOUNTS 10
#define MAX_NAME_LEN 50

// Define the Account structure
typedef struct {
    char name[MAX_NAME_LEN];
    float balance;
} Account;

Account accounts[MAX_ACCOUNTS];
int accountCount = 0;

// Function to add an account
void addAccount(const char *name, float balance) {
    if (accountCount < MAX_ACCOUNTS) {
        strncpy(accounts[accountCount].name, name, MAX_NAME_LEN);
        accounts[accountCount].balance = balance;
        accountCount++;
    }
}

// Function to find an account index by name
int findAccountIndex(const char *name) {
    for (int i = 0; i < accountCount; i++) {
        if (strcmp(accounts[i].name, name) == 0) {
            return i;
        }
    }
    return -1; // Not found
}

// Function to display all account balances
void displayBalances() {
    printf("\n📄 Account Balances:\n");
    printf("-----------------------------\n");
    printf("Name\t\tBalance\n");
    printf("-----------------------------\n");
    for (int i = 0; i < accountCount; i++) {
        printf("%-10s\t%.2f\n", accounts[i].name, accounts[i].balance);
    }
    printf("-----------------------------\n");
}

// Function to transfer funds between accounts
bool transferFunds(const char *senderName, const char *receiverName, float amount) {
    int senderIndex = findAccountIndex(senderName);
    int receiverIndex = findAccountIndex(receiverName);

    if (senderIndex == -1 || receiverIndex == -1) {
        printf("❌ Error: Sender or receiver account not found.\n");
        return false;
    }

    if (amount <= 0) {
        printf("❌ Error: Amount must be positive.\n");
        return false;
    }

    if (accounts[senderIndex].balance < amount) {
        printf("❌ Error: Insufficient balance.\n");
        return false;
    }

    accounts[senderIndex].balance -= amount;
    accounts[receiverIndex].balance += amount;

    printf("✅ Transfer of %.2f from %s to %s completed.\n", amount, senderName, receiverName);
    return true;
}

int main() {
    // Initialize accounts
    addAccount("Alice", 100.0);
    addAccount("Bob", 50.0);
    addAccount("Charlie", 75.0);

    printf("🚀 Welcome to Ethereum-style Account Model!\n");

    int choice;
    char sender[MAX_NAME_LEN], receiver[MAX_NAME_LEN];
    float amount;

    while (1) {
        printf("\n===== 📋 Main Menu =====\n");
        printf("1. Display Account Balances\n");
        printf("2. Transfer Funds\n");
        printf("3. Exit\n");
        printf("Select option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayBalances();
                break;

            case 2:
                displayBalances();
                printf("Enter sender name: ");
                scanf("%s", sender);
                printf("Enter receiver name: ");
                scanf("%s", receiver);
                printf("Enter amount to transfer: ");
                scanf("%f", &amount);

                if (transferFunds(sender, receiver, amount)) {
                    printf("🔄 Updated balances:\n");
                    displayBalances();
                }
                break;

            case 3:
                printf("👋 Goodbye!\n");
                return 0;

            default:
                printf("❌ Invalid option. Please try again.\n");
        }
    }

    return 0;
}
