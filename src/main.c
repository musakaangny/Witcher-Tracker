#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/*
 * Witcher Tracker Implementation
 * CMPE230, Systems Programming, Spring 2025
 *
 * This program implements a command interpreter and inventory-event tracking system
 * for Geralt in the world of The Witcher. It handles input commands related to
 * alchemy ingredients, potions, monster encounters, and knowledge acquisition.
 */

#define MAX_INPUT_LENGTH 1024
#define MAX_TOKEN_LENGTH 1024
#define MAX_TOKENS 64
#define MAX_NAME_LENGTH 1024
#define MAX_ITEMS 1024
#define MAX_INGREDIENTS 1024
#define MAX_TROPHIES 1024
#define MAX_POTIONS 1024    
#define MAX_SIGNS 1024  
#define MAX_BEASTS 1024
#define MAX_EFFECTIVENESS 1024
#define MAX_POTION_INGREDIENTS 1024

// Command types
typedef enum {
    INVALID_COMMAND,
    ACTION_LOOT,
    ACTION_TRADE,
    ACTION_BREW,
    KNOWLEDGE_EFFECTIVENESS,
    KNOWLEDGE_POTION_FORMULA,
    ENCOUNTER,
    QUERY_SPECIFIC_INVENTORY,
    QUERY_ALL_INVENTORY,
    QUERY_BESTIARY,
    QUERY_ALCHEMY,
    EXIT_COMMAND
} CommandType;

// Item categories
typedef enum {
    CATEGORY_INGREDIENT,
    CATEGORY_POTION,
    CATEGORY_TROPHY
} ItemCategory;

bool isLootAction(const char* input);
bool isTradeAction(const char* input);
bool isBrewAction(const char* input);
bool isEffectivenessKnowledge(const char* input);
bool isPotionFormulaKnowledge(const char* input);
bool isEncounterSentence(const char* input);
bool isInventoryQuery(const char* input, bool* isSpecific);
bool isBestiaryQuery(const char* input);
bool isAlchemyQuery(const char* input);
bool isExitCommand(const char* input);
bool isValidCommand(const char* input, CommandType* cmdType);


int executeLootAction(const char* input);
int executeTradeAction(const char* input);
int executeBrewAction(const char* input);
int executeEffectivenessKnowledge(const char* input);
int executeFormulaKnowledge(const char* input);
int executeEncounter(const char* input);
int executeSpecificInventoryQuery(const char* input);
int executeAllInventoryQuery(const char* input);
int executeBestiaryQuery(const char* input);
int executeAlchemyQuery(const char* input);
int executeCommand(const char* input, CommandType cmdType);
int execute_line(const char* line);


    // Function to clean up the input line
void cleanInputLine(char* input) {
    // Remove trailing newline if present
    size_t len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
        len--;
    }
    
    // Trim leading spaces
    int start = 0;
    while (isspace((unsigned char)input[start])) start++;
    
    if (start > 0) {
        memmove(input, input + start, len - start + 1);
        len -= start;
    }
    
    // Trim trailing spaces
    while (len > 0 && isspace((unsigned char)input[len-1])) {
        input[--len] = '\0';
    }
}


int execute_line(const char* line) {
    char inputCopy[MAX_INPUT_LENGTH];
    strncpy(inputCopy, line, MAX_INPUT_LENGTH - 1);
    inputCopy[MAX_INPUT_LENGTH - 1] = '\0';
    
    // Clean up the input (trim spaces, etc.)
    cleanInputLine(inputCopy);
    
    if (strlen(inputCopy) == 0)
        return -1;
    
    // Check if the command follows valid grammar
    CommandType cmdType;
    if (isValidCommand(inputCopy, &cmdType)) {
        // Execute the command based on its type
        return executeCommand(inputCopy, cmdType);
    }
    
    return -1;
}

int main(void) {

    char line[MAX_INPUT_LENGTH];

    while (1) {
        printf(">> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        // Check for the exit command
        if (strcmp(line, "Exit\n") == 0 || strcmp(line, "Exit") == 0) {
            break;
        }

        // Execute the command and handle result
        int result = execute_line(line);
        if (result == -1) {
            printf("INVALID\n");
        }
    }

    return 0;
}


/**
 * @brief Validates a given input string and determines its corresponding command type.
 *
 * This function checks the input string against various possible command patterns,
 * such as actions (loot, trade, brew), knowledge queries (effectiveness, potion formula),
 * encounter statements, inventory and bestiary queries, alchemy questions, and exit commands.
 * 
 * If the input matches one of these command types, the corresponding `CommandType`
 * is set and the function returns true. Otherwise, it sets the type to `INVALID_COMMAND` and returns false.
 *
 * @param input The input command string to be validated.
 * @param cmdType Pointer to a CommandType variable where the detected command type will be stored.
 * 
 * @return true if the command is valid and recognized, false otherwise.
 */
bool isValidCommand(const char* input, CommandType* cmdType) {
    if (isLootAction(input)) {
        *cmdType = ACTION_LOOT;
        return true;
    } else if (isTradeAction(input)) {
        *cmdType = ACTION_TRADE;
        return true;
    } else if (isBrewAction(input)) {
        *cmdType = ACTION_BREW;
        return true;
    } else if (isEffectivenessKnowledge(input)) {
        *cmdType = KNOWLEDGE_EFFECTIVENESS;
        return true;
    } else if (isPotionFormulaKnowledge(input)) {
        *cmdType = KNOWLEDGE_POTION_FORMULA;
        return true;
    } else if (isEncounterSentence(input)) {
        *cmdType = ENCOUNTER;
        return true;
    } else if (isInventoryQuery(input, &(bool){false})) {
        bool isSpecific = false;
        isInventoryQuery(input, &isSpecific);
        *cmdType = isSpecific ? QUERY_SPECIFIC_INVENTORY : QUERY_ALL_INVENTORY;
        return true;
    } else if (isBestiaryQuery(input)) {
        *cmdType = QUERY_BESTIARY;
        return true;
    } else if (isAlchemyQuery(input)) {
        *cmdType = QUERY_ALCHEMY;
        return true;
    } else if (isExitCommand(input)) {
        *cmdType = EXIT_COMMAND;
        return true;
    }
    
    *cmdType = INVALID_COMMAND;
    return false;
}


/**
 * @brief Checks if the given token is a valid positive integer.
 *
 * A valid positive integer has no leading zeros (except "0" itself)
 * and consists only of digit characters.
 *
 * @param token The string to check.
 * @return true if the token is a positive integer, false otherwise.
 */
bool isPositiveInteger(const char* token) {
    if (token == NULL || *token == '\0') return false;
    
    // Check for leading zeros (except for a single "0")
    if (token[0] == '0' && token[1] != '\0') return false;
    
    // Check if all characters are digits
    for (int i = 0; token[i] != '\0'; i++) {
        if (!isdigit(token[i])) return false;
    }
    
    int value = atoi(token);
    return value > 0;
}

/**
 * @brief Checks if the given token contains only alphabetic characters.
 *
 * Returns true if all characters in the token are alphabetic (A-Z, a-z),
 * and the token is not NULL or empty.
 *
 * @param token The string to validate.
 * @return true if the token is alphabetic only, false otherwise.
 */
bool isAlphabeticOnly(const char* token) {
    if (token == NULL || *token == '\0') return false;
    
    for (int i = 0; token[i] != '\0'; i++) {
        if (!isalpha(token[i])) return false;
    }
    
    return true;
}

/**
 * @brief Tokenizes the input string into an array of tokens.
 *
 * This function splits the input string into tokens based on whitespace and
 * specific keywords. It handles various command formats, including alchemy
 * queries, bestiary queries, and potion names.
 *
 * @param input The input string to tokenize.
 * @param tokens The array to store the resulting tokens.
 * @return The number of tokens parsed from the input string.
 */
int tokenizeInput(const char* input, char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH]) {
    int count = 0;
    int inputLen = strlen(input);
    int i = 0;

    // Skip leading whitespace
    while (i < inputLen && isspace(input[i])) i++;

    // Handle question queries
    if (i < inputLen && strncmp(input + i, "What", 4) == 0 && (isspace(input[i+4]) || input[i+4] == '\0')) {
        strcpy(tokens[0], "What");
        count = 1;
        i += 4;
        while (i < inputLen && isspace(input[i])) i++;

        // Check for "is in" (alchemy query) or "is effective against" (bestiary query)
        if (i < inputLen && strncmp(input + i, "is", 2) == 0 && (isspace(input[i+2]) || input[i+2] == '\0')) {
            
            strcpy(tokens[count++], "is");
            i += 2;
            while (i < inputLen && isspace(input[i])) i++;
            
            // Check for "in" (alchemy query)
            if (i < inputLen && strncmp(input + i, "in", 2) == 0 && (isspace(input[i+2]) || input[i+2] == '\0')) {
                
                strcpy(tokens[count++], "in");
                i += 2;
                while (i < inputLen && isspace(input[i])) i++;
                
                // Read potion name
                int potionStart = i;
                int potionEnd = -1;
                
                for (int j = i; j < inputLen; j++) {
                    if (input[j] == '?') {
                        potionEnd = j;
                        break;
                    }
                }
                
                if (potionEnd == -1) {

                    potionEnd = inputLen;
                }
                
                while (potionEnd > potionStart && isspace(input[potionEnd - 1])) {
                    potionEnd--;
                }
                
                int potionLen = potionEnd - potionStart;
                if (potionLen >= MAX_TOKEN_LENGTH) return 0;
                
                strncpy(tokens[count], input + potionStart, potionLen);
                tokens[count][potionLen] = '\0';
                count++;
                
                // find question mark
                while (i < inputLen && input[i] != '?') i++;
                if (i < inputLen && input[i] == '?') {
                    tokens[count][0] = '?';
                    tokens[count][1] = '\0';
                    count++;
                    i++; 
                    
                    // Continue parsing after question mark to check for commas or other tokens
                    while (i < inputLen && count < MAX_TOKENS) {
                        while (i < inputLen && isspace(input[i])) i++;
                        if (i >= inputLen) break;
                        
                        // Handle comma or any other character as a separate token
                        if (input[i] == ',') {
                            tokens[count][0] = ',';
                            tokens[count][1] = '\0';
                            count++;
                            i++; 
                            continue;
                        }
                        
                        // Handle other tokens
                        int tokenStart = i;
                        while (i < inputLen && !isspace(input[i]) && input[i] != ',') i++;
                        int tokenLen = i - tokenStart;
                        if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
                            strncpy(tokens[count], input + tokenStart, tokenLen);
                            tokens[count][tokenLen] = '\0';
                            count++;
                        }
                    }
                }
                
                return count;
            }
            // Check for "effective against" (bestiary query)
            else if (i < inputLen && strncmp(input + i, "effective", 9) == 0 &&
                    (isspace(input[i+9]) || input[i+9] == '\0')) {
                
                strcpy(tokens[count++], "effective");
                i += 9;
                while (i < inputLen && isspace(input[i])) i++;
                
                if (i < inputLen && strncmp(input + i, "against", 7) == 0 &&
                    (isspace(input[i+7]) || input[i+7] == '\0')) {
                    
                    strcpy(tokens[count++], "against");
                    i += 7;
                    while (i < inputLen && isspace(input[i])) i++;
                    
                    // Read monster name 
                    int monsterStart = i;
                    int monsterEnd = -1;
                    
                    for (int j = i; j < inputLen; j++) {
                        if (input[j] == '?') {
                            monsterEnd = j;
                            break;
                        }
                    }
                    
                    if (monsterEnd == -1) {
                        monsterEnd = inputLen;
                    }
        
                    while (monsterEnd > monsterStart && isspace(input[monsterEnd - 1])) {
                        monsterEnd--;
                    }
                    
                    int monsterLen = monsterEnd - monsterStart;
                    if (monsterLen >= MAX_TOKEN_LENGTH) return 0;
                    
                    strncpy(tokens[count], input + monsterStart, monsterLen);
                    tokens[count][monsterLen] = '\0';
                    count++;
                    
                    // find question mark
                    while (i < inputLen && input[i] != '?') i++;
                    if (i < inputLen && input[i] == '?') {
                        tokens[count][0] = '?';
                        tokens[count][1] = '\0';
                        count++;
                        i++;
                        
                        while (i < inputLen && count < MAX_TOKENS) {
                            while (i < inputLen && isspace(input[i])) i++;
                            if (i >= inputLen) break;
                            
                            // Handle comma or any other character as a separate token
                            if (input[i] == ',') {
                                tokens[count][0] = ',';
                                tokens[count][1] = '\0';
                                count++;
                                i++; 
                                continue;
                            }
                            
                            // Handle other tokens
                            int tokenStart = i;
                            while (i < inputLen && !isspace(input[i]) && input[i] != ',') i++;
                            int tokenLen = i - tokenStart;
                            if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
                                strncpy(tokens[count], input + tokenStart, tokenLen);
                                tokens[count][tokenLen] = '\0';
                                count++;
                            }
                        }
                    }
                    
                    return count;
                }
            }
        }

        const char* expected[] = { "is", "in" };
        for (int j = 0; j < 2 && count < MAX_TOKENS; j++) {
            int len = strlen(expected[j]);
            if (strncmp(input + i, expected[j], len) == 0 &&
                (isspace(input[i + len]) || input[i + len] == '\0')) {
                strcpy(tokens[count++], expected[j]);
                i += len;
                while (i < inputLen && isspace(input[i])) i++;
            } else {
                return 0;
            }
        }

        // Read potion name
        int potionStart = i;
        int potionEnd = -1;

        for (int j = i; j < inputLen; j++) {
            if (input[j] == '?') {
                potionEnd = j;
                break;
            }
        }

        if (potionEnd == -1) {
            potionEnd = inputLen;
        }

        while (potionEnd > potionStart && isspace(input[potionEnd - 1])) {
            potionEnd--;
        }

        int potionLen = potionEnd - potionStart;
        if (potionLen >= MAX_TOKEN_LENGTH) return 0;

        strncpy(tokens[count], input + potionStart, potionLen);
        tokens[count][potionLen] = '\0';
        count++;

        // find question mark
        while (i < inputLen && input[i] != '?') i++;
        if (i < inputLen && input[i] == '?') {
            tokens[count][0] = '?';
            tokens[count][1] = '\0';
            count++;
            i++;
            
            // Continue parsing after question mark to check for commas or other tokens
            while (i < inputLen && count < MAX_TOKENS) {
                while (i < inputLen && isspace(input[i])) i++;
                if (i >= inputLen) break;
                
                // Handle comma or any other character as a separate token
                if (input[i] == ',') {
                    tokens[count][0] = ',';
                    tokens[count][1] = '\0';
                    count++;
                    i++;
                    continue;
                }
                
                // Handle other tokens
                int tokenStart = i;
                while (i < inputLen && !isspace(input[i]) && input[i] != ',') i++;
                int tokenLen = i - tokenStart;
                if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
                    strncpy(tokens[count], input + tokenStart, tokenLen);
                    tokens[count][tokenLen] = '\0';
                    count++;
                }
            }
        }

        return count;
    }

    // Handle Total queries
    if (i < inputLen && strncmp(input + i, "Total", 5) == 0 && (isspace(input[i+5]) || input[i+5] == '\0')) {

        strcpy(tokens[0], "Total");
        count = 1;
        i += 5;
        while (i < inputLen && isspace(input[i])) i++;

        // Read category: potion / ingredient / trophy
        char category[MAX_TOKEN_LENGTH];
        int catStart = i;
        while (i < inputLen && !isspace(input[i]) && input[i] != '?') i++;
        int catLen = i - catStart;
        if (catLen >= MAX_TOKEN_LENGTH) return 0;

        strncpy(category, input + catStart, catLen);
        category[catLen] = '\0';
        strcpy(tokens[count++], category);

        while (i < inputLen && isspace(input[i])) i++;

        // If the next character is '?', add it as a separate token
        if (i < inputLen && input[i] == '?') {
            tokens[count][0] = '?';
            tokens[count][1] = '\0';
            count++;
            i++;
            
            // Continue parsing after question mark to check for commas or other tokens
            while (i < inputLen && count < MAX_TOKENS) {
                while (i < inputLen && isspace(input[i])) i++;
                if (i >= inputLen) break;
                
                // Handle comma or any other character as a separate token
                if (input[i] == ',') {
                    tokens[count][0] = ',';
                    tokens[count][1] = '\0';
                    count++;
                    i++; 
                    continue;
                }
                
                // Handle other tokens
                int tokenStart = i;
                while (i < inputLen && !isspace(input[i]) && input[i] != ',') i++;
                int tokenLen = i - tokenStart;
                if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
                    strncpy(tokens[count], input + tokenStart, tokenLen);
                    tokens[count][tokenLen] = '\0';
                    count++;
                }
            }
            
            return count;
        }

        // Return early if no more input
        if (i >= inputLen) return count;

        while (i < inputLen && isspace(input[i])) i++;

        // Check if it's "Total <category> ?" or "Total <category> <item> ?"
        if (i >= inputLen || input[i] == '?') {
            tokens[count][0] = '?';
            tokens[count][1] = '\0';
            count++;
            i++; 
            
            // Continue parsing after question mark to check for commas or other tokens
            while (i < inputLen && count < MAX_TOKENS) {
                while (i < inputLen && isspace(input[i])) i++;
                if (i >= inputLen) break;
                
                // Handle comma or any other character as a separate token
                if (input[i] == ',') {
                    tokens[count][0] = ',';
                    tokens[count][1] = '\0';
                    count++;
                    i++; 
                    continue;
                }
                
                // Handle other tokens
                int tokenStart = i;
                while (i < inputLen && !isspace(input[i]) && input[i] != ',') i++;
                int tokenLen = i - tokenStart;
                if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
                    strncpy(tokens[count], input + tokenStart, tokenLen);
                    tokens[count][tokenLen] = '\0';
                    count++;
                }
            }
            
            return count;
        }

        // Read item name
        int itemStart = i;
        int itemEnd = -1;
        for (int j = i; j < inputLen; j++) {
            if (input[j] == '?') {
                itemEnd = j;
                break;
            }
        }
        if (itemEnd == -1) itemEnd = inputLen;

        // Trim trailing spaces
        while (itemEnd > itemStart && isspace(input[itemEnd - 1])) itemEnd--;

        // Check if the last word ends with '?'
        if (input[itemEnd - 1] == '?' && itemEnd - itemStart < MAX_TOKEN_LENGTH) {
            // Check if it's something like "trophy?" with no space
            int wordLen = itemEnd - itemStart - 1;
            strncpy(tokens[count], input + itemStart, wordLen);
            tokens[count][wordLen] = '\0';
            count++;

            tokens[count][0] = '?';
            tokens[count][1] = '\0';
            count++;
            i = itemEnd; // Update position
            
            // Continue parsing after question mark to check for commas or other tokens
            while (i < inputLen && count < MAX_TOKENS) {
                while (i < inputLen && isspace(input[i])) i++;
                if (i >= inputLen) break;
                
                // Handle comma or any other character as a separate token
                if (input[i] == ',') {
                    tokens[count][0] = ',';
                    tokens[count][1] = '\0';
                    count++;
                    i++; 
                    continue;
                }
                
                // Handle other tokens
                int tokenStart = i;
                while (i < inputLen && !isspace(input[i]) && input[i] != ',') i++;
                int tokenLen = i - tokenStart;
                if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
                    strncpy(tokens[count], input + tokenStart, tokenLen);
                    tokens[count][tokenLen] = '\0';
                    count++;
                }
            }
        } else {
            int itemLen = itemEnd - itemStart;
            strncpy(tokens[count], input + itemStart, itemLen);
            tokens[count][itemLen] = '\0';
            count++;

            // Add '?' if present
            while (i < inputLen && input[i] != '?') i++;
            if (i < inputLen && input[i] == '?') {
                tokens[count][0] = '?';
                tokens[count][1] = '\0';
                count++;
                i++; 
                
                // Continue parsing after question mark to check for commas or other tokens
                while (i < inputLen && count < MAX_TOKENS) {
                    // Skip spaces
                    while (i < inputLen && isspace(input[i])) i++;
                    if (i >= inputLen) break;
                    
                    // Handle comma or any other character as a separate token
                    if (input[i] == ',') {
                        tokens[count][0] = ',';
                        tokens[count][1] = '\0';
                        count++;
                        i++; 
                        continue;
                    }
                    
                    // Handle other tokens
                    int tokenStart = i;
                    while (i < inputLen && !isspace(input[i]) && input[i] != ',') i++;
                    int tokenLen = i - tokenStart;
                    if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
                        strncpy(tokens[count], input + tokenStart, tokenLen);
                        tokens[count][tokenLen] = '\0';
                        count++;
                    }
                }
            }
        }

        return count;
    }
    // Skip leading whitespace
    while (i < inputLen && isspace(input[i])) i++;

    // Check for "Geralt"
    if (i < inputLen && strncmp(input + i, "Geralt", 6) == 0 && 
        (isspace(input[i+6]) || input[i+6] == '\0')) {
        strcpy(tokens[0], "Geralt");
        count = 1;
        i += 6;

        // Skip whitespace after "Geralt"
        while (i < inputLen && isspace(input[i])) i++;

        // Check for "brews" command
        if (i < inputLen && strncmp(input + i, "brews", 5) == 0 && 
            (isspace(input[i+5]) || input[i+5] == '\0')) {
            strcpy(tokens[1], "brews");
            count = 2;
            i += 5;

            // Skip whitespace
            while (i < inputLen && isspace(input[i])) i++;

            // Capture potion name
            int tokenIdx = 0;
            while (i < inputLen && tokenIdx < MAX_TOKEN_LENGTH - 1) {
                tokens[2][tokenIdx++] = input[i++];
            }
            tokens[2][tokenIdx] = '\0';

            if (tokenIdx > 0) {
                count = 3;
            }
            return count;
        }
        // Check for "learns" command
        else if (i < inputLen && strncmp(input + i, "learns", 6) == 0 && 
        (isspace(input[i+6]) || input[i+6] == '\0')) {
        strcpy(tokens[1], "learns");
        count = 2;
        i += 6;

        // Skip whitespace
        while (i < inputLen && isspace(input[i])) i++;

        // Store the position after "learns" command to restrict searching to this range
        int learnsStartPos = i;

        // Peek ahead to find "sign" or "potion"
        int wordStart = i, wordEnd = i;
        char tempTokens[MAX_NAME_LENGTH][MAX_TOKEN_LENGTH]; 
        int tempCount = 0;

        while (i < inputLen && tempCount < MAX_NAME_LENGTH) {
        while (i < inputLen && isspace(input[i])) i++;
        if (i >= inputLen) break;

        wordStart = i;
        while (i < inputLen && !isspace(input[i])) i++;
        wordEnd = i;

        int wordLen = wordEnd - wordStart;
        if (wordLen <= 0) break;

        strncpy(tempTokens[tempCount], input + wordStart, wordLen);
        tempTokens[tempCount][wordLen] = '\0';
        tempCount++;

        if (strcmp(tempTokens[tempCount - 1], "sign") == 0 ||
            strcmp(tempTokens[tempCount - 1], "potion") == 0) {

            bool isPotion = strcmp(tempTokens[tempCount - 1], "potion") == 0;
            const char* type = isPotion ? "potion" : "sign";

            if (tempCount >= 2) {
                // Find the start of the name by going from learnsStartPos
                // Instead of using strstr, we'll use the positions we already know
                int rawStart = learnsStartPos;
                int rawEnd = wordStart;
                int nameLen = rawEnd - rawStart;

                while (isspace(input[rawStart]) && nameLen > 0) {
                    rawStart++;
                    nameLen--;
                }
                while (nameLen > 0 && isspace(input[rawStart + nameLen - 1])) {
                    nameLen--;
                }

                if (nameLen <= 0 || nameLen >= MAX_TOKEN_LENGTH) return 0;

                strncpy(tokens[count], input + rawStart, nameLen);
                tokens[count][nameLen] = '\0';
                count++;

                strcpy(tokens[count++], type);

                // Check if "is effective against"
                while (i < inputLen && isspace(input[i])) i++;
                
                // Check for "is effective against" 
                if (i < inputLen && strncmp(input + i, "is", 2) == 0 && 
                    (isspace(input[i+2]) || input[i+2] == '\0')) {
                    strcpy(tokens[count++], "is");
                    i += 2;
                    
                    // Skip whitespace
                    while (i < inputLen && isspace(input[i])) i++;
                    
                    // Check for "effective"
                    if (i < inputLen && strncmp(input + i, "effective", 9) == 0 && 
                        (isspace(input[i+9]) || input[i+9] == '\0')) {
                        strcpy(tokens[count++], "effective");
                        i += 9;
                        
                        // Skip whitespace
                        while (i < inputLen && isspace(input[i])) i++;
                        
                        // Check for "against"
                        if (i < inputLen && strncmp(input + i, "against", 7) == 0 && 
                            (isspace(input[i+7]) || input[i+7] == '\0')) {
                            strcpy(tokens[count++], "against");
                            i += 7;
                            
                            // Skip whitespace
                            while (i < inputLen && isspace(input[i])) i++;
                            
                            // Capture monster name
                            int start = i;
                            while (i < inputLen) i++;
                            int len = i - start;
                            
                            if (len > 0 && len < MAX_TOKEN_LENGTH) {
                                strncpy(tokens[count], input + start, len);
                                tokens[count][len] = '\0';
                                count++;
                            }
                            
                            return count;
                        }
                    }
                }

                // Check for "consists of"
                i = wordEnd; // Reset position to after sign/potion
                while (i < inputLen && isspace(input[i])) i++;
                
                if (i < inputLen && strncmp(input + i, "consists", 8) == 0 && 
                    (isspace(input[i+8]) || input[i+8] == '\0')) {
                    strcpy(tokens[count++], "consists");
                    i += 8;
                    
                    // Skip whitespace
                    while (i < inputLen && isspace(input[i])) i++;
                    
                    // Check for "of"
                    if (i < inputLen && strncmp(input + i, "of", 2) == 0 && 
                        (isspace(input[i+2]) || input[i+2] == '\0')) {
                        strcpy(tokens[count++], "of");
                        i += 2;
                        
                        // Parse structured ingredient list
                        while (i < inputLen && count < MAX_TOKENS) {
                            while (i < inputLen && isspace(input[i])) i++;
                            if (i >= inputLen) break;
                            
                            if (isdigit(input[i])) {
                                // Handle quantity
                                int numStart = i;
                                while (i < inputLen && isdigit(input[i])) i++;
                                int numLen = i - numStart;

                                if (numLen > 0 && numLen < MAX_TOKEN_LENGTH) {
                                    strncpy(tokens[count], input + numStart, numLen);
                                    tokens[count][numLen] = '\0';
                                    count++;
                                }

                                // Skip any spaces
                                while (i < inputLen && isspace(input[i])) i++;

                                // Handle ingredient name
                                int nameStart = i;
                                while (i < inputLen && !isspace(input[i]) && input[i] != ',') i++;
                                int nameLen = i - nameStart;

                                if (nameLen > 0 && nameLen < MAX_TOKEN_LENGTH) {
                                    strncpy(tokens[count], input + nameStart, nameLen);
                                    tokens[count][nameLen] = '\0';
                                    count++;
                                }

                                // Skip any spaces
                                while (i < inputLen && isspace(input[i])) i++;

                                // Handle comma
                                if (i < inputLen && input[i] == ',') {
                                    tokens[count][0] = ',';
                                    tokens[count][1] = '\0';
                                    count++;
                                    i++;
             
                                }
                            } else {
                                // Handle non-digit start
                                int wordStart = i;
                                while (i < inputLen && !isspace(input[i]) && input[i] != ',') i++;
                                int wordLen = i - wordStart;
                                
                                if (wordLen > 0 && wordLen < MAX_TOKEN_LENGTH) {
                                    strncpy(tokens[count], input + wordStart, wordLen);
                                    tokens[count][wordLen] = '\0';
                                    count++;
                                }
                                
                                // Skip any spaces
                                while (i < inputLen && isspace(input[i])) i++;
                                
                                // Handle comma if present
                                if (i < inputLen && input[i] == ',') {
                                    tokens[count][0] = ',';
                                    tokens[count][1] = '\0';
                                    count++;
                                    i++; // Skip past the comma
                                    
                                    // No need to check for spaces after comma
                                }
                            }
                        }
                        
                        return count;
                    }
                }
            }
            return 0;
        }
        }

        return 0; // fallback if neither potion nor sign
        }
        // Check for "trades" command 
        else if (i < inputLen && strncmp(input + i, "trades", 6) == 0 && 
                (isspace(input[i+6]) || input[i+6] == '\0')) {
            strcpy(tokens[1], "trades");
            count = 2;
            i += 6;
            
            // Process the rest as generic tokens, making sure to handle commas properly
            while (i < inputLen && count < MAX_TOKENS) {
                // Skip spaces
                while (i < inputLen && isspace(input[i])) i++;
                if (i >= inputLen) break;
                
                // Handle comma as separate token
                if (input[i] == ',') {
                    tokens[count][0] = ',';
                    tokens[count][1] = '\0';
                    count++;
                    i++; // Skip past comma
                    continue;
                }
                
                // Handle regular token
                int tokenStart = i;
                while (i < inputLen && input[i] != ',' && !isspace(input[i])) i++;
                
                int tokenLen = i - tokenStart;
                if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
                    strncpy(tokens[count], input + tokenStart, tokenLen);
                    tokens[count][tokenLen] = '\0';
                    count++;
                }
            }
            
            return count;
        }
    }

    // Generic fallback
    // Reset index since we may have partially matched above
    i = 0;
    count = 0;
    
    while (i < inputLen && count < MAX_TOKENS) {
        // Skip spaces
        while (i < inputLen && isspace(input[i])) i++;
        if (i >= inputLen) break;

        // Handle comma as separate token
        if (input[i] == ',') {
            tokens[count][0] = ',';
            tokens[count][1] = '\0';
            count++;
            i++; // Skip past comma
            continue; // Continue to next token without checking for spaces
        }

        // Handle regular token
        int tokenStart = i;
        while (i < inputLen && input[i] != ',' && !isspace(input[i])) i++;

        int tokenLen = i - tokenStart;
        if (tokenLen > 0 && tokenLen < MAX_TOKEN_LENGTH) {
            strncpy(tokens[count], input + tokenStart, tokenLen);
            tokens[count][tokenLen] = '\0';
            count++;
        }
    }

    return count;
}



/**
 * @brief Checks for a comma spacing error in the token list.
 *
 * Scans the token array for the special marker "ERROR_NO_SPACE_AFTER_COMMA",
 * which indicates a missing space after a comma in the original input.
 *
 * @param tokens The array of token strings.
 * @param count The number of tokens in the array.
 * @return true if a comma spacing error is found, false otherwise.
 */
bool hasCommaSpacingError(const char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(tokens[i], "ERROR_NO_SPACE_AFTER_COMMA") == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Checks if the input string is a valid loot action.
 *
 * A loot action is defined as "Geralt loots <quantity> <ingredient> [, <quantity> <ingredient>]...".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @return true if the input is a valid loot action, false otherwise.
 */
bool isLootAction(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    // Check for comma spacing errors
    if (hasCommaSpacingError(tokens, count)) {
        return false;
    }
    
    // Minimum tokens for a loot action: "Geralt", "loots", quantity, ingredient
    if (count < 4) return false;
    
    // Check first two tokens
    if (strcmp(tokens[0], "Geralt") != 0 || strcmp(tokens[1], "loots") != 0)
        return false;
    
    // Check pattern of: quantity, ingredient [, quantity, ingredient]...
    for (int i = 2; i < count; i += 3) {
        // Check quantity is a positive integer
        if (!isPositiveInteger(tokens[i]))
            return false;
        
        // Check ingredient is alphabetic
        if (i + 1 >= count || !isAlphabeticOnly(tokens[i + 1]))
            return false;
        
        // If there are more tokens, check for comma
        if (i + 2 < count) {
            if (strcmp(tokens[i + 2], ",") != 0)
                return false;
        }
    }
    
    // Check if the last token is a comma
    if (count > 0 && strcmp(tokens[count-1], ",") == 0) {
        return false;
    }
    
    return true;
}

/**
 * @brief Checks if the input string is a valid trade action.
 *
 * A trade action is defined as "Geralt trades <quantity> <trophy> [, <quantity> <trophy>]... for <quantity> <ingredient> [, <quantity> <ingredient>]...".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @return true if the input is a valid trade action, false otherwise.
 */
bool isTradeAction(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);

    // Check first two tokens
    if (strcmp(tokens[0], "Geralt") != 0 || strcmp(tokens[1], "trades") != 0)
        return false;

    
    // Find "for"
    int forIndex = -1;
    for (int i = 2; i < count; i++) {
        if (strcmp(tokens[i], "for") == 0) {
            forIndex = i;
            break;
        }
    }

    if (forIndex == -1 || forIndex <= 4 || forIndex >= count - 2) return false;

    // Trophy list validation: from index 2 to forIndex - 1
    int i = 2;
    bool expectingQuantity = true;
    bool lastTrophyHasKeyword = false;

    while (i < forIndex) {
        // Quantity
        if (expectingQuantity) {
            if (!isPositiveInteger(tokens[i])) return false;
            i++;
            expectingQuantity = false;
        }

        // Monster name
        else if (!isAlphabeticOnly(tokens[i])) {
            return false;
        } else {
            i++;
            // If we're at the last trophy pair
            if (i < forIndex && strcmp(tokens[i], "trophy") == 0 && i + 1 == forIndex) {
                lastTrophyHasKeyword = true;
                i++;
            } else if (i < forIndex && strcmp(tokens[i], ",") == 0) {
                i++;
                expectingQuantity = true;
            } else {
                // Must either be 'trophy' at end or comma between entries
                return false;
            }
        }
    }

    if (!lastTrophyHasKeyword) return false;

    // Ingredient list after "for"
    i = forIndex + 1;
    expectingQuantity = true;

    while (i < count) {
        // Check for quantity
        if (expectingQuantity) {
            if (!isPositiveInteger(tokens[i])) return false;
            i++;
            expectingQuantity = false;
            
            // If we hit the end after a number, it's invalid
            if (i == count) return false;
        } 
        // Check for ingredient name
        else if (!isAlphabeticOnly(tokens[i])) {
            return false;
        } else {
            i++;
            // Now we have a complete quantity-ingredient pair
            
            // If we've reached the end, we're good
            if (i == count) {
                break;
            }
            
            if (strcmp(tokens[i], ",") == 0) {
                i++;
                expectingQuantity = true;
                
                // Check if this comma is the last token (trailing comma)
                if (i == count) {
                    return false;
                }
            } else {
                return false;
            }
        }
    }

    // Make sure we're not expecting another quantity-ingredient pair
    if (expectingQuantity) {
        return false;
    }

    return true;
}

/**
 * @brief Checks if the input string is a valid brew action.
 *
 * A brew action is defined as "Geralt brews <potion_name>".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @return true if the input is a valid brew action, false otherwise.
 */
bool isBrewAction(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Minimum tokens: "Geralt", "brews", potion_name
    if (count < 3) return false;
    
    // Check first two tokens
    if (strcmp(tokens[0], "Geralt") != 0 || strcmp(tokens[1], "brews") != 0)
        return false;
    
    // Check if the potion name contains only alphabetic characters and spaces
    char* potionName = tokens[2];
    char* p = potionName;
    
    bool lastWasSpace = false;

    while (*p) {
        if (*p == ' ') {
            if (lastWasSpace) // two consecutive spaces
                return false;
            lastWasSpace = true;
        } else if (!isalpha(*p)) {
            return false;
        } else {
            lastWasSpace = false;
        }
        p++;
    }
    
    return true;
}
/**
 * @brief Checks if the input string is a valid potion name token.
 *
 * A valid potion name token consists of alphabetic characters and spaces,
 * but cannot have two consecutive spaces.
 *
 * @param token The input string to check.
 * @return true if the input is a valid potion name token, false otherwise.
 */

bool isValidPotionNameToken(const char* token) {
    const char* p = token;
    bool lastWasSpace = false;

    if (*p == '\0') return false;  // Empty

    while (*p) {
        if (*p == ' ') {
            if (lastWasSpace) // two consecutive spaces
                return false;
            lastWasSpace = true;
        } else if (!isalpha(*p)) {
            return false;
        } else {
            lastWasSpace = false;
        }
        p++;
    }

    return true;
}

/**
 * @brief Checks if the input string is a valid effectiveness knowledge statement.
 *
 * An effectiveness knowledge statement is defined as "Geralt learns <item_name> potion/sign is effective against <monster_name>".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @return true if the input is a valid effectiveness knowledge statement, false otherwise.
 */
bool isEffectivenessKnowledge(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);


    // Must start with Geralt learns
    if (count < 5 || strcmp(tokens[0], "Geralt") != 0 || strcmp(tokens[1], "learns") != 0)
        return false;

    // Look for pattern: <item> "potion"/"sign" "is" "effective" "against" <monster>
    const char* type = tokens[3]; // "potion" or "sign"
    const char* itemName = tokens[2];

    if (!(strcmp(type, "potion") == 0 || strcmp(type, "sign") == 0))
        return false;

    if (strcmp(tokens[4], "is") != 0 ||
        strcmp(tokens[5], "effective") != 0 ||
        strcmp(tokens[6], "against") != 0)
        return false;

    // Monster name should exist and be alphabetical
    if (count != 8 || !isAlphabeticOnly(tokens[7]))
        return false;

    // Validate sign name or potion name
    if (strcmp(type, "sign") == 0) {
        // Sign must be one word and alphabetic
        return isAlphabeticOnly(itemName);
    } else {
        // Potion name may include spaces, validate structure
        return isValidPotionNameToken(itemName);
    }
}

/**
 * @brief Checks if the input string is a valid potion formula knowledge statement.
 *
 * A potion formula knowledge statement is defined as "Geralt learns <potion_name> potion consists of <quantity> <ingredient> [, <quantity> <ingredient>]...".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @return true if the input is a valid potion formula knowledge statement, false otherwise.
 */

bool isPotionFormulaKnowledge(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    if (count < 7) return false;

    // Check for comma spacing issues
    if (hasCommaSpacingError(tokens, count)) return false;
    
    // Check if the last token is a comma - this is invalid
    if (count > 0 && strcmp(tokens[count-1], ",") == 0) {
        return false;
    }

    // Check the beginning
    if (strcmp(tokens[0], "Geralt") != 0 || strcmp(tokens[1], "learns") != 0)
        return false;

    // Find indexes
    int potionIndex = -1, consistsIndex = -1, ofIndex = -1;
    for (int i = 2; i < count; i++) {
        if (potionIndex == -1 && strcmp(tokens[i], "potion") == 0)
            potionIndex = i;
        else if (consistsIndex == -1 && strcmp(tokens[i], "consists") == 0)
            consistsIndex = i;
        else if (ofIndex == -1 && strcmp(tokens[i], "of") == 0)
            ofIndex = i;
    }

    // Ensure all are found
    if (potionIndex == -1 || consistsIndex == -1 || ofIndex == -1)
        return false;

    // Ensure order
    if (!(1 < potionIndex && potionIndex < consistsIndex && consistsIndex < ofIndex))
        return false;

    // Potion name should be alphabetic-only and come right after "learns" and before "potion"
    for (int i = 2; i < potionIndex; i++) {
        if (!isValidPotionNameToken(tokens[i])) return false;
    }

    // Ensure "potion consists of"
    if (!(potionIndex + 1 == consistsIndex && consistsIndex + 1 == ofIndex))
        return false;

    // Ingredient list: after "of"
    int i = ofIndex + 1;
    bool expectingIngredient = false;
    
    while (i < count) {
        // Quantity
        if (i >= count || !isPositiveInteger(tokens[i])) return false;
        i++;

        // Ingredient
        if (i >= count || !isAlphabeticOnly(tokens[i])) return false;
        i++;

        // Optional comma
        if (i < count) {
            if (strcmp(tokens[i], ",") == 0) {
                i++;
                // If comma is the last token, that's invalid
                if (i == count) return false;
            } else {
                // If there's something else instead of comma, it must be the last item
                if ((count - i) >= 1) return false;
            }
        }
    }

    return true;
}


/**
 * @brief Checks if the input string is a valid encounter sentence.
 *
 * An encounter sentence is defined as "Geralt encounters a <monster_name>".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @return true if the input is a valid encounter sentence, false otherwise.
 */

bool isEncounterSentence(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Required pattern: "Geralt", "encounters", "a", <monster>
    if (count != 4) return false;
    
    if (strcmp(tokens[0], "Geralt") != 0 || 
        strcmp(tokens[1], "encounters") != 0 || 
        strcmp(tokens[2], "a") != 0)
        return false;
    
    // Validate monster name is alphabetic
    if (!isAlphabeticOnly(tokens[3]))
        return false;
    
    return true;
}


/**
 * @brief Checks if the input string is a valid inventory query.
 *
 * An inventory query is defined as "Total <category> [<item_name>] ?".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @param isSpecific Pointer to a boolean that will be set to true if the query is specific.
 * @return true if the input is a valid inventory query, false otherwise.
 */

bool isInventoryQuery(const char* input, bool* isSpecific) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);

    if (count < 3 || count > 4) return false;

    // Make sure the last token is "?"
    if (strcmp(tokens[count - 1], "?") != 0)
        return false;

    // Must start with "Total"
    if (strcmp(tokens[0], "Total") != 0)
        return false;

    // Category must be one of: ingredient, potion, trophy
    if (strcmp(tokens[1], "ingredient") != 0 &&
        strcmp(tokens[1], "potion") != 0 &&
        strcmp(tokens[1], "trophy") != 0)
        return false;

    // Specific query if there's an item name before the "?"
    *isSpecific = (count == 4);  // Total + category + name + '?'

    if (*isSpecific) {
        if (strcmp(tokens[1], "ingredient") == 0 || strcmp(tokens[1], "trophy") == 0) {
            if(!isAlphabeticOnly(tokens[2])) {
                return false;
            }
        } else if (strcmp(tokens[1], "potion") == 0) {

            if (!isValidPotionNameToken(tokens[2])) {
                return false;
            }
        }
    }

    // All checks passed — it's a valid inventory query
    return true;
}



/**
 * @brief Checks if the input string is a valid bestiary query.
 *
 * A bestiary query is defined as "What is effective against <monster_name> ?".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @return true if the input is a valid bestiary query, false otherwise.
 */
bool isBestiaryQuery(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);


    // Expected pattern: What is effective against <monster> ?
    if (count != 6) return false;

    if (strcmp(tokens[0], "What") != 0 || 
        strcmp(tokens[1], "is") != 0 || 
        strcmp(tokens[2], "effective") != 0 || 
        strcmp(tokens[3], "against") != 0)
        return false;

    if (!isAlphabeticOnly(tokens[4])) return false;

    if (strcmp(tokens[5], "?") != 0) return false;

    return true;
}

/**
 * @brief Checks if the input string is a valid alchemy query.
 *
 * An alchemy query is defined as "What is in <potion_name> ?".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @return true if the input is a valid alchemy query, false otherwise.
 */
bool isAlchemyQuery(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);


    // Expected pattern: What is in <potion words...> ?
    if (count < 5) return false;

    if (strcmp(tokens[0], "What") != 0 || 
        strcmp(tokens[1], "is") != 0 || 
        strcmp(tokens[2], "in") != 0)
        return false;

    if (strcmp(tokens[count - 1], "?") != 0) return false;

    // Check that all tokens between "in" and "?" are valid potion words
    for (int i = 3; i < count - 1; i++) {
        if (!isValidPotionNameToken(tokens[i])) return false;
    }

    return true;
}


/**
 * @brief Checks if the input string is a valid exit command.
 *
 * An exit command is defined as "Exit".
 * The function checks for the correct format and spacing.
 *
 * @param input The input string to check.
 * @return true if the input is a valid exit command, false otherwise.
 */

bool isExitCommand(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    return count == 1 && strcmp(tokens[0], "Exit") == 0;
}

/**
 * @brief Executes the command based on the command type.
 *
 * @param input The input string to execute.
 * @param cmdType The type of command to execute.
 * @return 0 on success, -1 on failure.
 */
int executeCommand(const char* input, CommandType cmdType) {
    switch (cmdType) {
        case ACTION_LOOT:
            return executeLootAction(input);
        case ACTION_TRADE:
            return executeTradeAction(input);
        case ACTION_BREW:
            return executeBrewAction(input);
        case KNOWLEDGE_EFFECTIVENESS:
            return executeEffectivenessKnowledge(input);
        case KNOWLEDGE_POTION_FORMULA:
            return executeFormulaKnowledge(input);
        case ENCOUNTER:
            return executeEncounter(input);
        case QUERY_SPECIFIC_INVENTORY:
            return executeSpecificInventoryQuery(input);
        case QUERY_ALL_INVENTORY:
            return executeAllInventoryQuery(input);
        case QUERY_BESTIARY:
            return executeBestiaryQuery(input);
        case QUERY_ALCHEMY:
            return executeAlchemyQuery(input);
        case EXIT_COMMAND:
            return 0;
        default:
            return -1;
    }

}


/**
 * @brief Represents an alchemical ingredient with a name and quantity.
 */
typedef struct {
    char name[MAX_TOKEN_LENGTH];  /**< Name of the ingredient */
    int quantity;                 /**< Available quantity */
} Ingredient;

/** Global list of all available ingredients. */
static Ingredient ingredients[MAX_INGREDIENTS] = {0};
/** Number of currently stored ingredients. */
static int num_ingredients = 0;



/**
 * @brief Represents a trophy with a name and quantity.
 */
typedef struct {
    char name[MAX_TOKEN_LENGTH];  /**< Name of the trophy */
    int quantity;                 /**< Number of this trophy */
} Trophy;

/** Global list of all collected trophies. */
static Trophy trophies[MAX_TROPHIES] = {0};




/**
 * @brief Represents a potion composed of ingredients.
 */
typedef struct {
    char name[MAX_TOKEN_LENGTH];                         /**< Name of the potion */
    int ingredient_indices[MAX_POTION_INGREDIENTS];      /**< Indices of required ingredients */
    int ingredient_quantities[MAX_POTION_INGREDIENTS];   /**< Quantities for each ingredient */
    int ingredients_count;                               /**< Total number of ingredients */
    int quantity;                                        /**< Quantity of the potion available */
} Potion;

/** Global list of known potions. */
static Potion potions[MAX_POTIONS] = {0};
/** Number of known potions. */
static int potionsCount = 0;



/**
 * @brief Represents a magical sign.
 */
typedef struct {
    char name[MAX_TOKEN_LENGTH];  /**< Name of the sign */
} Sign;

/** Global list of known signs. */
static Sign signs[MAX_SIGNS] = {0};


/**
 * @brief Represents a beast and its weaknesses to signs and potions.
 */
typedef struct {
    char name[MAX_TOKEN_LENGTH];                          /**< Name of the beast */
    int effective_sign_indices[MAX_EFFECTIVENESS];        /**< Indices of effective signs */
    int effective_signs_count;                            /**< Count of effective signs */
    int effective_potion_indices[MAX_EFFECTIVENESS];      /**< Indices of effective potions */
    int effective_potions_count;                          /**< Count of effective potions */
} Beast;

/** Global list of known beasts. */
static Beast beasts[MAX_BEASTS] = {0};



/**
 * @brief Executes the "Geralt loots" action by parsing and storing obtained ingredients.
 *
 * This function tokenizes the input command, extracts ingredient names and their
 * quantities, and updates the global ingredient list. If an ingredient is new,
 * it is added to the list. If it already exists, its quantity is increased.
 *
 *
 * @param input The full command string starting with "Geralt loots".
 * @return Always returns 0.
 */
int executeLootAction(const char* input) {
    
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Parse the input to extract ingredients and quantities
    int token_index = 2; // Start after "Geralt loots"
    
    while (token_index < count) {
        int quantity = atoi(tokens[token_index]);
        token_index++;
        
        // Get the ingredient name
        char ingredient_name[MAX_TOKEN_LENGTH] = {0};
        strcpy(ingredient_name, tokens[token_index]);
        token_index++;
        
        // Check if we already have this ingredient
        int ingredient_index = -1;
        for (int i = 0; i < num_ingredients; i++) {
            if (strcmp(ingredients[i].name, ingredient_name) == 0) {
                ingredient_index = i;
                break;
            }
        }
        
        // If ingredient doesn't exist yet, add it
        if (ingredient_index == -1) {
            ingredient_index = num_ingredients;
            strcpy(ingredients[num_ingredients].name, ingredient_name);
            num_ingredients++;
        }
        
        // Update the quantity
        ingredients[ingredient_index].quantity += quantity;
        
        // Skip comma if present
        if (token_index < count && strcmp(tokens[token_index], ",") == 0) {
            token_index++;
        }
    }

    
    // Output the standard response
    printf("Alchemy ingredients obtained\n");

    return 0;
}

/**
 * @brief Executes the "Geralt trades" action by parsing and updating trophies and ingredients.
 *
 * This function tokenizes the input command, extracts trophy names and their
 * quantities, and updates the global trophy list. It also updates the ingredient
 * list based on the trade.
 *
 * @param input The full command string starting with "Geralt trades".
 * @return Always returns 0.
 */

int executeTradeAction(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Temporary arrays to store what trophies are required and what ingredients will be gained
    typedef struct {
        char name[MAX_TOKEN_LENGTH];
        int quantity;
        int index;  // Index in the original arrays
    } TradeItem;
    
    TradeItem required_trophies[MAX_INGREDIENTS] = {0};
    TradeItem gained_ingredients[MAX_INGREDIENTS] = {0};
    
    int num_required_trophies = 0;
    int num_gained_ingredients = 0;
    
    // Parse the input to extract required trophies and gained ingredients
    int token_index = 2; // Start after "Geralt trades"
    int parsing_trophies = 1; // Flag to indicate if we're parsing trophies (1) or ingredients (0)
    
    while (token_index < count) {
        // Check if we've reached the "for" token, which separates trophies from ingredients
        if (strcmp(tokens[token_index], "for") == 0) {
            parsing_trophies = 0;
            token_index++;
            continue;
        }
        
        // Parse quantity
        int quantity = atoi(tokens[token_index]);
        token_index++;
        
        // Parse name - might be multiple tokens until "trophy", comma, or "for"
        char item_name[MAX_TOKEN_LENGTH * 3] = {0}; // Allow for longer names
        
        // For trophies, we need to handle special case where quantity might be followed by multiple
        // words until we hit "trophy" or ","
        if (parsing_trophies) {
            while (token_index < count && 
                   strcmp(tokens[token_index], "trophy") != 0 && 
                   strcmp(tokens[token_index], ",") != 0 && 
                   strcmp(tokens[token_index], "for") != 0) {
                if (strlen(item_name) > 0) {
                    strcat(item_name, " ");
                }
                strcat(item_name, tokens[token_index]);
                token_index++;
            }
            
            // Skip "trophy" token if present
            if (token_index < count && strcmp(tokens[token_index], "trophy") == 0) {
                token_index++;
            }
            
            // Add to required trophies
            strcpy(required_trophies[num_required_trophies].name, item_name);
            required_trophies[num_required_trophies].quantity = quantity;
            required_trophies[num_required_trophies].index = -1; // Will set actual index later
            num_required_trophies++;
        } else {
            // For ingredients, just get the name (should be a single token)
            strcpy(item_name, tokens[token_index]);
            token_index++;
            
            // Add to gained ingredients
            strcpy(gained_ingredients[num_gained_ingredients].name, item_name);
            gained_ingredients[num_gained_ingredients].quantity = quantity;
            gained_ingredients[num_gained_ingredients].index = -1; // Will set actual index later
            num_gained_ingredients++;
        }
        
        // Skip comma if present
        if (token_index < count && strcmp(tokens[token_index], ",") == 0) {
            token_index++;
        }
    }
    
    // Check if Geralt has enough trophies
    int has_enough_trophies = 1; // Assume he has enough
    
    // Find indices of required trophies in the original trophies array
    for (int i = 0; i < num_required_trophies; i++) {
        int trophy_index = -1;
        
        // Search for the trophy in Geralt's inventory
        for (int j = 0; j < MAX_INGREDIENTS; j++) { // Assuming trophies array has MAX_INGREDIENTS elements
            if (trophies[j].quantity > 0 && strcmp(trophies[j].name, required_trophies[i].name) == 0) {
                trophy_index = j;
                break;
            }
        }
        
        // Check if trophy exists and has enough quantity
        if (trophy_index == -1 || trophies[trophy_index].quantity < required_trophies[i].quantity) {
            has_enough_trophies = 0;
            break;
        }
        
        required_trophies[i].index = trophy_index;
    }
    
    // Find indices of gained ingredients in the original ingredients array
    for (int i = 0; i < num_gained_ingredients; i++) {
        int ingredient_index = -1;
        
        // Search for the ingredient in Geralt's inventory
        for (int j = 0; j < MAX_INGREDIENTS; j++) {
            if (strlen(ingredients[j].name) > 0 && strcmp(ingredients[j].name, gained_ingredients[i].name) == 0) {
                ingredient_index = j;
                break;
            }
        }
        
        // If ingredient doesn't exist, find an empty slot
        if (ingredient_index == -1) {
            for (int j = 0; j < MAX_INGREDIENTS; j++) {
                if (strlen(ingredients[j].name) == 0) {
                    ingredient_index = j;
                    strcpy(ingredients[j].name, gained_ingredients[i].name);
                    ingredients[j].quantity = 0;
                    break;
                }
            }
        }
        
        gained_ingredients[i].index = ingredient_index;
    }
    
    // Perform the trade if Geralt has enough trophies
    if (has_enough_trophies) {
        // Reduce trophies
        for (int i = 0; i < num_required_trophies; i++) {
            trophies[required_trophies[i].index].quantity -= required_trophies[i].quantity;
        }
        
        // Increase ingredients
        for (int i = 0; i < num_gained_ingredients; i++) {
            ingredients[gained_ingredients[i].index].quantity += gained_ingredients[i].quantity;
        }
        
        printf("Trade successful\n");
    } else {
        printf("Not enough trophies\n");
    }
    
    return 0;
}

/**
 * @brief Executes the "Geralt brews" action by parsing and updating potion quantities.
 *
 * This function tokenizes the input command, extracts potion names, and updates
 * the global potion list. It checks if the required ingredients are available
 * before allowing the brew.
 *
 * @param input The full command string starting with "Geralt brews".
 * @return 0 on success, -1 on failure.
 */

int executeBrewAction(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Check if the command format is valid
    if (count < 3 || strcmp(tokens[0], "Geralt") != 0 || strcmp(tokens[1], "brews") != 0) {
        printf("Invalid command format\n");
        return -1;  // This is an invalid command
    }
    
    // Extract potion name - for brew action, the potion name is in tokens[2]
    char potionName[MAX_TOKEN_LENGTH] = "";
    strcpy(potionName, tokens[2]);
    
    // Find the potion in the potions array
    int potionIndex = -1;
    for (int i = 0; i < MAX_POTIONS; i++) {
        if (potions[i].name[0] != '\0' && strcmp(potions[i].name, potionName) == 0) {
            potionIndex = i;
            break;
        }
    }
    
    // Check if the potion formula exists
    if (potionIndex == -1) {
        printf("No formula for %s\n", potionName);
        return 0;  // Changed from -1 to 0 - command was valid but couldn't be executed
    }
    
    // Check if there are enough ingredients
    Potion* potion = &potions[potionIndex];
    int hasEnoughIngredients = 1;
    
    for (int i = 0; i < potion->ingredients_count; i++) {
        int ingredientIndex = potion->ingredient_indices[i];
        int requiredQuantity = potion->ingredient_quantities[i];
        
        if (ingredients[ingredientIndex].quantity < requiredQuantity) {
            hasEnoughIngredients = 0;
            break;
        }
    }
    
    if (!hasEnoughIngredients) {
        printf("Not enough ingredients\n");
        return 0;  // Changed from -1 to 0 - command was valid but couldn't be executed
    }
    
    // If we have enough ingredients, brew the potion
    // Reduce the ingredients
    for (int i = 0; i < potion->ingredients_count; i++) {
        int ingredientIndex = potion->ingredient_indices[i];
        int requiredQuantity = potion->ingredient_quantities[i];
        
        ingredients[ingredientIndex].quantity -= requiredQuantity;
    }
    
    // Increase the potion quantity
    potion->quantity++;
    
    printf("Alchemy item created: %s\n", potionName);
    return 0;
}

/**
 * @brief Executes the "Geralt learns" action for effectiveness knowledge.
 *
 * This function tokenizes the input command, extracts the counter name (sign or potion),
 * type, and monster name, and updates the bestiary with effectiveness information.
 *
 * @param input The full command string starting with "Geralt learns".
 * @return 0 on success, -1 on failure.
 */
int executeEffectivenessKnowledge(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Extract the counter name (sign or potion name), type, and monster name
    char counter_name[MAX_TOKEN_LENGTH] = {0};
    char counter_type[MAX_TOKEN_LENGTH] = {0}; // "sign" or "potion"
    char monster_name[MAX_TOKEN_LENGTH] = {0};
    
    // Parse input to extract necessary information
    strcpy(counter_name, tokens[2]); // Counter name is the 3rd token (index 2)
    strcpy(counter_type, tokens[3]); // Counter type is the 4th token (index 3)
    strcpy(monster_name, tokens[count-1]); // Monster name is the last token
    
    // Check if the monster already exists in the bestiary
    int monster_index = -1;
    for (int i = 0; i < MAX_BEASTS; i++) {
        if (beasts[i].name[0] != '\0' && strcmp(beasts[i].name, monster_name) == 0) {
            monster_index = i;
            break;
        }
    }
    
    // If monster doesn't exist, add it to the bestiary
    if (monster_index == -1) {
        // Find an empty slot in the beasts array
        for (int i = 0; i < MAX_BEASTS; i++) {
            if (beasts[i].name[0] == '\0') {
                monster_index = i;
                strcpy(beasts[i].name, monster_name);
                beasts[i].effective_potions_count = 0;
                beasts[i].effective_signs_count = 0;
                break;
            }
        }
        
        // Add the effectiveness information
        if (strcmp(counter_type, "sign") == 0) {
            // Check if sign exists in signs array, if not add it
            int sign_index = -1;
            for (int i = 0; i < MAX_SIGNS; i++) {
                if (signs[i].name[0] != '\0' && strcmp(signs[i].name, counter_name) == 0) {
                    sign_index = i;
                    break;
                }
            }
            
            if (sign_index == -1) {
                // Add new sign
                for (int i = 0; i < MAX_SIGNS; i++) {
                    if (signs[i].name[0] == '\0') {
                        strcpy(signs[i].name, counter_name);
                        sign_index = i;
                        break;
                    }
                }
            }
            
            // Add sign index to beast's effective signs
            beasts[monster_index].effective_sign_indices[0] = sign_index;
            beasts[monster_index].effective_signs_count = 1;
        } else if (strcmp(counter_type, "potion") == 0) {
            // For potions, we need to handle two cases:
            // 1. If the potion formula is already known (exists in potions array)
            // 2. If only the effectiveness is known (doesn't exist in potions array)
            
            int potion_index = -1;
            
            // First check if the potion formula is already known
            for (int i = 0; i < MAX_POTIONS; i++) {
                if (potions[i].name[0] != '\0' && strcmp(potions[i].name, counter_name) == 0) {
                    potion_index = i;
                    break;
                }
            }
            
            // If potion formula is not known, we need to create a special entry
            if (potion_index == -1) {
                // Create a special entry in the signs array to track this potion's name
                // (We're repurposing the signs array to also store potion names that are only known for effectiveness)
                for (int i = 0; i < MAX_SIGNS; i++) {
                    if (signs[i].name[0] == '\0') {
                        strcpy(signs[i].name, counter_name);
                        potion_index = i + MAX_POTIONS; // Use an offset to distinguish from regular potion indices
                        break;
                    }
                }
            }
            
            // Add potion index to beast's effective potions
            beasts[monster_index].effective_potion_indices[0] = potion_index;
            beasts[monster_index].effective_potions_count = 1;
        }
        
        printf("New bestiary entry added: %s\n", monster_name);
    } else {
        // Monster exists, check if the effectiveness is already known
        if (strcmp(counter_type, "sign") == 0) {
            // Check if this sign is already known to be effective
            int sign_index = -1;
            for (int i = 0; i < MAX_SIGNS; i++) {
                if (signs[i].name[0] != '\0' && strcmp(signs[i].name, counter_name) == 0) {
                    sign_index = i;
                    break;
                }
            }
            
            if (sign_index == -1) {
                // Add new sign
                for (int i = 0; i < MAX_SIGNS; i++) {
                    if (signs[i].name[0] == '\0') {
                        strcpy(signs[i].name, counter_name);
                        sign_index = i;
                        break;
                    }
                }
            }
            
            // Check if this sign is already known to be effective against this monster
            int already_known = 0;
            for (int i = 0; i < beasts[monster_index].effective_signs_count; i++) {
                if (beasts[monster_index].effective_sign_indices[i] == sign_index) {
                    already_known = 1;
                    break;
                }
            }
            
            if (already_known) {
                printf("Already known effectiveness\n");
            } else {
                // Add sign index to beast's effective signs
                beasts[monster_index].effective_sign_indices[beasts[monster_index].effective_signs_count] = sign_index;
                beasts[monster_index].effective_signs_count++;
                printf("Bestiary entry updated: %s\n", monster_name);
            }
        } else if (strcmp(counter_type, "potion") == 0) {
            // For potions, handle the same two cases as above
            int potion_index = -1;
            
            // First check if the potion formula is already known
            for (int i = 0; i < MAX_POTIONS; i++) {
                if (potions[i].name[0] != '\0' && strcmp(potions[i].name, counter_name) == 0) {
                    potion_index = i;
                    break;
                }
            }
            
            // If potion formula is not known, check if we already have an effectiveness entry
            if (potion_index == -1) {
                for (int i = 0; i < MAX_SIGNS; i++) {
                    if (signs[i].name[0] != '\0' && strcmp(signs[i].name, counter_name) == 0) {
                        potion_index = i + MAX_POTIONS; // Use the same offset convention
                        break;
                    }
                }
                
                // If no effectiveness entry exists yet, create one
                if (potion_index == -1) {
                    for (int i = 0; i < MAX_SIGNS; i++) {
                        if (signs[i].name[0] == '\0') {
                            strcpy(signs[i].name, counter_name);
                            potion_index = i + MAX_POTIONS;
                            break;
                        }
                    }
                }
            }
            
            // Check if this potion is already known to be effective against this monster
            int already_known = 0;
            for (int i = 0; i < beasts[monster_index].effective_potions_count; i++) {
                int existing_index = beasts[monster_index].effective_potion_indices[i];
                
                // Direct index match
                if (existing_index == potion_index) {
                    already_known = 1;
                    break;
                }
                
                // Check if it's the same potion but with different index types
                // (one regular index, one offset index)
                if (existing_index < MAX_POTIONS && potion_index >= MAX_POTIONS) {
                    // existing is regular, potion_index is offset
                    int sign_index = potion_index - MAX_POTIONS;
                    if (strcmp(potions[existing_index].name, signs[sign_index].name) == 0) {
                        already_known = 1;
                        break;
                    }
                } else if (existing_index >= MAX_POTIONS && potion_index < MAX_POTIONS) {
                    // existing is offset, potion_index is regular
                    int sign_index = existing_index - MAX_POTIONS;
                    if (strcmp(signs[sign_index].name, potions[potion_index].name) == 0) {
                        already_known = 1;
                        break;
                    }
                }
            }
            
            if (already_known) {
                printf("Already known effectiveness\n");
            } else {
                // Add potion index to beast's effective potions
                beasts[monster_index].effective_potion_indices[beasts[monster_index].effective_potions_count] = potion_index;
                beasts[monster_index].effective_potions_count++;
                printf("Bestiary entry updated: %s\n", monster_name);
            }
        }
    }
    
    return 0;
}

/**
 * @brief Executes the "Geralt learns" action for potion formula knowledge.
 *
 * This function tokenizes the input command, extracts the potion name and its
 * ingredients, and updates the potions array with the new formula.
 *
 * @param input The full command string starting with "Geralt learns".
 * @return 0 on success, -1 on failure.
 */
int executeFormulaKnowledge(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Extract the potion name
    char potion_name[MAX_TOKEN_LENGTH] = {0};
    strcpy(potion_name, tokens[2]);
    
    // Handle multi-word potion names (like "Black Blood")
    if (strcmp(tokens[3], "potion") != 0) {
        strcat(potion_name, " ");
        strcat(potion_name, tokens[3]);
        // If there are more words in the potion name, we would continue here
    }
    
    // Check if the potion already exists in the potions array
    int potion_index = -1;
    for (int i = 0; i < MAX_POTIONS; i++) {
        if (potions[i].name[0] != '\0' && strcmp(potions[i].name, potion_name) == 0) {
            potion_index = i;
            break;
        }
    }
    
    // If potion already exists, it's an already known formula
    if (potion_index != -1) {
        printf("Already known formula\n");
        return 0;
    }
    
    // Find an empty slot for the new potion
    for (int i = 0; i < MAX_POTIONS; i++) {
        if (potions[i].name[0] == '\0') {
            potion_index = i;
            break;
        }
    }
    
    
    // Add the new potion
    strcpy(potions[potion_index].name, potion_name);
    potions[potion_index].ingredients_count = 0;
    potions[potion_index].quantity = 0;  // Initialize quantity to 0
    
    // Find the index where ingredients start
    int ingredients_start = 6; // Default position after "Geralt learns [potion] consists of"
    
    // Adjust if potion name has multiple words
    if (strcmp(tokens[3], "potion") != 0) {
        ingredients_start++;
    }
    
    // Parse the ingredients
    for (int i = ingredients_start; i < count; i++) {
        // Extract quantity
        int quantity = atoi(tokens[i]);
        i++; // Move to ingredient name
        
        // Extract ingredient name
        char ingredient_name[MAX_TOKEN_LENGTH] = {0};
        strcpy(ingredient_name, tokens[i]);
        
        // Remove trailing comma if present
        int len = strlen(ingredient_name);
        if (len > 0 && ingredient_name[len-1] == ',') {
            ingredient_name[len-1] = '\0';
        }
        
        // Find or add the ingredient
        int ingredient_index = -1;
        
        // First, search for an existing ingredient with the same name
        for (int j = 0; j < MAX_INGREDIENTS; j++) {
            if (ingredients[j].name[0] != '\0' && strcmp(ingredients[j].name, ingredient_name) == 0) {
                ingredient_index = j;
                break;
            }
        }
        
        // If ingredient doesn't exist, add it
        if (ingredient_index == -1) {
            for (int j = 0; j < MAX_INGREDIENTS; j++) {
                if (ingredients[j].name[0] == '\0') {
                    ingredient_index = j;
                    strcpy(ingredients[j].name, ingredient_name);
                    ingredients[j].quantity = 0; // Initialize quantity
                    num_ingredients++; // Increment the global count of ingredients
                    break;
                }
            }
        }
        
        // Add ingredient to potion's ingredients list
        potions[potion_index].ingredient_indices[potions[potion_index].ingredients_count] = ingredient_index;
        potions[potion_index].ingredient_quantities[potions[potion_index].ingredients_count] = quantity;
        potions[potion_index].ingredients_count++;
        
        // Skip comma token if present
        if (i+1 < count && strcmp(tokens[i+1], ",") == 0) {
            i++;
        }
    }
    
    potionsCount++;

    // Output success message
    printf("New alchemy formula obtained: %s\n", potion_name);
    return 0;
}

/**
 * @brief Executes the "Geralt encounters" action by checking for effective potions and signs.
 *
 * This function tokenizes the input command, extracts the monster name, and checks
 * if Geralt has effective potions or signs to counter the monster.
 *
 * @param input The full command string starting with "Geralt encounters".
 * @return 0 on success, -1 on failure.
 */
int executeEncounter(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Extract the monster name (should be the 4th token)
    if (count < 4) {
        printf("Invalid encounter format\n");
        return -1;
    }
    
    char monsterName[MAX_TOKEN_LENGTH];
    strcpy(monsterName, tokens[3]);
    
    // Check if the monster exists in the bestiary
    int monsterIndex = -1;
    for (int i = 0; i < MAX_BEASTS; i++) {
        if (beasts[i].name[0] != '\0' && strcmp(beasts[i].name, monsterName) == 0) {
            monsterIndex = i;
            break;
        }
    }
    
    // If monster is not in the bestiary, Geralt is unprepared
    if (monsterIndex == -1) {
        printf("Geralt is unprepared and barely escapes with his life\n");
        return 0;
    }
    
    // Check if Geralt has any effective potions or signs
    Beast* monster = &beasts[monsterIndex];
    bool hasEffectiveCounter = false;
    
    // Check for effective potions that are in inventory
    bool hasEffectivePotions = false;
    for (int i = 0; i < monster->effective_potions_count; i++) {
        int potionIndex = monster->effective_potion_indices[i];
        
        // If it's a regular potion index
        if (potionIndex < MAX_POTIONS) {
            if (potions[potionIndex].quantity > 0) {
                hasEffectiveCounter = true;
                hasEffectivePotions = true;
                break;
            }
        }
        // If it's an effectiveness-only potion index (with offset)
        else {
            // Find the actual potion by name
            int signIndex = potionIndex - MAX_POTIONS;
            char* potionName = signs[signIndex].name;
            
            // Search for this potion in the potions array
            for (int j = 0; j < MAX_POTIONS; j++) {
                if (potions[j].name[0] != '\0' && 
                    strcmp(potions[j].name, potionName) == 0 &&
                    potions[j].quantity > 0) {
                    hasEffectiveCounter = true;
                    hasEffectivePotions = true;
                    break;
                }
            }
            if (hasEffectivePotions) break;
        }
    }
    
    // Check for effective signs
    bool hasEffectiveSigns = false;
    if (!hasEffectiveCounter && monster->effective_signs_count > 0) {
        hasEffectiveCounter = true;
        hasEffectiveSigns = true;
    }
    
    // If Geralt has no effective counter, he barely escapes
    if (!hasEffectiveCounter) {
        printf("Geralt is unprepared and barely escapes with his life\n");
        return 0;
    }
    
    // Geralt defeats the monster!
    
    // Consume one of each effective potion in inventory
    if (hasEffectivePotions) {
        for (int i = 0; i < monster->effective_potions_count; i++) {
            int potionIndex = monster->effective_potion_indices[i];
            
            // If it's a regular potion index
            if (potionIndex < MAX_POTIONS) {
                if (potions[potionIndex].quantity > 0) {
                    potions[potionIndex].quantity--;
                }
            }
            // If it's an effectiveness-only potion index (with offset)
            else {
                // Find the actual potion by name
                int signIndex = potionIndex - MAX_POTIONS;
                char* potionName = signs[signIndex].name;
                
                // Search for this potion in the potions array and consume one
                for (int j = 0; j < MAX_POTIONS; j++) {
                    if (potions[j].name[0] != '\0' && 
                        strcmp(potions[j].name, potionName) == 0 &&
                        potions[j].quantity > 0) {
                        potions[j].quantity--;
                        break;
                    }
                }
            }
        }
    }
    
    // Add trophy to inventory
    int trophyIndex = -1;
    for (int i = 0; i < MAX_TROPHIES; i++) {
        if (trophies[i].name[0] != '\0' && strcmp(trophies[i].name, monsterName) == 0) {
            trophyIndex = i;
            break;
        }
    }
    
    if (trophyIndex == -1) {
        // Trophy doesn't exist yet, find an empty slot
        for (int i = 0; i < MAX_TROPHIES; i++) {
            if (trophies[i].name[0] == '\0') {
                trophyIndex = i;
                strcpy(trophies[i].name, monsterName);
                trophies[i].quantity = 0;
                break;
            }
        }
    }
    
    // Increment trophy quantity
    if (trophyIndex != -1) {
        trophies[trophyIndex].quantity++;
    }
    
    printf("Geralt defeats %s\n", monsterName);
    return 0;
}

/**
 * @brief Executes the "Geralt checks" action for inventory queries.
 *
 * This function tokenizes the input command, extracts the category and item name,
 * and checks the inventory for the specified item.
 *
 * @param input The full command string starting with "Geralt checks".
 * @return 0 on success, -1 on failure.
 */
int executeSpecificInventoryQuery(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Extract the category and item name
    char category[MAX_TOKEN_LENGTH];
    strcpy(category, tokens[1]);
    
    char itemName[MAX_TOKEN_LENGTH];
    strcpy(itemName, tokens[2]);
    
    // Handle different categories
    if (strcmp(category, "ingredient") == 0) {
        // Search for the ingredient
        int quantity = 0;
        for (int i = 0; i < MAX_INGREDIENTS; i++) {
            if (ingredients[i].name[0] != '\0' && strcmp(ingredients[i].name, itemName) == 0) {
                quantity = ingredients[i].quantity;
                break;
            }
        }
        printf("%d\n", quantity);
    }
    else if (strcmp(category, "potion") == 0) {
        // Search for the potion
        int quantity = 0;
        for (int i = 0; i < MAX_POTIONS; i++) {
            if (potions[i].name[0] != '\0' && strcmp(potions[i].name, itemName) == 0) {
                quantity = potions[i].quantity;
                break;
            }
        }
        printf("%d\n", quantity);
    }
    else if (strcmp(category, "trophy") == 0) {
        // Search for the trophy
        int quantity = 0;
        for (int i = 0; i < MAX_TROPHIES; i++) {
            if (trophies[i].name[0] != '\0' && strcmp(trophies[i].name, itemName) == 0) {
                quantity = trophies[i].quantity;
                break;
            }
        }
        printf("%d\n", quantity);
    }
    
    return 0;
}

/**
 * @brief Executes the "Geralt checks all" action for inventory queries.
 *
 * This function tokenizes the input command, extracts the category, and checks
 * the inventory for all items in that category.
 *
 * @param input The full command string starting with "Geralt checks all".
 * @return 0 on success, -1 on failure.
 */
int executeAllInventoryQuery(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Extract the category
    char category[MAX_TOKEN_LENGTH];
    strcpy(category, tokens[1]);
    
    // Temporary array to store items for sorting
    typedef struct {
        char name[MAX_TOKEN_LENGTH];
        int quantity;
    } InventoryItem;
    
    InventoryItem items[MAX_ITEMS];
    int itemCount = 0;
    
    // Handle different categories
    if (strcmp(category, "ingredient") == 0) {
        // Collect all ingredients with non-zero quantity
        for (int i = 0; i < MAX_INGREDIENTS; i++) {
            if (ingredients[i].name[0] != '\0' && ingredients[i].quantity > 0) {
                strcpy(items[itemCount].name, ingredients[i].name);
                items[itemCount].quantity = ingredients[i].quantity;
                itemCount++;
            }
        }
    }
    else if (strcmp(category, "potion") == 0) {
        // Collect all potions with non-zero quantity
        for (int i = 0; i < MAX_POTIONS; i++) {
            if (potions[i].name[0] != '\0' && potions[i].quantity > 0) {
                strcpy(items[itemCount].name, potions[i].name);
                items[itemCount].quantity = potions[i].quantity;
                itemCount++;
            }
        }
    }
    else if (strcmp(category, "trophy") == 0) {
        // Collect all trophies with non-zero quantity
        for (int i = 0; i < MAX_TROPHIES; i++) {
            if (trophies[i].name[0] != '\0' && trophies[i].quantity > 0) {
                strcpy(items[itemCount].name, trophies[i].name);
                items[itemCount].quantity = trophies[i].quantity;
                itemCount++;
            }
        }
    }
    else {
        printf("Invalid category\n");
        return -1;
    }
    
    // Check if there are any items
    if (itemCount == 0) {
        printf("None\n");
        return 0;
    }
    
    // Sort items alphabetically by name
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if (strcmp(items[j].name, items[j+1].name) > 0) {
                // Swap items
                InventoryItem temp = items[j];
                items[j] = items[j+1];
                items[j+1] = temp;
            }
        }
    }
    
    // Format and print the output
    for (int i = 0; i < itemCount; i++) {
        printf("%d %s", items[i].quantity, items[i].name);
        if (i < itemCount - 1) {
            printf(", ");
        }
    }
    printf("\n");
    
    return 0;
}

/**
 * @brief Executes the "Geralt checks" action for bestiary queries.
 *
 * This function tokenizes the input command, extracts the monster name, and checks
 * the bestiary for effective potions and signs against that monster.
 *
 * @param input The full command string starting with "Geralt checks".
 * @return 0 on success, -1 on failure.
 */
int executeBestiaryQuery(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Extract the monster name (should be the 5th token)
    char monsterName[MAX_TOKEN_LENGTH];
    strcpy(monsterName, tokens[4]);
    
    // Check if the monster exists in the bestiary
    int monsterIndex = -1;
    for (int i = 0; i < MAX_BEASTS; i++) {
        if (beasts[i].name[0] != '\0' && strcmp(beasts[i].name, monsterName) == 0) {
            monsterIndex = i;
            break;
        }
    }
    
    // If monster is not in the bestiary, Geralt has no knowledge of it
    if (monsterIndex == -1) {
        printf("No knowledge of %s\n", monsterName);
        return 0;
    }
    
    // Collect all effective potions and signs
    Beast* monster = &beasts[monsterIndex];
    
    // Temporary array to store effective items for sorting
    typedef struct {
        char name[MAX_TOKEN_LENGTH];
    } EffectiveItem;
    
    EffectiveItem effectiveItems[MAX_EFFECTIVENESS * 2]; // Max possible items
    int itemCount = 0;
    
    // Add effective potions
    for (int i = 0; i < monster->effective_potions_count; i++) {
        int potionIndex = monster->effective_potion_indices[i];
        
        // Handle regular potions vs. effectiveness-only potions
        if (potionIndex < MAX_POTIONS) {
            // Regular potion (formula is known)
            strcpy(effectiveItems[itemCount].name, potions[potionIndex].name);
        } else {
            // Effectiveness-only potion (formula not known)
            int signIndex = potionIndex - MAX_POTIONS;
            strcpy(effectiveItems[itemCount].name, signs[signIndex].name);
        }
        itemCount++;
    }
    
    // Add effective signs
    for (int i = 0; i < monster->effective_signs_count; i++) {
        int signIndex = monster->effective_sign_indices[i];
        strcpy(effectiveItems[itemCount].name, signs[signIndex].name);
        itemCount++;
    }
    
    // Sort items alphabetically by name
    for (int i = 0; i < itemCount - 1; i++) {
        for (int j = 0; j < itemCount - i - 1; j++) {
            if (strcmp(effectiveItems[j].name, effectiveItems[j+1].name) > 0) {
                // Swap items
                EffectiveItem temp = effectiveItems[j];
                effectiveItems[j] = effectiveItems[j+1];
                effectiveItems[j+1] = temp;
            }
        }
    }
    
    // Format and print the output
    for (int i = 0; i < itemCount; i++) {
        printf("%s", effectiveItems[i].name);
        if (i < itemCount - 1) {
            printf(", ");
        }
    }
    printf("\n");
    
    return 0;
}

/**
 * @brief Executes the "Geralt brews" action by checking for potion formulas.
 *
 * This function tokenizes the input command, extracts the potion name, and checks
 * if Geralt has the formula for that potion.
 *
 * @param input The full command string starting with "Geralt brews".
 * @return 0 on success, -1 on failure.
 */
int executeAlchemyQuery(const char* input) {
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int count = tokenizeInput(input, tokens);
    
    // Extract the potion name (should be the 4th token)
    char potionName[MAX_TOKEN_LENGTH];
    strcpy(potionName, tokens[3]);
    
    // Check if the potion exists in Geralt's knowledge
    int potionIndex = -1;
    for (int i = 0; i < MAX_POTIONS; i++) {
        if (potions[i].name[0] != '\0' && strcmp(potions[i].name, potionName) == 0) {
            potionIndex = i;
            break;
        }
    }
    
    // If potion is not known, Geralt doesn't have the formula
    if (potionIndex == -1) {
        printf("No formula for %s\n", potionName);
        return 0;
    }
    
    // Get the potion and its ingredients
    Potion* potion = &potions[potionIndex];
    
    // Temporary array to store ingredients for sorting
    typedef struct {
        char name[MAX_TOKEN_LENGTH];
        int quantity;
    } PotionIngredient;
    
    PotionIngredient potionIngredients[MAX_POTION_INGREDIENTS];
    int ingredientCount = 0;
    
    // Collect all ingredients and their quantities
    for (int i = 0; i < potion->ingredients_count; i++) {
        int ingredientIndex = potion->ingredient_indices[i];
    
        
        // Ensure the ingredient index is valid
        if (ingredientIndex >= 0 && ingredientIndex < MAX_INGREDIENTS && ingredients[ingredientIndex].name[0] != '\0') {
            strcpy(potionIngredients[ingredientCount].name, ingredients[ingredientIndex].name);
            potionIngredients[ingredientCount].quantity = potion->ingredient_quantities[i];
            ingredientCount++;
        }
    }
    
    // Sort ingredients first by quantity (descending) and then by name
    for (int i = 0; i < ingredientCount - 1; i++) {
        for (int j = 0; j < ingredientCount - i - 1; j++) {
            // Sort by quantity (descending)
            if (potionIngredients[j].quantity < potionIngredients[j+1].quantity) {
                // Swap ingredients
                PotionIngredient temp = potionIngredients[j];
                potionIngredients[j] = potionIngredients[j+1];
                potionIngredients[j+1] = temp;
            }
            // If quantities are equal, sort by name (alphabetically)
            else if (potionIngredients[j].quantity == potionIngredients[j+1].quantity &&
                     strcmp(potionIngredients[j].name, potionIngredients[j+1].name) > 0) {
                // Swap ingredients
                PotionIngredient temp = potionIngredients[j];
                potionIngredients[j] = potionIngredients[j+1];
                potionIngredients[j+1] = temp;
            }
        }
    }
    
    // Format and print the output
    for (int i = 0; i < ingredientCount; i++) {
        printf("%d %s", potionIngredients[i].quantity, potionIngredients[i].name);
        if (i < ingredientCount - 1) {
            printf(", ");
        }
    }
    printf("\n");
    
    return 0;
}