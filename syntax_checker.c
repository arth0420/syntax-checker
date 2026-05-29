#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ============================
// Stack Node
// ============================
typedef struct StackNode {
    char symbol;
    int line;
    struct StackNode *next;
} StackNode;

StackNode *top = NULL;

// Stack operations
void push(char symbol, int line) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    newNode->symbol = symbol;
    newNode->line = line;
    newNode->next = top;
    top = newNode;
}
char pop() {
    if (top == NULL) return '\0';
    char sym = top->symbol;
    StackNode *tmp = top;
    top = top->next;
    free(tmp);
    return sym;
}
int isEmpty() { return top == NULL; }

// ============================
// Queue Node
// ============================
typedef struct QueueNode {
    char line[256];
    int lineNumber;
    struct QueueNode *next;
} QueueNode;

QueueNode *front = NULL, *rear = NULL;

void enqueue(char *line, int lineNumber) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    strcpy(newNode->line, line);
    newNode->lineNumber = lineNumber;
    newNode->next = NULL;
    if (rear == NULL)
        front = rear = newNode;
    else {
        rear->next = newNode;
        rear = newNode;
    }
}
QueueNode *dequeue() {
    if (front == NULL) return NULL;
    QueueNode *temp = front;
    front = front->next;
    if (front == NULL) rear = NULL;
    return temp;
}
void clearQueue() {
    while (front != NULL) {
        QueueNode *tmp = dequeue();
        free(tmp);
    }
}

// ============================
// Helper Functions
// ============================
int isOpening(char c) { return c=='(' || c=='{' || c=='['; }
int isClosing(char c) { return c==')' || c=='}' || c==']'; }
int isMatching(char o, char c) {
    return (o=='(' && c==')') || (o=='{' && c=='}') || (o=='[' && c==']');
}
char getClosing(char o) {
    if (o=='(') return ')';
    if (o=='{') return '}';
    if (o=='[') return ']';
    return ' ';
}

// ============================
// Syntax Check + Correction
// ============================
void printCorrection(char lines[200][256], int total, char missing[], int missCount) {
    if (missCount == 0) return;
    printf("\n\033[1;34m--- Suggested Correction ---\033[0m\n");
    for (int i = 0; i < total; i++)
        printf("%s\n", lines[i]);
    printf("\033[1;33m// Added missing closing symbols: ");
    for (int i = missCount - 1; i >= 0; i--)
        printf("%c", missing[i]);
    printf("\033[0m\n");
}

void checkSyntax(char lines[200][256], int total) {
    int errors = 0;
    char missing[100];
    int missCount = 0;

    while (front != NULL) {
        QueueNode *node = dequeue();
        char *line = node->line;
        int lineNum = node->lineNumber;

        for (int i = 0; line[i] != '\0'; i++) {
            if (isOpening(line[i])) {
                push(line[i], lineNum);
            } else if (isClosing(line[i])) {
                if (isEmpty()) {
                    printf("\033[1;31mSyntax Error:\033[0m Extra '%c' at line %d\n", line[i], lineNum);
                    errors++;
                } else {
                    char o = pop();
                    if (!isMatching(o, line[i])) {
                        printf("\033[1;31mSyntax Error:\033[0m Mismatched '%c' at line %d (opened with '%c')\n",
                                line[i], lineNum, o);
                        errors++;
                    }
                }
            }
        }
        free(node);
    }

    // Record all missing closings before clearing stack
    while (!isEmpty() && missCount < 100) {
        char o = pop();
        missing[missCount++] = getClosing(o);
        printf("\033[1;31mSyntax Error:\033[0m Unmatched '%c' opened earlier\n", o);
        errors++;
    }

    if (errors == 0)
        printf("\033[1;32mAll symbols are balanced.\033[0m\n");
    else {
        printf("\nSyntax checking completed with errors above.\n");
        printCorrection(lines, total, missing, missCount);
    }
}

// ============================
// File Input (Handles Spaces + Quotes)
// ============================
void loadFile() {
    char filename[300];
    printf("Enter filename (with extension): ");
    getchar(); // clear newline from menu input
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0; // remove newline

    // Remove quotes if any
    if (filename[0] == '"' || filename[0] == '\'')
        memmove(filename, filename + 1, strlen(filename));
    int len = strlen(filename);
    if (len > 0 && (filename[len - 1] == '"' || filename[len - 1] == '\''))
        filename[len - 1] = '\0';

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("\033[1;31mError:\033[0m Could not open file '%s'\n", filename);
        return;
    }

    char lines[200][256];
    int total = 0;
    char line[256];
    int lineNum = 1;

    clearQueue();
    while (fgets(line, sizeof(line), fp)) {
        strcpy(lines[total++], line);
        enqueue(line, lineNum++);
    }
    fclose(fp);
    checkSyntax(lines, total);
}

// ============================
// MAIN MENU
// ============================
int main() {
    int choice;
    do {
        printf("\n============================================\n");
        printf("        SYNTAX CHECKER PROJECT (C)\n");
        printf("============================================\n");
        printf("1. Enter code manually\n");
        printf("2. Load and check code from file\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            getchar(); // clear newline
            clearQueue();
            printf("\nEnter your C code below (type END to finish):\n");
            char lines[200][256];
            int total = 0;
            char line[256];
            int lineNum = 1;

            while (1) {
                printf("%2d | ", lineNum);
                fgets(line, sizeof(line), stdin);
                line[strcspn(line, "\n")] = '\0';

                char temp[256];
                strcpy(temp, line);
                for (int i=0; temp[i]; i++) temp[i]=tolower(temp[i]);
                if (strcmp(temp, "end") == 0) break;

                strcpy(lines[total++], line);
                enqueue(line, lineNum++);
            }
            checkSyntax(lines, total);
        }
        else if (choice == 2) {
            loadFile();
        }
        else if (choice == 3) {
            printf("Exiting program...\n");
        }
        else {
            printf("Invalid choice! Try again.\n");
        }

    } while (choice != 3);

    return 0;
}
