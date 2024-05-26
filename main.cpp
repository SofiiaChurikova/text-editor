#include <iostream>
#include <cstdlib>

struct UserParams {
    char *userInput;
    char *allInputs;
    size_t bufferInput;
};

enum Commands {
    COMMAND_HELP = 0,
    COMMAND_APPEND = 1,
    START_NEW_LINE = 2,
    LOAD_FROM_FILE = 3,
    SAVE_TO_FILE = 4,
    PRINT_TEXT = 5,
    INSERT_BY_INDEX = 6,
    SEARCH_TEXT = 7,
    CLEAR_CONSOLE = 8,
    EXIT = 9
};

void ProgramHelper() {
    printf("Hello! This is a Simple Text Editor. You can use these commands: \n"
        "0 - Help \n"
        "1 - Append text \n"
        "2 - Start the new line \n"
        "3 - Load the information from the file \n"
        "4 - Save the information to the file \n"
        "5 - Print the text \n"
        "6 - Insert the text by line and symbol \n"
        "7 - Search the text \n"
        "8 - Clear the console \n"
        "9 - Exit \n");
}

void CommandParser(int *command) {
    printf("Which command do you want to use? (0 - Help): ");
    while (scanf("%d", command) != 1 || *command < COMMAND_HELP || *command > EXIT) {
        printf("Invalid input! Please enter a valid command (0 - 9): ");
        while (getchar() != '\n');
    }
    while (getchar() != '\n');
}

void AppendText(UserParams *up) {
    up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
    printf("Enter a string that you want append: ");
    getline(&up->userInput, &up->bufferInput, stdin);
    up->userInput[strcspn(up->userInput, "\n")] = '\0';
    if (up->allInputs == NULL) {
        up->allInputs = (char *) malloc((strlen(up->userInput) + 1) * sizeof(char));
        strcpy(up->allInputs, up->userInput);
    } else {
        up->allInputs = (char *) realloc(up->allInputs,
                                         (strlen(up->userInput) + strlen(up->allInputs) + 1) * sizeof(char));
        strcat(up->allInputs, up->userInput);
    }
    // printf("You entered: %s\n", up->allInputs);
    printf("Text was appended.\n");
    free(up->userInput);
    up->userInput = NULL;
}

void NewLine(UserParams *up) {
    up->allInputs = (char *) realloc(up->allInputs, (strlen(up->allInputs) + 2) * sizeof(char));
    strcat(up->allInputs, "\n");
    printf("New line was started.\n");
}

void PrintText(UserParams *up) {
    if (up->allInputs == NULL) {
        printf("There is no text!\n");
    } else {
        printf("%s", up->allInputs);
        printf("\n");
    }
}

void Clear(UserParams *up) {
    free(up->allInputs);
    up->allInputs = NULL;
    printf("Console was cleared\n");
}

void SaveFile(UserParams *up) {
    FILE *file;
    printf("Enter a title for file: \n");
    up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
    scanf("%s", up->userInput);
    file = fopen(up->userInput, "w");
    if (file != NULL) {
        fputs(up->allInputs, file);
        fclose(file);
        printf("File created and text saved!\n");
    } else {
        printf("Error opening file\n");
    }
    free(up->userInput);
}

void LoadFromFile(UserParams *up) {
    FILE *file;
    if (up->allInputs != NULL) {
        char dataAnswer[20];
        printf("Do you really want to load a new file? (Yes/No) You have unsaved text: ");
        scanf("%s", dataAnswer);
        for (int i = 0; dataAnswer[i]; i++) {
            dataAnswer[i] = tolower(dataAnswer[i]);
        }
        if (strcmp(dataAnswer, "yes") == 0) {
            if (up->allInputs != NULL) {
                free(up->allInputs);
                up->allInputs = NULL;
            }
        } else {
            return;
        }
    }
    printf("Enter a file that you want to load info from: ");
    char fileInput[256];
    scanf("%s", fileInput);
    file = fopen(fileInput, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        fclose(file);
        return;
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);
    up->userInput = (char *) realloc(up->userInput, (fileSize + 1) * sizeof(char));
    if (fread(up->userInput, sizeof(char), fileSize, file) == fileSize) {
        up->userInput[fileSize] = '\0';
        up->allInputs = (char *) realloc(up->allInputs, (fileSize + 1) * sizeof(char));
        strcat(up->allInputs, up->userInput);
        printf("File content loaded\n");
    } else {
        printf("Error reading file\n");
    }
    fclose(file);
    free(up->userInput);
    up->userInput = NULL;
}

void SearchText(UserParams *up) {
    int line = 1;
    int charIndex = 1;
    if (up->allInputs == NULL) {
        printf("There is no text to search!\n");
        return;
    }
    printf("Enter text to search: ");
    up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
    scanf("%s", up->userInput);
    char *search = strstr(up->allInputs, up->userInput);
    if (search == NULL) {
        printf("Text was not found.\n");
    } else {
        char *currentPosition = up->allInputs;
        while (search != NULL) {
            while (currentPosition != search) {
                if (*currentPosition == '\n') {
                    line++;
                    charIndex = 1;
                } else {
                    charIndex++;
                }
                currentPosition++;
            }
            printf("Text is present in this position: line %d, index %d\n", line, charIndex);
            search = strstr(currentPosition + 1, up->userInput);
        }
    }
    free(up->userInput);
    up->userInput = NULL;
}

void InsertByIndex(UserParams *up) {
    int line;
    int index;
    int numOfLines = 0;
    printf("Choose line and index: ");
    scanf("%d %d", &line, &index);
    if (up->allInputs[0] != '\0') {
        numOfLines = 1;
    }
    for (int l = 0; up->allInputs[l] != '\0'; l++) {
        if (up->allInputs[l] == '\n') {
            numOfLines++;
        }
    }
    if (line > numOfLines) {
        printf("There aren't that many lines! Try again. Number of lines: %d\n", numOfLines);
        free(up->userInput);
        up->userInput = NULL;
        return;
    }
    int curLine = 1;
    int numOfChars = 0;
    for (int c = 0; up->allInputs[c] != '\0'; c++) {
        if (curLine == line) {
            if (up->allInputs[c] == '\n') {
                break;
            }
            numOfChars++;
        }
        if (up->allInputs[c] == '\n') {
            curLine++;
        }
    }

    if (index > numOfChars + 1) {
        printf("There aren't that many positions to insert! Try again. Number of positions you can insert to: %d\n",
               numOfChars + 1);
        return;
    }
    printf("Enter what do you want to insert: ");
    up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
    scanf("%s", up->userInput);
    if (up->allInputs == NULL) {
        printf("There is no text to insert. Use Append Command!\n");
        free(up->userInput);
        up->userInput = NULL;
        return;
    }
    char *newBuffer = (char *) malloc(strlen(up->allInputs) + strlen(up->userInput) + 1);
    int currentLine = 1;
    int currentIndex = 1;
    int i = 0;
    int j = 0;
    int m = 0;
    while (currentLine < line || currentIndex < index) {
        newBuffer[j++] = up->allInputs[i];
        if (up->allInputs[i] == '\n') {
            currentLine++;
            currentIndex = 1;
        } else {
            currentIndex++;
        }
        i++;
    }
    while (up->userInput[m] != '\0') {
        newBuffer[j++] = up->userInput[m++];
    }
    while (up->allInputs[i] != '\0') {
        newBuffer[j++] = up->allInputs[i++];
    }
    up->allInputs = (char *) realloc(up->allInputs, strlen(newBuffer) * sizeof(char));
    strcpy(up->allInputs, newBuffer);
    printf("Text was inserted.\n");
    free(newBuffer);
    newBuffer = NULL;
    free(up->userInput);
    up->userInput = NULL;
}

void CommandRunner(int command, UserParams *up) {
    switch (command) {
        case COMMAND_HELP:
            ProgramHelper();
            return;
        case COMMAND_APPEND:
            AppendText(up);
            break;
        case START_NEW_LINE:
            NewLine(up);
            break;
        case LOAD_FROM_FILE:
            LoadFromFile(up);
            break;
        case SAVE_TO_FILE:
            SaveFile(up);
            break;
        case PRINT_TEXT:
            PrintText(up);
            break;
        case INSERT_BY_INDEX:
            InsertByIndex(up);
            break;
        case SEARCH_TEXT:
            SearchText(up);
            break;
        case CLEAR_CONSOLE:
            Clear(up);
            break;
        case EXIT:
            Clear(up);
            printf("Thanks for using this program. Bye!");
            break;;
        default:
            printf("Unknown command\n");
            break;
    }
}

int main() {
    int command;
    UserParams up{NULL, NULL, 100};
    do {
        CommandParser(&command);
        CommandRunner(command, &up);
    } while
    (
        command != EXIT
    );
    return 0;
}
