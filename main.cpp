#include <iostream>
#include <cstdlib>

struct UserParams{
    char *userInput;
    char *allInputs;
    size_t bufferInput;
};
void ProgramHelper(){
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
    while (scanf("%d", command) < 0 || *command > 9) {
        printf("Invalid input! Please enter a valid command (0 - 9): ");
        while (getchar() != '\n');
    }
    while (getchar() != '\n');
}
void AppendText(UserParams *up) {
    up->userInput = (char*)malloc(up->bufferInput * sizeof(char));
    printf("Enter a string that you want append: ");
    getline(&up->userInput, &up->bufferInput, stdin);
    up->userInput[strcspn(up->userInput, "\n")] = '\0';
    if (up->allInputs == NULL)
    {
        up->allInputs = (char*)malloc((strlen(up->userInput) + 1) * sizeof(char));
        strcpy(up->allInputs, up->userInput);
    }
    else
    {
        up->allInputs = (char*)realloc(up->allInputs, (strlen(up->userInput) + strlen(up->allInputs) + 1) * sizeof(char));
        strcat(up->allInputs, up->userInput);
    }
    // printf("You entered: %s\n", up->allInputs);
    printf("Text was appended.\n");
    free(up->userInput);
    up->userInput = NULL;
}
void NewLine(UserParams *up) {
    up->allInputs = (char*)realloc(up->allInputs, (strlen(up->allInputs) + 2) * sizeof(char));
    strcat(up->allInputs, "\n");
    printf("New line was started.\n");
}
void PrintText(UserParams *up) {
    if (up->allInputs == NULL) {
        printf("There is no text!\n");
    }
    else {
        printf(up->allInputs);
        printf("\n");
    }
}
void Clear(UserParams *up) {
    free(up->allInputs);
    up->allInputs = NULL;
    printf("Console was cleared\n");
}
void SaveFile(UserParams *up) {
    FILE * file;
    printf("Enter a title for file: \n");
    up->userInput = (char*)malloc(up->bufferInput * sizeof(char));
    scanf("%s", up->userInput);
    file = fopen(up->userInput, "w");
    if (file != NULL)
    {
        fputs(up->allInputs, file);
        fclose(file);
        printf("File created and text saved!\n");
    }
    else
    {
        printf("Error opening file\n");
    }
    free(up->userInput);
}
void LoadFromFile(UserParams *up) {
    FILE * file;
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
    up->userInput = (char*)realloc(up->userInput, (fileSize + 1) * sizeof(char));
    if (fread(up->userInput, sizeof(char), fileSize, file) == fileSize) {
        up->allInputs = (char*)realloc(up->allInputs, (fileSize + 1) * sizeof(char));
        strcat(up->allInputs, up->userInput);
        printf("File content loaded\n");
    } else {
        printf("Error reading file\n");
    }
    fclose(file);
    free(up->userInput);
    up->userInput = NULL;
}

void CommandRunner(int command, UserParams *up) {
    switch (command) {
        case 0:
            ProgramHelper();
        return;
        case 1:
            AppendText(up);
        break;
        case 2:
            NewLine(up);
        break;
        case 3:
            LoadFromFile(up);
        break;
        case 4:
            SaveFile(up);
        break;
        case 5:
            PrintText(up);
        break;
        case 6:
            printf("Command is not implemented yet \n");
        break;
        case 7:
            printf("Command is not implemented yet \n");
        break;
        case 8:
            Clear(up);
        break;
        default:
            printf("Thanks for using this program. Bye!");
        break;
    }
}
int main() {
    int command;
    UserParams up{NULL, NULL, 100};
    do {
        CommandParser(&command);
        CommandRunner(command, &up);
    }
    while
    (
        command != 9
    );
    return 0;
}