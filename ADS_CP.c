#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h> 

#define ALPHABET_SIZE 26
#define CHAR_TO_INDEX(c) (tolower(c) - 'a')


struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool isWordEnd;
    char *meaning; 
};


struct TrieNode *getNode(void) {
    struct TrieNode *pNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    pNode->isWordEnd = false;
    pNode->meaning = NULL;

    for (int i = 0; i < ALPHABET_SIZE; i++)
        pNode->children[i] = NULL;

    return pNode;
}

// Function to convert a string to lowercase
void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// If not present, inserts key into trie. If the key is a prefix of a trie node, just marks the leaf node
void insert(struct TrieNode *root, const char *key, const char *meaning) {
    char lowerKey[strlen(key) + 1];
    strcpy(lowerKey, key);
    toLowerCase(lowerKey);

    struct TrieNode *pCrawl = root;

    for (int level = 0; level < strlen(lowerKey); level++) {
        int index = CHAR_TO_INDEX(lowerKey[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();

        pCrawl = pCrawl->children[index];
    }

    // Mark the last node as a leaf
    pCrawl->isWordEnd = true;

    // Assign meaning to the word
    if (pCrawl->meaning) {
        free(pCrawl->meaning);
    }
    pCrawl->meaning = strdup(meaning);
}

// Returns 0 if the current node has a child. If all children are NULL, return 1.
bool isLastNode(struct TrieNode *root) {
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (root->children[i])
            return false;
    return true;
}

// Recursive function to print suggestions
void suggestionsRec(struct TrieNode *root, char *currPrefix) {
    if (root->isWordEnd) {
        printf("%s: %s\n", currPrefix, root->meaning);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            // Child node character value
            char child = 'a' + i;
            char newPrefix[strlen(currPrefix) + 2];
            strcpy(newPrefix, currPrefix);
            newPrefix[strlen(currPrefix)] = child;
            newPrefix[strlen(currPrefix) + 1] = '\0';
            suggestionsRec(root->children[i], newPrefix);
        }
    }
}

// Function to print suggestions for the given prefix.
int printAutoSuggestions(struct TrieNode *root, const char *query) {
    char lowerQuery[strlen(query) + 1];
    strcpy(lowerQuery, query);
    toLowerCase(lowerQuery);

    struct TrieNode *pCrawl = root;
    for (int i = 0; i < strlen(lowerQuery); i++) {
        int ind = CHAR_TO_INDEX(lowerQuery[i]);

        if (!pCrawl->children[ind]) {
            return 0; // No string found with this prefix
        }

        pCrawl = pCrawl->children[ind];
    }

    if (isLastNode(pCrawl)) {
        printf("%s: %s\n", query, pCrawl->meaning);
        return -1; // Exact match found
    }

    suggestionsRec(pCrawl, (char *)query);
    return 1;
}

// Read words and meanings from file and populate the trie
void readFromFile(struct TrieNode *root, const char *filename) {
    FILE *file = fopen(filename, "a+");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[1000];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Splitting line into word and meaning
        char *word = strtok(buffer, ":");
        char *meaning = strtok(NULL, ":");

        if (word && meaning) {
            // Removing newline character from meaning
            meaning[strcspn(meaning, "\n")] = '\0';

            // Convert word to lowercase before inserting
            toLowerCase(word);

            // Insert word and meaning into trie
            insert(root, word, meaning);
        }
    }

    fclose(file);
}

// Main function
int main() {
    struct TrieNode *root = getNode();

    // Read words and meanings from file and populate the trie
    readFromFile(root, "words.txt");

    char option[10];
    char query[100];
    char word[100];
    char meaning[1000];

    while (true) {
        printf("\nOptions:\n");
        printf("1. Search for a word\n");
        printf("2. Add a new word to the dictionary\n");
        printf("3. Exit\n");
        printf("Enter your choice (1, 2, or 3): ");
        scanf("%s", option);

        if (strcmp(option, "3") == 0) {
            printf("Exiting...\n");
            break;
        } else if (strcmp(option, "1") == 0) {
            // Search for a word
            printf("\nEnter the initial letters of the given string: ");
            scanf("%s", query);

            // Convert query to lowercase
            toLowerCase(query);

            int result = printAutoSuggestions(root, query);

            if (result == -1) {
                printf("Exact match found.\n");
            } else if (result == 0) {
                printf("No suggestions found.\n");
            }
        } else if (strcmp(option, "2") == 0) {
            // Add a new word to the dictionary
            printf("\nEnter the word you want to add: ");
            scanf("%s", word);
            printf("Enter the meaning of the word: ");
            getchar(); // Clear the newline character from the buffer
            fgets(meaning, sizeof(meaning), stdin);

            // Remove newline character from meaning
            meaning[strcspn(meaning, "\n")] = '\0';

            // Convert the word to lowercase
            toLowerCase(word);

            // Insert the word and meaning into the trie
            insert(root, word, meaning);

            printf("Word and meaning added to the dictionary.\n");
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
