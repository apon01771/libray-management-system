#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAX_BOOKS = 100;

struct Book {
    int id;
    char title[50];
    char author[50];
    int copies;
};

struct Issued {
    int bookId;
    char userName[50];
    struct Issued *next;
};

struct Request {
    int bookId;
    char userName[50];
    struct Request *next;
};

struct Book books[100];
int bookCount = 0;

struct Book deletedStack[100];
int top = -1;

struct Issued *issuedStart = NULL;

struct Request *front = NULL, *rear = NULL;

void processWaitingQueue(int bookId) {
    if (front != NULL && front->bookId == bookId) {
        struct Request *bookRequester = front;
        front = front->next;
        if (front == NULL) {
            rear = NULL;
        }


        printf("Book is now available for the next user in queue: %s\n", bookRequester->userName);

        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == bookId) {
                books[i].copies--;
                break;
            }
        }
        free(bookRequester);
    }
}


void addBook() {
    if (bookCount >= MAX_BOOKS) {
        printf("Library is full...:)Cannot add more books.\n");
        return;
    }
    struct Book b;
    printf("Enter Book ID: ");
    scanf("%d", &b.id);
    getchar();
    printf("Enter Title: ");
    fgets(b.title, 50, stdin);
    b.title[strcspn(b.title, "\n")] = '\0';
    printf("Enter Author: ");
    fgets(b.author, 50, stdin);
    b.author[strcspn(b.author, "\n")] = '\0';
    printf("Enter Copies: ");
    scanf("%d", &b.copies);
    books[bookCount++] = b;
    printf("Book Added Successfully!\n");
}

void showBooks() {
    if (bookCount == 0) {
        printf("No books found in the library.\n");
        return;
    }
    printf("\n--- Available Books ---\n");
    printf("ID\tTitle\t\tAuthor\t\tCopies\n");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < bookCount; i++) {
        printf("%d\t%-15s\t%-15s\t%d\n", books[i].id, books[i].title, books[i].author, books[i].copies);
    }
}

void deleteBook() {
    int id, i;
    printf("Enter Book ID to delete: ");
    scanf("%d", &id);

    for (i = 0; i < bookCount; i++) {
        if (books[i].id == id) {
            if (top < MAX_BOOKS - 1) {
                deletedStack[++top] = books[i];
            }
            for (int j = i; j < bookCount - 1; j++) {
                books[j] = books[j + 1];
            }
            bookCount--;
            printf("Book deleted and saved for Undo.\n");
            return;
        }
    }
    printf("Book with ID %d not found.\n", id);
}

void undoDelete() {
    if (top >= 0) {
        books[bookCount++] = deletedStack[top--];
        printf("Undo successful! Book has been restored.\n");
    } else {
        printf("Nothing to undo.\n");
    }
}

void issueBook() {
    int id;
    char name[50];
    printf("Enter Book ID to issue: ");
    scanf("%d", &id);
    getchar();
    printf("Enter your name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = '\0';

    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == id) {
            if (books[i].copies > 0) {
                books[i].copies--;
                struct Issued *newNode = (struct Issued*)malloc(sizeof(struct Issued));
                newNode->bookId = id;
                strcpy(newNode->userName, name);
                newNode->next = issuedStart;
                issuedStart = newNode;
                printf("Book issued successfully to %s.\n", name);
                return;
            } else {
                struct Request *newRequest = (struct Request*)malloc(sizeof(struct Request));
                newRequest->bookId = id;
                strcpy(newRequest->userName, name);
                newRequest->next = NULL;
                if (rear == NULL) {
                    front = rear = newRequest;
                } else {
                    rear->next = newRequest;
                    rear = newRequest;
                }
                printf("Book is not available. Sorry!!!!\n");
                return;
            }
        }
    }
    printf("Book with ID %d not found. Sorry!!!\n", id);
}

void returnBook() {
    int id;
    char name[50];
    printf("Enter Book ID to return: ");
    scanf("%d", &id);
    getchar();
    printf("Enter your name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = '\0';

    struct Issued *preview = NULL, *current = issuedStart;
    while (current != NULL) {
        if (current->bookId == id && strcmp(current->userName, name) == 0) {
            if (preview == NULL) {
                issuedStart = current->next;
            } else {
                preview->next = current->next;
            }
            free(current);
            for (int i = 0; i < bookCount; i++) {
                if (books[i].id == id) {
                    books[i].copies++;
                    break;
                }
            }
            printf("Book returned successfully!!!\n");

            processWaitingQueue(id);
            return;
        }
        preview = current;
        current = current->next;
    }
    printf("Return failed. You did not issue this book.\n");
}

void adminMenu() {
    int choice;
    do {
        printf("\n--- Admin Menu ---\n");
        printf("1. Add Book\n");
        printf("2. Show All Books\n");
        printf("3. Delete Book\n");
        printf("4. Undo Delete\n");
        printf("5. Back to Main Menu\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                showBooks();
                break;
            case 3:
                deleteBook();
                break;
            case 4:
                undoDelete();
                break;
            case 5:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 5);
}

void userMenu() {
    int choice;
    do {
        printf("\n--- User Menu ---\n");
        printf("1. Show All Books\n");
        printf("2. Issue Book\n");
        printf("3. Return Book\n");
        printf("4. Back to Main Menu\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                showBooks();
                break;
            case 2:
                issueBook();
                break;
            case 3:
                returnBook();
                break;
            case 4:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 4);
}

int main() {
    int choice;
    do {
        printf("\n======= Library Management System =======\n"
               "1. Admin Login\n"
               "2. User Login\n"
               "3. Exit\n"
               "Choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                adminMenu();
                break;
            case 2:
                userMenu();
                break;
            case 3:
                printf("Exiting the program. Tata!!!!\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 3);

    return 0;
}
