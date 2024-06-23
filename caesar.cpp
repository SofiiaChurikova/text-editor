#include <cctype>
#include <cstring>
#include <iostream>

using namespace std;

extern "C" {
char *Encrypt(char *rawText, int key) {
    int length = strlen(rawText);
    char *encryptedText = new char[length + 1];

    for (int i = 0; i < length; i++) {
        char element = rawText[i];

        if (isalpha(element)) {
            if (islower(element)) {
                encryptedText[i] = (element - 'a' + key) % 26;
                if (encryptedText[i] < 0) {
                    encryptedText[i] += 26;
                }
                encryptedText[i] += 'a';
            } else {
                encryptedText[i] = (element - 'A' + key) % 26;
                if (encryptedText[i] < 0) {
                    encryptedText[i] += 26;
                }
                encryptedText[i] += 'A';
            }
        } else {
            encryptedText[i] = element;
        }
    }
    encryptedText[length] = '\0';
    return encryptedText;
}

char *Decrypt(char *encryptedText, int key) {
    int length = strlen(encryptedText);
    char *decryptedText = new char[length + 1];

    for (int i = 0; i < length; i++) {
        char element = encryptedText[i];

        if (isalpha(element)) {
            if (islower(element)) {
                decryptedText[i] = (element - 'a' - key) % 26;
                if (decryptedText[i] < 0) {
                    decryptedText[i] += 26;
                }
                decryptedText[i] += 'a';
            } else {
                decryptedText[i] = (element - 'A' - key) % 26;
                if (decryptedText[i] < 0) {
                    decryptedText[i] += 26;
                }
                decryptedText[i] += 'A';
            }
        } else {
            decryptedText[i] = element;
        }
    }
    decryptedText[length] = '\0';
    return decryptedText;
}
}