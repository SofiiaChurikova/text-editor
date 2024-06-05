#include <iostream>
#include <cstdlib>

class UserParams {
public:
    char *userInput = NULL;
    char *allInputs = NULL;
    size_t bufferInput = 256;
    bool isSaved = true;
};

class TextEditor {
public:
    UserParams up;

    static void ProgramHelper() {
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

    static void AppendText(UserParams *up) {
        up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
        printf("Enter a string that you want append: ");
        size_t len = 0;
        int i;
        while ((i = getchar()) != '\n') {
            if (len + 1 >= up->bufferInput) {
                up->bufferInput += 256;
                up->userInput = (char *) realloc(up->userInput, (up->bufferInput) * sizeof(char));
            }
            up->userInput[len++] = i;
        }
        up->userInput[len] = '\0';
        if (up->allInputs == NULL) {
            up->allInputs = (char *) malloc((strlen(up->userInput) + 1) * sizeof(char));
            strcpy(up->allInputs, up->userInput);
        } else {
            up->allInputs = (char *) realloc(up->allInputs,
                                             (strlen(up->userInput) + strlen(up->allInputs) + 1) * sizeof(char));
            strcat(up->allInputs, up->userInput);
        }
        printf("Text was appended.\n");
        free(up->userInput);
        up->userInput = NULL;
        up->isSaved = false;
    }

    static void NewLine(UserParams *up) {
        if (up->allInputs == NULL) {
            up->allInputs = (char *) malloc(2 * sizeof(char));
            strcpy(up->allInputs, "\n");
        } else {
            up->allInputs = (char *) realloc(up->allInputs, (strlen(up->allInputs) + 2) * sizeof(char));
            strcat(up->allInputs, "\n");
        }
        printf("New line was started.\n");
        up->isSaved = false;
    }

    static void PrintText(UserParams *up) {
        if (up->allInputs == NULL) {
            printf("There is no text to print!\n");
        } else {
            printf("%s", up->allInputs);
            printf("\n");
        }
    }

    static void Clear(UserParams *up) {
        if (up->allInputs == NULL) {
            printf("Console is empty.\n");
            return;
        } else {
            free(up->allInputs);
            up->allInputs = NULL;
            printf("Console was cleared.\n");
            up->isSaved = false;
        }
    }

    static void SearchText(UserParams *up) {
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
                printf("Text was found at: line %d, index %d\n", line, charIndex);
                search = strstr(currentPosition + 1, up->userInput);
            }
        }
        free(up->userInput);
        up->userInput = NULL;
    }

    static void InsertByIndex(UserParams *up) {
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
        if (up->allInputs == NULL) {
            printf("There is no text to insert. Use Append Command!\n");
            free(up->userInput);
            up->userInput = NULL;
            return;
        }
        getchar();
        getline(&up->userInput, &up->bufferInput, stdin);
        up->userInput[strcspn(up->userInput, "\n")] = '\0';
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
        up->isSaved = false;
    }
};

class FileHandler {
public:
    UserParams up;

    static void SaveFile(UserParams *up) {
        FILE *file;
        if (up->allInputs == NULL) {
            printf("There is no text to save!\n");
            return;
        }
        printf("Enter a title for file: ");
        up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
        scanf("%s", up->userInput);
        file = fopen(up->userInput, "w");
        if (file != NULL) {
            fputs(up->allInputs, file);
            printf("File created and text saved!\n");
            up->isSaved = true;
        } else {
            printf("Error opening file.\n");
        }
        fclose(file);
        free(up->userInput);
        up->userInput = NULL;
    }

    static void LoadFromFile(UserParams *up) {
        FILE *file;
        if (!up->isSaved && up->allInputs != NULL) {
            char dataAnswer[20];
            while (true) {
                printf("Do you really want to load a new file? (Yes/No) You have unsaved text: ");
                scanf("%s", dataAnswer);
                for (int i = 0; dataAnswer[i]; i++) {
                    dataAnswer[i] = tolower(dataAnswer[i]);
                }
                if (strcmp(dataAnswer, "yes") == 0) {
                    free(up->allInputs);
                    up->allInputs = NULL;
                    break;
                } else {
                    return;
                }
            }
        }
        printf("Enter a file that you want to load info from: ");
        char fileInput[256];
        scanf("%s", fileInput);
        file = fopen(fileInput, "r");
        if (file == NULL) {
            printf("Error opening file.\n");
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
            printf("File content loaded.\n");
            up->isSaved = true;
        } else {
            printf("Error reading file.\n");
        }
        fclose(file);
        free(up->userInput);
        up->userInput = NULL;
    }
};

class CommandsRunner {
public:
    UserParams up;
    FileHandler files;
    TextEditor editor;

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

    static void CommandParser(int *command) {
        printf("Which command do you want to use? (0 - Help): ");
        while (scanf("%d", command) != 1 || *command < COMMAND_HELP || *command > EXIT) {
            printf("Invalid input! Please enter a valid command (0 - 9): ");
            while (getchar() != '\n');
        }
        while (getchar() != '\n');
    }

    void CommandRunner(int command, UserParams *up) {
        switch (command) {
            case COMMAND_HELP:
                editor.ProgramHelper();
                return;
            case COMMAND_APPEND:
                editor.AppendText(up);
                break;
            case START_NEW_LINE:
                editor.NewLine(up);
                break;
            case LOAD_FROM_FILE:
                files.LoadFromFile(up);
                break;
            case SAVE_TO_FILE:
                files.SaveFile(up);
                break;
            case PRINT_TEXT:
                editor.PrintText(up);
                break;
            case INSERT_BY_INDEX:
                editor.InsertByIndex(up);
                break;
            case SEARCH_TEXT:
                editor.SearchText(up);
                break;
            case CLEAR_CONSOLE:
                editor.Clear(up);
                break;
            case EXIT:
                editor.Clear(up);
                printf("Thanks for using this program. Bye!");
                break;;
            default:
                printf("Unknown command.\n");
                break;
        }
    }
};

int main() {
    UserParams up;
    CommandsRunner commands;
    int command;
    do {
        commands.CommandParser(&command);
        commands.CommandRunner(command, &up);
    } while
    (
        command != commands.EXIT
    );
    return 0;
}
