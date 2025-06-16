#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAXMEM 500  // Maximum buffer size for reading lines

// Define a struct for a bank account
struct account {
    int accNum;     // Account number
    int accPin;     // Account PIN
    float balance;  // Account balance
};

int accountNumbers = 0;  // Global variable to store number of accounts

// Function declarations
void atmInterface(int index, struct account accounts[]);
void checkBal(int index, struct account accounts[]);
void deposit(int index, struct account accounts[]);
void withdraw(int index, struct account accounts[]);
void saveChanges(struct account accounts[]);
void recordChanges(int accNum, char type, float amount);

// Load account data from file into memory
struct account *loadToMem(int *accountNumbers) {
    FILE *fp;
    char buffer[MAXMEM];
    int lines = 0;

    fp = fopen("accounts.txt", "r");
    while (fgets(buffer, MAXMEM, fp) != NULL) {
        lines++;  // Count the number of lines/accounts
    }

    rewind(fp);  // Go back to beginning of file

    struct account *accounts = malloc(sizeof(struct account) * lines);
    *accountNumbers = lines;

    int i = 0;
    // Read account details into memory
    while (fgets(buffer, MAXMEM, fp) != NULL && i < lines) {
        sscanf(buffer, "%d %d %f", &accounts[i].accNum, &accounts[i].accPin, &accounts[i].balance);
        i++;
    }
    fclose(fp);
    return accounts;
}

// Login prompt for user to access ATM
void loginInterface(struct account accounts[], int *accountNumbers) {
    int accNum, accPin, index;
    int found = 0;

    printf("\nATM\n");

    // Ask for account number
    do {
        printf("\nEnter your Account Number: ");
        scanf("%d", &accNum);
        getchar();

        for (int i = 0; i < *accountNumbers; i++) {
            if (accounts[i].accNum == accNum) {
                found = 1;
                index = i;
                break;
            }
        }
    } while (found == 0);

    found = 0;
    // Ask for PIN
    do {
        printf("\nEnter your PIN: ");
        scanf("%d", &accPin);
        getchar();

        if (accounts[index].accPin == accPin) {
            found = 1;
        }
    } while (found == 0);

    // Proceed to ATM menu
    atmInterface(index, accounts);
}

// ATM menu interface for a logged-in user
void atmInterface(int index, struct account accounts[]) {
    printf("Welcome Account Num: %d!\nWhat do you want to do:\n", accounts[index].accNum);
    int choice = 0;

    // Display menu until valid choice
    while (choice < 1 || choice > 4) {
        printf("1)Check Balance\n2)Deposit\n3)Withdraw\n4)Exit");
        printf("\nYour Choice: ");
        scanf("%d", &choice);
        getchar();
    }

    // Call function based on user's choice
    switch (choice) {
        case 1:
            checkBal(index, accounts);
            break;
        case 2:
            deposit(index, accounts);
            break;
        case 3:
            withdraw(index, accounts);
            break;
        case 4:
            exit(0);
        default:
            exit(1);
    }
}

// Display account balance
void checkBal(int index, struct account accounts[]) {
    printf("\nHello Account Number: %d\n", accounts[index].accNum);
    printf("Here's Your Balance:\n");
    printf("%.2f\n", accounts[index].balance);
    printf("Press Any Key to Continue.....\n");
    getchar();  // Wait for user input
    getchar();

    atmInterface(index, accounts);  // Return to menu
}

// Deposit money into account
void deposit(int index, struct account accounts[]) {
    float depositAm;
    char type = 'd';

    printf("\nHello Account Number: %d\n", accounts[index].accNum);
    printf("How much do you want to deposit?: ");
    scanf("%f", &depositAm);
    getchar();

    accounts[index].balance += depositAm;  // Add to balance

    saveChanges(accounts);  // Save updated balances
    recordChanges(accounts[index].accNum, type, depositAm);  // Log transaction

    atmInterface(index, accounts);
}

// Withdraw money from account
void withdraw(int index, struct account accounts[]) {
    float amToWithdraw;
    char type = 'w';

    printf("\nHello Account Number: %d\n", accounts[index].accNum);
    printf("How much do you want to Withdraw?: ");
    scanf("%f", &amToWithdraw);
    getchar();

    // Ensure sufficient balance
    while (amToWithdraw > accounts[index].balance) {
        printf("Insufficient Funds\nEnter Again: ");
        scanf("%f", &amToWithdraw);
        getchar();
    }

    accounts[index].balance -= amToWithdraw;  // Deduct from balance

    recordChanges(accounts[index].accNum, type, amToWithdraw);  // Log transaction
    saveChanges(accounts);

    atmInterface(index, accounts);
}

// Write all account data back to the file
void saveChanges(struct account accounts[]) {
    FILE *fp;
    char buffer[MAXMEM];

    fp = fopen("accounts.txt", "w+");

    for (int i = 0; i < accountNumbers; i++) {
        fprintf(fp, "%d %d %.2f\n", accounts[i].accNum, accounts[i].accPin, accounts[i].balance);
    }

    fclose(fp);
}

// Record each transaction with timestamp
void recordChanges(int accNum, char type, float amount) {
    FILE *fp;

    fp = fopen("transactions.txt", "a");
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strcspn(time_str, "\n")] = 0;  // Remove newline

    fprintf(fp, "%s %d %c %.2f\n\n", time_str, accNum, type, amount);
    fclose(fp);
}

// Entry point
int main() {
    struct account* accounts = loadToMem(&accountNumbers);  // Load accounts from file
    loginInterface(accounts, &accountNumbers);  // Start login interface
}
