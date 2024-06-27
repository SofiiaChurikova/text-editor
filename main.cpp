#include <iostream>
#include <cstdlib>
#include <dlfcn.h>
#include <cstring>

using namespace std;

class UserParams {
public:
    char *userInput = nullptr;
    char *allInputs = nullptr;
    size_t bufferInput = 768;
    bool isSaved = true;
    char *undoFirst = nullptr;
    char *undoSecond = nullptr;
    char *undoThird = nullptr;
    char *redoFirst = nullptr;
    char *redoSecond = nullptr;
    char *redoThird = nullptr;
    char *pasteBuffer = nullptr;
    char *processed = nullptr;

    UserParams() {
        allInputs = (char *) malloc(1);
        allInputs[0] = '\0';
    }

    ~UserParams() {
        FreeMemory();
    }

    void FreeMemory() {
        free(userInput);
        userInput = nullptr;
        free(allInputs);
        allInputs = nullptr;
        free(undoFirst);
        undoFirst = nullptr;
        free(undoSecond);
        undoSecond = nullptr;
        free(undoThird);
        undoThird = nullptr;
        free(redoFirst);
        redoFirst = nullptr;
        free(redoSecond);
        redoSecond = nullptr;
        free(redoThird);
        redoThird = nullptr;
        free(pasteBuffer);
        pasteBuffer = nullptr;
        free(processed);
        processed = nullptr;
    }
};

class CaesarCipher {
private:
    void *handle;

    typedef char *(*encrypt_ptr)(char *, int);

    typedef char *(*decrypt_ptr)(char *, int);

    encrypt_ptr encrypt;
    decrypt_ptr decrypt;

    void closeLibrary() {
        if (handle) {
            dlclose(handle);
            handle = nullptr;
        }
    }

    bool loadLibrary() {
        handle = dlopen("./libcaesar.dylib", RTLD_LAZY);
        if (handle == nullptr) {
            return false;
        }
        encrypt = (encrypt_ptr) dlsym(handle, "Encrypt");
        decrypt = (decrypt_ptr) dlsym(handle, "Decrypt");
        if (!encrypt || !decrypt) {
            cout << "Proc failed or not found" << endl;
            closeLibrary();
            return false;
        }
        return true;
    }

public:
    CaesarCipher() {
        if (!loadLibrary()) {
            closeLibrary();
            cout << "Problems with your library!" << endl;
        }
    }

    ~CaesarCipher() {
        closeLibrary();
    }

    void Encrypt(char *Text, int key) {
        char *encrypted = encrypt(Text, key);
        strcpy(Text, encrypted);
        free(encrypted);
    }

    void Decrypt(char *Text, int key) {
        char *decrypted = decrypt(Text, key);
        strcpy(Text, decrypted);
        free(decrypted);
    }

    void EncryptChunk(char *Text, int key) {
        char *encrypted = encrypt(Text, key);
        strncpy(Text, encrypted, 128);
        free(encrypted);
        encrypted = nullptr;
    }

    void DecryptChunk(char *Text, int key) {
        char *decrypted = decrypt(Text, key);
        strncpy(Text, decrypted, 128);
        free(decrypted);
        decrypted = nullptr;
    }
};


class TextEditor {
    UserParams up;
    CaesarCipher cipher;

public:
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
            "9 - Delete number of symbols by line and index \n"
            "10 - Undo \n"
            "11 - Redo \n"
            "12 - Cut \n"
            "13 - Copy \n"
            "14 - Paste \n"
            "15 - Insert with replacement \n"
            "16 - Encrypt \n"
            "17 - Decrypt \n"
            "18 - Encrypt file \n"
            "19 - Decrypt file \n"
            "20 - Exit \n");
    }

    void SaveState(UserParams *up) {
        free(up->undoThird);
        up->undoThird = up->undoSecond;
        up->undoSecond = up->undoFirst;
        if (up->allInputs != nullptr) {
            size_t len = strnlen(up->allInputs, up->bufferInput);
            up->undoFirst = (char *) malloc((len + 1) * sizeof(char));
            strncpy(up->undoFirst, up->allInputs, up->bufferInput);
        } else {
            up->undoFirst = nullptr;
        }
        free(up->redoFirst);
        up->redoFirst = nullptr;
        free(up->redoSecond);
        up->redoSecond = nullptr;
        free(up->redoThird);
        up->redoThird = nullptr;
    }

    void AppendText(UserParams *up) {
        SaveState(up);
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
        if (up->allInputs == nullptr) {
            up->allInputs = (char *) malloc((strlen(up->userInput) + 1) * sizeof(char));
            strcpy(up->allInputs, up->userInput);
        } else {
            up->allInputs = (char *) realloc(up->allInputs,
                                             (strlen(up->userInput) + strlen(up->allInputs) + 1) * sizeof(char));
            strcat(up->allInputs, up->userInput);
        }
        printf("Text was appended.\n");
        free(up->userInput);
        up->userInput = nullptr;
        up->isSaved = false;
    }

    void NewLine(UserParams *up) {
        SaveState(up);
        if (up->allInputs == nullptr) {
            up->allInputs = (char *) malloc(2 * sizeof(char));
            strcpy(up->allInputs, "\n");
        } else {
            up->allInputs = (char *) realloc(up->allInputs, (strlen(up->allInputs) + 2) * sizeof(char));
            strcat(up->allInputs, "\n");
        }
        printf("New line was started.\n");
        up->isSaved = false;
    }

    void PrintText(UserParams *up) {
        if (up->allInputs == nullptr) {
            printf("There is no text to print!\n");
        } else {
            printf("%s", up->allInputs);
            printf("\n");
        }
    }

    void Clear(UserParams *up) {
        SaveState(up);
        if (up->allInputs == nullptr) {
            printf("Console is empty.\n");
            return;
        } else {
            free(up->allInputs);
            up->allInputs = nullptr;
            printf("Console was cleared.\n");
            up->isSaved = false;
        }
    }

    void SearchText(UserParams *up) {
        int line = 1;
        int charIndex = 1;
        if (up->allInputs == nullptr) {
            printf("There is no text to search!\n");
            return;
        }
        printf("Enter text to search: ");
        up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
        scanf("%s", up->userInput);
        char *search = strstr(up->allInputs, up->userInput);
        if (search == nullptr) {
            printf("Text was not found.\n");
            return;
        }

        char *currentPosition = up->allInputs;
        while (search != nullptr) {
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

        free(up->userInput); // todo
        up->userInput = nullptr;
    }

    void InsertByIndex(UserParams *up) {
        SaveState(up);
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
            up->userInput = nullptr;
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
            printf(
                "There aren't that many positions to insert! Try again. Number of positions you can insert to: %d\n",
                numOfChars + 1);
            return;
        }
        printf("Enter what do you want to insert: ");
        up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
        if (up->allInputs == nullptr) {
            printf("There is no text to insert. Use Append Command!\n");
            free(up->userInput);
            up->userInput = nullptr;
            return;
        }
        getchar();
        getline(&up->userInput, &up->bufferInput, stdin);
        up->userInput[strcspn(up->userInput, "\n")] = '\0';
        char *newBuffer = (char *) malloc(strlen(up->allInputs) + strlen(up->userInput) + 1);
        int j = copyUntilIndex(up->allInputs, newBuffer, line, index);
        int m = 0;
        while (up->userInput[m] != '\0') {
            newBuffer[j++] = up->userInput[m++];
        }
        int i = 0;
        while (up->allInputs[i] != '\0') {
            newBuffer[j++] = up->allInputs[i++];
        }
        up->allInputs = (char *) realloc(up->allInputs, strlen(newBuffer) * sizeof(char));
        strcpy(up->allInputs, newBuffer);
        printf("Text was inserted.\n");
        free(newBuffer);
        newBuffer = nullptr;
        free(up->userInput);
        up->userInput = nullptr;
        up->isSaved = false;
    }

    int copyUntilIndex(char *source, char *destination, int line, int index) {
        int currentLine = 1;
        int currentIndex = 1;
        int i = 0;
        int j = 0;
        while (source[i] != '\0') {
            if (currentLine == line && currentIndex == index) {
                break;
            }
            if (destination != nullptr) {
                destination[j++] = source[i];
            }
            if (source[i] == '\n') {
                currentLine++;
                currentIndex = 1;
            } else {
                currentIndex++;
            }
            i++;
        }
        if (destination != nullptr) {
            destination[j] = '\0';
        }
        return i;
    }

    char *DeleteText(UserParams *up, int line, int index, int numSymbols) {
        SaveState(up);
        char *newBuffer = (char *) malloc(strlen(up->allInputs) + 1);
        int i = copyUntilIndex(up->allInputs, newBuffer, line, index);
        int j = i;
        char *deletedText = (char *) malloc((numSymbols + 1) * sizeof(char));
        int k = 0;
        while (numSymbols > 0 && up->allInputs[i] != '\0') {
            if (deletedText != nullptr) {
                deletedText[k++] = up->allInputs[i];
            }
            i++;
            numSymbols--;
        }
        deletedText[k] = '\0';
        while (up->allInputs[i] != '\0') {
            newBuffer[j++] = up->allInputs[i++];
        }
        newBuffer[j] = '\0';
        up->allInputs = (char *) realloc(up->allInputs, (strlen(newBuffer) + 1) * sizeof(char));
        strcpy(up->allInputs, newBuffer);
        free(newBuffer);
        newBuffer = nullptr;
        up->isSaved = false;
        return deletedText;
    }

    void Delete(UserParams *up) {
        int line, index, numSymbols;
        printf("Choose line, index and number of symbols to delete: ");
        scanf("%d %d %d", &line, &index, &numSymbols);
        char *deletedText = DeleteText(up, line, index, numSymbols);
        printf("Text was deleted.\n");
        free(deletedText);
        deletedText = nullptr;
    }

    void Undo(UserParams *up) {
        if (up->undoFirst != nullptr) {
            if (up->redoThird != nullptr) {
                free(up->redoThird);
            }
            up->redoThird = up->redoSecond;
            up->redoSecond = up->redoFirst;
            if (up->allInputs != nullptr) {
                size_t len = strnlen(up->allInputs, up->bufferInput);
                up->redoFirst = (char *) malloc((len + 1) * sizeof(char));
                strncpy(up->redoFirst, up->allInputs, len);
                up->redoFirst[len] = '\0';
            } else {
                up->redoFirst = nullptr;
            }
            free(up->allInputs);
            up->allInputs = up->undoFirst;
            up->undoFirst = up->undoSecond;
            up->undoSecond = up->undoThird;
            up->undoThird = nullptr;
            printf("Undo completed.\n");
            up->isSaved = false;
        } else {
            printf("No actions to undo.\n");
        }
    }

    void Redo(UserParams *up) {
        if (up->redoFirst != nullptr) {
            if (up->undoThird != nullptr) {
                free(up->undoThird);
            }
            up->undoThird = up->undoSecond;
            up->undoSecond = up->undoFirst;
            if (up->allInputs != nullptr) {
                size_t len = strnlen(up->allInputs, up->bufferInput);
                up->undoFirst = (char *) malloc((len + 1) * sizeof(char));
                strncpy(up->undoFirst, up->allInputs, len);
                up->undoFirst[len] = '\0';
            } else {
                up->undoFirst = nullptr;
            }
            free(up->allInputs);
            up->allInputs = up->redoFirst;
            up->redoFirst = up->redoSecond;
            up->redoSecond = up->redoThird;
            up->redoThird = nullptr;

            printf("Redo completed.\n");
            up->isSaved = false;
        } else {
            printf("No actions to redo.\n");
        }
    }

    void Cut(UserParams *up) {
        int line, index, numSymbols;
        if (up->pasteBuffer != nullptr) {
            free(up->pasteBuffer);
        }
        printf("Choose line and index and number of symbols: ");
        scanf("%d %d %d", &line, &index, &numSymbols);
        getchar();
        up->pasteBuffer = DeleteText(up, line, index, numSymbols);
        printf("Text was cut.\n");
    }

    void Copy(UserParams *up) {
        int line, index, numSymbols;
        if (up->pasteBuffer != nullptr) {
            free(up->pasteBuffer);
        }
        printf("Choose line, index and number of symbols: ");
        scanf("%d %d %d", &line, &index, &numSymbols);
        getchar();
        up->pasteBuffer = (char *) malloc((numSymbols + 1) * sizeof(char));
        int start = copyUntilIndex(up->allInputs, up->pasteBuffer, line, index);
        strncpy(up->pasteBuffer, up->allInputs + start, numSymbols);
        up->pasteBuffer[numSymbols] = '\0';
        up->isSaved = false;
        printf("Text was copied\n");
    }

    void Paste(UserParams *up) {
        SaveState(up);
        int line, index;
        printf("Choose line and index: ");
        scanf("%d %d", &line, &index);
        getchar();
        if (up->pasteBuffer == nullptr) {
            printf("There is nothing to paste!\n");
            return;
        }
        char *Buffer = (char *) malloc(strnlen(up->allInputs, up->bufferInput)
                                       + strnlen(up->pasteBuffer, up->bufferInput) + 1);
        int j = copyUntilIndex(up->allInputs, Buffer, line, index);
        int i = 0;
        while (up->pasteBuffer[i] != '\0') {
            Buffer[j++] = up->pasteBuffer[i++];
        }
        int k = copyUntilIndex(up->allInputs, nullptr, line, index);
        while (up->allInputs[k] != '\0') {
            Buffer[j++] = up->allInputs[k++];
        }
        Buffer[j] = '\0';

        up->allInputs = (char *) realloc(up->allInputs, (strnlen(Buffer, up->bufferInput) + 1) * sizeof(char));
        strncpy(up->allInputs, Buffer, up->bufferInput);
        printf("Text was pasted.\n");
        free(Buffer);
        Buffer = nullptr;
        up->isSaved = false;
    }

    void InsertWithReplacement(UserParams *up) {
        int line, index;
        printf("Choose line and index: ");
        scanf("%d %d", &line, &index);
        getchar();
        printf("Write text: ");
        up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
        fgets(up->userInput, up->bufferInput, stdin);
        up->userInput[strcspn(up->userInput, "\n")] = '\0';
        SaveState(up);
        int insertPos = copyUntilIndex(up->allInputs, NULL, line, index);
        int lengthDelete = 0;
        for (int i = insertPos; up->allInputs[i] != ' ' && up->allInputs[i] != '\0'; i++) {
            if (up->allInputs[i] == '\n') {
                break;
            }
            lengthDelete++;
        }
        char *Buffer = (char *) malloc(
            strnlen(up->allInputs, up->bufferInput) + strnlen(up->userInput, up->bufferInput) + 1 - lengthDelete);
        strncpy(Buffer, up->allInputs, insertPos);
        Buffer[insertPos] = '\0';
        strncat(Buffer, up->userInput, up->bufferInput);
        strncat(Buffer, up->allInputs + insertPos + lengthDelete, up->bufferInput);
        up->allInputs = (char *) realloc(up->allInputs, strlen(Buffer) + 1);
        strncpy(up->allInputs, Buffer, up->bufferInput);
        printf("Text was inserted with replacement.\n");
        free(Buffer);
        free(up->userInput);
        Buffer = NULL;
        up->userInput = NULL;
        up->isSaved = false;
    }

    void EncryptText(UserParams *up) {
        SaveState(up);
        int key;
        cout << "Enter the key for encryption: ";
        cin >> key;
        cin.ignore();
        cipher.Encrypt(up->allInputs, key);
        cout << "Text was encrypted." << endl;
        up->isSaved = false;
    }

    void DecryptText(UserParams *up) {
        SaveState(up);
        int key;
        cout << "Enter the key for decryption: ";
        cin >> key;
        cin.ignore();
        cipher.Decrypt(up->allInputs, key);
        cout << "Text was decrypted." << endl;
        up->isSaved = false;
    }
};

class FileHandler {
    UserParams up;

private:
    bool CheckUnsavedChanges(UserParams *up) {
        if (!up->isSaved && up->allInputs != nullptr) {
            char dataAnswer[20];
            while (true) {
                printf("Do you really want to load a new file? (Yes/No) You have unsaved text: ");
                scanf("%s", dataAnswer);
                for (int i = 0; dataAnswer[i]; i++) {
                    dataAnswer[i] = tolower(dataAnswer[i]);
                }
                if (strcmp(dataAnswer, "yes") == 0) {
                    free(up->allInputs);
                    up->allInputs = nullptr;
                    return true;
                } else {
                    return false;
                }
            }
        }
        return true;
    }

public:
    void SaveFile(UserParams *up) {
        if (up->allInputs == nullptr) {
            printf("There is no text to save!\n");
            return;
        }
        printf("Enter a title for file: ");
        up->userInput = (char *) malloc(up->bufferInput * sizeof(char));
        scanf("%s", up->userInput);

        FILE *file = fopen(up->userInput, "w");
        if (file == nullptr) {
            printf("Error opening file %s for writing.\n", up->userInput);
            free(up->userInput);
            up->userInput = nullptr;
            return;
        }
        if (up->allInputs != nullptr) {
            fputs(up->allInputs, file);
        }
        fclose(file);
        printf("File created and text saved!\n");
        up->isSaved = true;

        free(up->userInput);
        up->userInput = nullptr;
    }

    void LoadFromFile(UserParams *up) {
        if (!CheckUnsavedChanges(up)) {
            return;
        }
        printf("Enter a file that you want to load info from: ");
        char fileInput[256];
        scanf("%s", fileInput);

        FILE *file = fopen(fileInput, "r");
        if (file == nullptr) {
            printf("Error opening file %s for reading.\n", fileInput);
            return;
        }
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        rewind(file);
        if (up->allInputs != nullptr) {
            free(up->allInputs);
            up->allInputs = nullptr;
        }

        up->userInput = (char *) realloc(up->userInput, (fileSize + 1) * sizeof(char));
        if (fread(up->userInput, sizeof(char), fileSize, file) == fileSize) {
            up->userInput[fileSize] = '\0';
            up->allInputs = (char *) malloc((fileSize + 1) * sizeof(char));
            strcpy(up->allInputs, up->userInput);

            fclose(file);
            printf("File content loaded.\n");
            up->isSaved = false;
        } else {
            printf("Error reading file %s.\n", fileInput);
            fclose(file);
        }
    }
};

class CaesarFiles {
    CaesarCipher cipher;
    UserParams up;

    class FileData {
    private:
        char *content;
        long size;

    public:
        FileData(char *content = nullptr, long size = 0) : content(content), size(size) {
        }

        ~FileData() {
            if (content) {
                free(content);
                content = nullptr;
            }
        }

        char *getContent() const { return content; }
        long getSize() const { return size; }
    };

private:
    FileData LoadFileInfo(const char *filePath) {
        FILE *file = fopen(filePath, "rb");
        if (!file) {
            cout << "Error opening file " << filePath << endl;
            return FileData();
        }

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        rewind(file);

        char *fileContent = (char *) malloc(fileSize + 1);
        if (fread(fileContent, 1, fileSize, file) != fileSize) {
            cout << "Error reading file " << filePath << endl;
            fclose(file);
            free(fileContent);
            fileContent = nullptr;
            return FileData();
        }

        fileContent[fileSize] = '\0';
        fclose(file);
        return FileData(fileContent, fileSize);
    }

    bool SaveFileInfo(char *filePath, char *content, long size) {
        FILE *file = fopen(filePath, "wb");
        if (!file) {
            cout << "Error opening file " << filePath << endl;
            return false;
        }

        if (fwrite(content, 1, size, file) != size) {
            cout << "Error writing to file " << filePath << endl;
            fclose(file);
            return false;
        }

        fclose(file);
        return true;
    }

    void ProcessFile(UserParams *up, char *inputFilePath, char *outputFilePath, int key, bool encrypt) {
        FileData fileText = LoadFileInfo(inputFilePath);
        if (fileText.getContent() != nullptr) {
            long fileSize = fileText.getSize();
            long textSize = 0;
            up->processed = (char *) malloc((fileSize + 1) * sizeof(char));

            while (textSize < fileSize) {
                long remainingSize = fileSize - textSize;
                long chunkSize;
                if (remainingSize > 128) {
                    chunkSize = 128;
                } else {
                    chunkSize = remainingSize;
                }
                char buffer[128];
                memcpy(buffer, fileText.getContent() + textSize, chunkSize);
                cout << "Processing chunk of size: " << chunkSize << " at: " << textSize << endl;

                if (encrypt) {
                    cipher.EncryptChunk(buffer, key);
                } else {
                    cipher.DecryptChunk(buffer, key);
                }

                memcpy(up->processed + textSize, buffer, chunkSize);
                textSize += chunkSize;
            }

            if (SaveFileInfo(outputFilePath, up->processed, fileSize)) {
                if (encrypt) {
                    cout << "File encrypted and saved as " << outputFilePath << endl;
                } else {
                    cout << "File decrypted and saved as " << outputFilePath << endl;
                }
            }

            free(up->processed);
            up->processed = nullptr;
        }
    }

public:
    void EncryptFile(UserParams *up) {
        char inputFile[256];
        char outputFile[256];
        cout << "Enter input file path: ";
        cin >> inputFile;
        cin.ignore();
        cout << "Enter output file path: ";
        cin >> outputFile;
        cin.ignore();
        int key;
        cout << "Enter the key for encryption: ";
        cin >> key;
        cin.ignore();

        ProcessFile(up, inputFile, outputFile, key, true);
    }

    void DecryptFile(UserParams *up) {
        char inputFile[256];
        char outputFile[256];
        cout << "Enter input file path: ";
        cin >> inputFile;
        cin.ignore();
        cout << "Enter output file path: ";
        cin >> outputFile;
        cin.ignore();
        int key;
        cout << "Enter the key for decryption: ";
        cin >> key;
        cin.ignore();

        ProcessFile(up, inputFile, outputFile, key, false);
    }
};


class CommandsRunner {
    UserParams up;
    FileHandler files;
    TextEditor editor;
    CaesarFiles cipherFile;

public:
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
        DELETE = 9,
        UNDO = 10,
        REDO = 11,
        CUT = 12,
        COPY = 13,
        PASTE = 14,
        INSERT_WITH_REPLACEMENT = 15,
        ENCRYPT = 16,
        DECRYPT = 17,
        ENCRYPT_FILE = 18,
        DECRYPT_FILE = 19,
        EXIT = 20
    };

    static void CommandParser(int *command) {
        printf("Which command do you want to use? (0 - Help): ");
        while (scanf("%d", command) != 1 || *command < COMMAND_HELP || *command > EXIT) {
            printf("Invalid input! Please enter a valid command (0 - 18): ");
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
            case DELETE:
                editor.Delete(up);
                break;
            case UNDO:
                editor.Undo(up);
                break;
            case REDO:
                editor.Redo(up);
                break;
            case CUT:
                editor.Cut(up);
                break;
            case COPY:
                editor.Copy(up);
                break;
            case PASTE:
                editor.Paste(up);
                break;
            case INSERT_WITH_REPLACEMENT:
                editor.InsertWithReplacement(up);
                break;
            case ENCRYPT:
                editor.EncryptText(up);
                break;
            case DECRYPT:
                editor.DecryptText(up);
                break;
            case ENCRYPT_FILE:
                cipherFile.EncryptFile(up);
                break;
            case DECRYPT_FILE:
                cipherFile.DecryptFile(up);
                break;
            case EXIT:
                editor.Clear(up);
                printf("Thanks for using this program. Bye!\n");
                break;
            default:
                printf("Unknown command.\n");
                break;
        }
    }

    void Run() {
        int command = 0;
        do {
            CommandParser(&command);
            CommandRunner(command, &up);
        } while (command != EXIT);
    }
};

int main() {
    CommandsRunner cmd;
    cmd.Run();
    return 0;
}
