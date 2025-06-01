#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>     // ✅ For sleep()

#include <openssl/sha.h>

#define MAX_BLOCKS 100
#define MAX_TRANSACTIONS 10
#define MAX_DATA_LEN 256
#define HASH_SIZE 65
#define DIFFICULTY 4

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

// 🎯 SHA-256 Hash Function
void calculateSHA256(char *input, char output[HASH_SIZE]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + (i * 2), "%02x", hash[i]);
    output[64] = '\0';
}

// 🔧 Compute Hash for a Block
void computeBlockHash(Block *block) {
    char buffer[2048] = {0};
    char txConcat[MAX_TRANSACTIONS * MAX_DATA_LEN] = {0};
    for (int i = 0; i < block->transactionCount; i++)
        strcat(txConcat, block->transactions[i]);

    snprintf(buffer, sizeof(buffer), "%d%ld%s%s%d",
             block->index, block->timestamp, txConcat, block->previousHash, block->nonce);

    calculateSHA256(buffer, block->hash);
}

// ⛏️ Proof-of-Work Mining
void mineBlock(Block *block) {
    char target[DIFFICULTY + 1];
    memset(target, '0', DIFFICULTY);
    target[DIFFICULTY] = '\0';

    printf("🔍 Mining block %d...\n", block->index);

    do {
        block->nonce++;
        computeBlockHash(block);
    } while (strncmp(block->hash, target, DIFFICULTY) != 0);

    sleep(2); // ⏱️ Delay to simulate mining effort

    printf("✅ Block mined! Nonce: %d\n", block->nonce);
    printf("🔑 Hash: %s\n", block->hash);
}

// 📦 Create a New Block
Block createBlock(char *prevHash, char transactions[][MAX_DATA_LEN], int txnCount) {
    Block block;
    block.index = blockCount;
    block.timestamp = time(NULL);
    block.transactionCount = txnCount;
    block.nonce = 0;
    strcpy(block.previousHash, prevHash);

    for (int i = 0; i < txnCount; i++)
        strcpy(block.transactions[i], transactions[i]);

    mineBlock(&block);
    return block;
}

// 🧾 Add Block with User Transactions
void addBlockFromInput() {
    int txnCount;
    char transactions[MAX_TRANSACTIONS][MAX_DATA_LEN];

    printf("\n📨 Enter number of transactions (max %d): ", MAX_TRANSACTIONS);
    scanf("%d", &txnCount);
    getchar(); // Consume newline

    if (txnCount <= 0 || txnCount > MAX_TRANSACTIONS) {
        printf("❌ Invalid number of transactions.\n");
        return;
    }

    printf("✍️ Enter transactions (format: sender -> receiver: amount):\n");
    for (int i = 0; i < txnCount; i++) {
        printf("   ➤ Transaction %d: ", i + 1);
        fgets(transactions[i], MAX_DATA_LEN, stdin);
        transactions[i][strcspn(transactions[i], "\n")] = '\0'; // Remove newline
    }

    char prevHash[HASH_SIZE] = "0";
    if (blockCount > 0)
        strcpy(prevHash, blockchain[blockCount - 1].hash);

    Block newBlock = createBlock(prevHash, transactions, txnCount);
    blockchain[blockCount++] = newBlock;

    printf("🧱 Block %d successfully added to the blockchain!\n", newBlock.index);
}

// 📜 Display the Entire Blockchain
void printBlockchain() {
    if (blockCount == 0) {
        printf("\n🚫 Blockchain is empty.\n");
        return;
    }

    printf("\n🧬 ====== Blockchain ======\n");

    for (int i = 0; i < blockCount; i++) {
        Block *b = &blockchain[i];
        printf("\n🔗 ── Block %d ───────────────────────────\n", b->index);
        printf("🕒 Timestamp      : %s", ctime(&b->timestamp));
        printf("💬 Transactions   :\n");
        for (int j = 0; j < b->transactionCount; j++) {
            printf("   • %s\n", b->transactions[j]);
        }
        printf("🔗 Previous Hash  : %.20s...\n", b->previousHash);
        printf("🧮 Nonce          : %d\n", b->nonce);
        printf("🔐 Hash           : %.20s...\n", b->hash);
    }

    printf("\n=========================== 🧬\n");
}

// 📋 Main Menu
void showMenu() {
    int choice;
    do {
        printf("\n=============================\n");
        printf("📋 Blockchain Menu\n");
        printf("=============================\n");
        printf("1️⃣  Add New Block\n");
        printf("2️⃣  View Blockchain\n");
        printf("3️⃣  Exit\n");
        printf("=============================\n");
        printf("Select an option: ");
        scanf("%d", &choice);
        getchar(); // Consume newline

        switch (choice) {
            case 1:
                addBlockFromInput();
                break;
            case 2:
                printBlockchain();
                break;
            case 3:
                printf("👋 Exiting blockchain simulator. Bye!\n");
                break;
            default:
                printf("❌ Invalid option. Try again.\n");
        }
    } while (choice != 3);
}

// 🚀 Main Entry
int main() {
    printf("🔐 Welcome to the Blockchain Simulator with Proof-of-Work\n");
    printf("⚙️  Mining difficulty set to %d leading zeros\n", DIFFICULTY);
    showMenu();
    return 0;
}
