#include <iostream>

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
void CommandRunner(int command) {
    switch (command) {
        case 0:
            ProgramHelper();
        return;
        case 1:
            printf("Command is not implemented yet \n");
        break;
        case 2:
            printf("Command is not implemented yet \n");
        break;
        case 3:
            printf("Command is not implemented yet \n");
        break;
        case 4:
            printf("Command is not implemented yet \n");
        break;
        case 5:
            printf("Command is not implemented yet \n");
        break;
        case 6:
            printf("Command is not implemented yet \n");
        break;
        case 7:
            printf("Command is not implemented yet \n");
        break;
        case 8:
            printf("Command is not implemented yet \n");
        break;
        default:
            printf("Thanks for using this program. Bye!");
        break;
    }
}
int main() {
    int command;
    do {
        CommandParser(&command);
        CommandRunner(command);
    }
    while
    (
        command != 9
    );
    return 0;
}
