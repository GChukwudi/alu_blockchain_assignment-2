#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
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
int difficulty = 4;  // Default difficulty

// 🧮 SHA-256 Hash
void calculateSHA256(char *input, char output[HASH_SIZE]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + (i * 2), "%02x", hash[i]);
    output[64] = '\0';
}

// 🔧 Hash Block
void computeBlockHash(Block *block) {
    char buffer[2048] = {0};
    char txConcat[MAX_TRANSACTIONS * MAX_DATA_LEN] = {0};
    for (int i = 0; i < block->transactionCount; i++)
        strcat(txConcat, block->transactions[i]);

    snprintf(buffer, sizeof(buffer), "%d%ld%s%s%d",
             block->index, block->timestamp, txConcat, block->previousHash, block->nonce);

    calculateSHA256(buffer, block->hash);
}

// ⛏️ Mine Block
void mineBlock(Block *block) {
    char target[HASH_SIZE];
    memset(target, '0', difficulty);
    target[difficulty] = '\0';

    printf("🔍 Mining block %d with difficulty %d...\n", block->index, difficulty);

    clock_t start = clock();
    do {
        block->nonce++;
        computeBlockHash(block);
    } while (strncmp(block->hash, target, difficulty) != 0);
    clock_t end = clock();

    double timeTaken = (double)(end - start) / CLOCKS_PER_SEC;

    printf("✅ Block mined! Nonce: %d\n", block->nonce);
    printf("🔑 Hash: %s\n", block->hash);
    printf("⏱️ Mining Time: %.2f seconds\n", timeTaken);
}

// 📦 Create Block
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

// 🧾 Add Block from User Input
void addBlockFromInput() {
    int txnCount;
    char transactions[MAX_TRANSACTIONS][MAX_DATA_LEN];

    printf("\n📨 Enter number of transactions (max %d): ", MAX_TRANSACTIONS);
    scanf("%d", &txnCount);
    getchar(); // remove newline

    if (txnCount <= 0 || txnCount > MAX_TRANSACTIONS) {
        printf("❌ Invalid number of transactions.\n");
        return;
    }

    printf("✍️ Enter transactions (format: sender -> receiver: amount):\n");
    for (int i = 0; i < txnCount; i++) {
        printf("   ➤ Transaction %d: ", i + 1);
        fgets(transactions[i], MAX_DATA_LEN, stdin);
        transactions[i][strcspn(transactions[i], "\n")] = '\0';
    }

    char prevHash[HASH_SIZE] = "0";
    if (blockCount > 0)
        strcpy(prevHash, blockchain[blockCount - 1].hash);

    Block newBlock = createBlock(prevHash, transactions, txnCount);
    blockchain[blockCount++] = newBlock;

    printf("🧱 Block %d added to blockchain!\n", newBlock.index);
}

// 🧬 View Blockchain
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
        for (int j = 0; j < b->transactionCount; j++)
            printf("   • %s\n", b->transactions[j]);
        printf("🔗 Previous Hash  : %.20s...\n", b->previousHash);
        printf("🧮 Nonce          : %d\n", b->nonce);
        printf("🔐 Hash           : %.20s...\n", b->hash);
    }

    printf("\n=========================== 🧬\n");
}

// ✅ Verify Blockchain
int verifyBlockchain() {
    for (int i = 1; i < blockCount; i++) {
        char expectedHash[HASH_SIZE];
        computeBlockHash(&blockchain[i - 1]);
        strcpy(expectedHash, blockchain[i - 1].hash);

        if (strcmp(expectedHash, blockchain[i].previousHash) != 0) {
            printf("❌ Invalid previous hash at block %d\n", i);
            return 0;
        }

        computeBlockHash(&blockchain[i]);
        char target[HASH_SIZE];
        memset(target, '0', difficulty);
        target[difficulty] = '\0';

        if (strncmp(blockchain[i].hash, target, difficulty) != 0) {
            printf("❌ Invalid hash at block %d\n", i);
            return 0;
        }
    }
    return 1;
}

// 🧪 Benchmark Mode
void benchmarkDifficulty() {
    int maxDifficulty;
    printf("🎯 Enter maximum difficulty to test up to: ");
    scanf("%d", &maxDifficulty);
    getchar();

    for (int d = 1; d <= maxDifficulty; d++) {
        difficulty = d;
        char txs[1][MAX_DATA_LEN] = {"benchmark -> test: 1"};
        char prevHash[HASH_SIZE] = "0";
        if (blockCount > 0)
            strcpy(prevHash, blockchain[blockCount - 1].hash);
        Block testBlock = createBlock(prevHash, txs, 1);
        printf("🧱 Benchmark block at difficulty %d complete.\n", d);
        printf("---------------------------------------------\n");
    }
}

// ⚙️ Set Mining Difficulty
void adjustDifficulty() {
    printf("🎯 Enter new mining difficulty: ");
    scanf("%d", &difficulty);
    getchar();
    printf("✅ Difficulty set to %d leading zeros.\n", difficulty);
}

// 📋 Menu
void showMenu() {
    int choice;
    do {
        printf("\n=============================\n");
        printf("📋 Blockchain Menu\n");
        printf("=============================\n");
        printf("1️⃣  Add New Block\n");
        printf("2️⃣  View Blockchain\n");
        printf("3️⃣  Verify Blockchain\n");
        printf("4️⃣  Adjust Difficulty\n");
        printf("5️⃣  Benchmark Difficulty\n");
        printf("6️⃣  Exit\n");
        printf("=============================\n");
        printf("Select an option: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: addBlockFromInput(); break;
            case 2: printBlockchain(); break;
            case 3:
                if (verifyBlockchain())
                    printf("✅ Blockchain integrity verified.\n");
                else
                    printf("❌ Blockchain is invalid or tampered.\n");
                break;
            case 4: adjustDifficulty(); break;
            case 5: benchmarkDifficulty(); break;
            case 6: printf("👋 Exiting. Goodbye!\n"); break;
            default: printf("❌ Invalid option.\n");
        }
    } while (choice != 6);
}

// 🚀 Main
int main() {
    printf("🔐 Welcome to the Blockchain Simulator with Proof-of-Work\n");
    printf("⚙️  Mining difficulty set to %d leading zeros\n", difficulty);
    showMenu();
    return 0;
}
