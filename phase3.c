#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_SPELLS 100
#define MAX_SPELL_LENGTH 20
//char* castSpell_<groupname>(char** spells, char* previous_spell){ use for battle royale part of assignment
// Function to ch00eck if a move is valid
int isValidMove(char *spell, char *lastSpell)
{
    // Special case for the first move
    if (strlen(lastSpell) == 0)
    {
        return 1; // First move of the game is always valid
    }

    // Trim trailing whitespace from lastSpell
    int lastSpellLength = strlen(lastSpell);
    while (lastSpellLength > 0 && isspace(lastSpell[lastSpellLength - 1]))
    {
        lastSpellLength--;
    }

    // Compare the first character of the current spell with the last character of the previous spell
    if (spell[0] != lastSpell[lastSpellLength - 1])
    {
        return 0; // First character of the spell does not match the last character of the previous spell
    }

    return 1;
}

// Function to check if a move has already been played
int isRepeatMove(char *spell, char **playedSpells, int numPlayed)
{
    for (int i = 0; i < numPlayed; ++i)
    {
        if (strcmp(spell, playedSpells[i]) == 0)
        {
            return 1; // Spell has already been played
        }
    }
    return 0;
}

// Function to determine the winner
int determineWinner(char *spell, char **playedSpells, int numPlayed)
{
    if (!isValidMove(spell, playedSpells[numPlayed - 1]))
    {
        return 1; // Opponent's move is not valid
    }
    if (isRepeatMove(spell, playedSpells, numPlayed))
    {
        return 1; // Opponent repeated a spell
    }
    return 0; // No winner yet
}

// Basic bot implementation
void makeBotMove(char **playedSpells, int numPlayed, char *botSpell, char **spells, int numSpells, int difficulty) {
    if (difficulty == 1) {
        // Easy: Random move
        int randomIndex;
        do {
            randomIndex = rand() % numSpells;
        } while (isRepeatMove(spells[randomIndex], playedSpells, numPlayed) ||
                 !isValidMove(spells[randomIndex], playedSpells[numPlayed - 1]));
        strcpy(botSpell, spells[randomIndex]);
    } else if (difficulty == 2) {
        // Medium: Choose the first available valid and non-repeated spell
        for (int i = 0; i < numSpells; ++i) {
            if (!isRepeatMove(spells[i], playedSpells, numPlayed) &&
                isValidMove(spells[i], playedSpells[numPlayed - 1])) {
                strcpy(botSpell, spells[i]);
                return;
            }
        }
    } else if (difficulty == 3) {
        // Hard: Choose the spell that minimizes opponent's options
        int minOptions = numSpells;
        for (int i = 0; i < numSpells; ++i) {
            if (!isRepeatMove(spells[i], playedSpells, numPlayed)) {
                int options = 0;
                for (int j = 0; j < numSpells; ++j) {
                    if (!isRepeatMove(spells[j], playedSpells, numPlayed) &&
                        isValidMove(spells[j], playedSpells[numPlayed - 1]) &&
                        !isValidMove(spells[j], spells[i])) {
                        options++;
                    }
                }
                if (options < minOptions) {
                    minOptions = options;
                    strcpy(botSpell, spells[i]);
                }
            }
        }
    }

    // If no valid and non-repeated spell is found, set a default value for botSpell
    strcpy(botSpell, spells[rand() % numSpells]);
}
//}
int main() {
    srand(time(NULL));

    // Phase One - Reading spells from a file
    FILE *file = fopen("spells.txt", "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    int numSpells;
    fscanf(file, "%d", &numSpells);

    char **spells = malloc(MAX_SPELLS * sizeof(char *));
    for (int i = 0; i < MAX_SPELLS; ++i) {
        spells[i] = malloc(MAX_SPELL_LENGTH * sizeof(char));
    }

    for (int i = 0; i < numSpells; ++i) {
        fscanf(file, "%s", spells[i]);
    }

    fclose(file);

    // Display the list of spells
    printf("List of Spells:\n");
    for (int i = 0; i < numSpells; ++i) {
        printf("%-15s", spells[i]);
        if ((i + 1) % 5 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    // Phase Two - Implementing the Spell Master game with a bot
    // Get player names
    char player[MAX_SPELL_LENGTH], bot[MAX_SPELL_LENGTH];
    printf("Enter your name: ");
    scanf("%s", player);
    strcpy(bot, "Bot");

    // Choose difficulty level
    int difficulty;
    printf("Choose difficulty level (1-Easy, 2-Medium, 3-Hard): ");
    scanf("%d", &difficulty);

    // Toss a fair coin to determine who starts
    int currentPlayer = 1;
    printf("%s starts!\n", (currentPlayer == 1) ? player : bot);

    char **playedSpells = malloc((numSpells + 1) * sizeof(char *));
    for (int i = 0; i <= numSpells; ++i) {
        playedSpells[i] = malloc(MAX_SPELL_LENGTH * sizeof(char));
    }

    int numPlayed = 0;
    int winner = 0;

    while (!winner) {
        char currentSpell[MAX_SPELL_LENGTH];

        if (currentPlayer == 1) {
            // Human player's turn
            printf("%s, enter your spell: ", player);
            scanf("%s", currentSpell);

            if (!isValidMove(currentSpell, playedSpells[numPlayed])) {
                printf("Invalid move! %s wins.\n", bot);
                winner = 1;
            } else if (isRepeatMove(currentSpell, playedSpells, numPlayed)) {
                printf("Repeat move! %s wins.\n", bot);
                winner = 1;
            } else {
                strcpy(playedSpells[numPlayed], currentSpell);
                printf("%s played: %s\n", player, currentSpell);
            }
        } else {
            // Bot's turn
            makeBotMove(playedSpells, numPlayed, currentSpell, spells, numSpells, difficulty);
            printf("%s chooses: %s\n", bot, currentSpell);

            if (!isValidMove(currentSpell, playedSpells[numPlayed])) {
                printf("Invalid move! %s wins.\n", player);
                winner = 1;
            } else if (isRepeatMove(currentSpell, playedSpells, numPlayed)) {
                printf("Repeat move! %s wins.\n", player);
                winner = 1;
            } else {
                strcpy(playedSpells[numPlayed], currentSpell);
            }
        }

        numPlayed++;

        if (numPlayed == numSpells) {
            printf("No more spells left. It's a tie!\n");
            winner = 1;
        } else {
            currentPlayer = 3 - currentPlayer;  // Switch players (1 -> 2, 2 -> 1)
        }
    }

    // Free allocated memory
    for (int i = 0; i < MAX_SPELLS; ++i) {
        free(spells[i]);
    }
    free(spells);

    for (int i = 0; i <= numSpells; ++i) {
        free(playedSpells[i]);
    }
    free(playedSpells);

    return 0;
}
