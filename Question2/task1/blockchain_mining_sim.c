#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_BLOCKS 100
#define MAX_TRANSACTIONS 10
#define MAX_DATA_LEN 256
#define HASH_SIZE 65

typedef struct Block {
    int index;
    time_t timestamp;
    char transactions[MAX_TRANSACTIONS][MAX_DATA_LEN];
    int transactionCount;
    char previousHash[HASH_SIZE];
    char hash[HASH_SIZE];
    int nonce;
} Block;

Block blockchain[MAX_BLOCKS];
int blockCount = 0;

// === 🔐 SHA-256 Hashing ===
void calculateSHA256(char *input, char output[HASH_SIZE]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[64] = '\0';
}

// === 🔢 Compute Block Hash ===
void computeBlockHash(Block *block) {
    char transactionsConcat[MAX_TRANSACTIONS * MAX_DATA_LEN] = {0};
    for (int i = 0; i < block->transactionCount; i++) {
        strncat(transactionsConcat, block->transactions[i], MAX_DATA_LEN - 1);
    }

    // Estimate a safe size
    char buffer[4096] = {0};
    snprintf(buffer, sizeof(buffer), "%d%ld%s%s%d",
             block->index, block->timestamp,
             transactionsConcat,
             block->previousHash, block->nonce);

    calculateSHA256(buffer, block->hash);
}

// === 📦 Create New Block ===
Block createBlock(char *prevHash, char transactions[][MAX_DATA_LEN], int txnCount) {
    Block block;
    block.index = blockCount;
    block.timestamp = time(NULL);
    block.transactionCount = txnCount;
    block.nonce = 0;

    strcpy(block.previousHash, prevHash);

    for (int i = 0; i < txnCount; i++) {
        strcpy(block.transactions[i], transactions[i]);
    }

    computeBlockHash(&block);
    return block;
}

// === 📜 Print Blockchain ===
void printBlockchain() {
    if (blockCount == 0) {
        printf("🚫 Blockchain is empty.\n");
        return;
    }

    for (int i = 0; i < blockCount; i++) {
        Block *b = &blockchain[i];
        printf("\n🔗 === Block %d ===\n", b->index);
        printf("🕒 Timestamp: %s", ctime(&b->timestamp));
        printf("💬 Transactions:\n");
        for (int j = 0; j < b->transactionCount; j++) {
            printf("  - %s\n", b->transactions[j]);
        }
        printf("⛓️  Previous Hash: %s\n", b->previousHash);
        printf("🧮 Nonce: %d\n", b->nonce);
        printf("🔑 Hash: %s\n", b->hash);
    }
}

// === 🧾 Add Block via User Input ===
void addBlockFromInput() {
    char transactions[MAX_TRANSACTIONS][MAX_DATA_LEN];
    int txnCount;

    printf("📝 Enter number of transactions (max %d): ", MAX_TRANSACTIONS);
    scanf("%d", &txnCount);
    getchar(); // consume newline

    if (txnCount <= 0 || txnCount > MAX_TRANSACTIONS) {
        printf("❌ Invalid number of transactions.\n");
        return;
    }

    for (int i = 0; i < txnCount; i++) {
        printf("  ➤ Enter transaction %d (e.g., Alice -> Bob: 50): ", i + 1);
        fgets(transactions[i], MAX_DATA_LEN, stdin);
        transactions[i][strcspn(transactions[i], "\n")] = 0; // remove newline
    }

    char prevHash[HASH_SIZE] = "0";
    if (blockCount > 0) {
        strcpy(prevHash, blockchain[blockCount - 1].hash);
    }

    Block newBlock = createBlock(prevHash, transactions, txnCount);
    blockchain[blockCount++] = newBlock;

    printf("✅ Block %d added to blockchain!\n", newBlock.index);
}

// === 📋 Main Menu ===
void showMenu() {
    int choice;
    do {
        printf("\n===== 📋 Blockchain Menu =====\n");
        printf("1. Add New Block\n");
        printf("2. View Blockchain\n");
        printf("3. Exit\n");
        printf("Select an option: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1:
                addBlockFromInput();
                break;
            case 2:
                printBlockchain();
                break;
            case 3:
                printf("👋 Exiting blockchain simulation.\n");
                break;
            default:
                printf("❌ Invalid option.\n");
        }
    } while (choice != 3);
}

// === 🚀 Main ===
int main() {
    printf("🔐 Simple Blockchain Simulator (Account/Transaction Model)\n");
    showMenu();
    return 0;
}
