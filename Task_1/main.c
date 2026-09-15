//Крылов М. И. ПС-22
//Задание № 10
//Некоторый текст состоит из нескольких частей, записанных в отдельных файлах.
//Имена этих файлов и общий заголовок текста указаны в отдельном  файле.
//Создать файл с полным текстом. Заголовок должен содержаться в центре первой строки
// 6 баллов

#include <stdio.h>
#include <stdbool.h>

#define maxName 401
#define lineWidth 80

void removeNewLineChar(char text[]) {
    int i = 0;
    while (text[i] != '\0') {
        if (text[i] == '\n') {
            text[i] = '\0';
            break;
        }
        i++;
    }
}

bool readFileName(char name[]) {
    if (fgets(name, maxName, stdin) == NULL) {
        printf("Ошибка: не удалось прочитать имя файла\n");
        return false;
    }

    int length = 0;

    while (name[length] != '\0') {
        length++;
    }

    if (length == maxName - 1 && name[length - 1] != '\n') {
        int symbol = getchar();

        if (symbol != '\n' && symbol != EOF) {
            while (symbol != '\n' && symbol != EOF) {
                symbol = getchar();
            }

            printf("Ошибка: имя длиннее %d символов\n", maxName - 1);
            return false;
        }

        if (ferror(stdin)) {
            printf("Ошибка чтения имени файла: %s\n", name);
            return false;
        }
    }

    removeNewLineChar(name);
    return true;
}

bool startIOReading(char input[], char output[]) {
    printf("Введите имя файла с содержанием: ");

    if (!readFileName(input)) {
        return false;
    }

    printf("Введите имя выходного файла: ");

    if (!readFileName(output)) {
        return false;
    }

    return true;
}

bool fileErrorCompiler(FILE *file, char fileName[]) {
    if (file == NULL) {
        printf("Ошибка: Не существует или невозможно открыть файл %s\n", fileName);

        return false;
    } else {
        printf("Успешно: файл %s открыт\n", fileName);

        return true;
    }
}

bool namesErrorCompiler(char inputName[], char outputName[]) {
    bool errorCheck = true;
    int i = 0;
    while (inputName[i] != '\0' && inputName[i] == outputName[i] && outputName[i] != '\0') {
        i++;
    }
    if (inputName[i] == outputName[i]) {
        printf("Ошибка: Названия файлов должно отличаться\n");

        errorCheck = false;
    } 
    
    if (inputName[0] == '\0') {
        printf("Ошибка: Имя файла с контентом не может быть пустым\n");
        errorCheck = false;
    }
    if (outputName[0] == '\0') {
        printf("Ошибка: Имя файла с результатом не может быть пустым\n");
        errorCheck = false;
    }
    if (errorCheck) {
        printf("Успешно: Проверка названий файлов прошла без ошибок\n");
            
        return errorCheck;
    } else {
        printf("Ошибка: Работа завершена. Исправь ошибки и повтори попытку, ма бой\n");
        return errorCheck;
    }
}

void centralPrint(int nameLength, char text[], FILE *outputFile) {
    int spaces = (lineWidth - nameLength) / 2;

    while (spaces > 0) {
        fputc(' ', outputFile);
        spaces--;
    }

    fputs(text, outputFile);
}

void printTitle(FILE *inputFile, FILE *outputFile) {
    char title[lineWidth + 1];
    int length = 0;
    int symbol;
    bool reseted = false;

    while ((symbol = fgetc(inputFile)) != EOF && symbol != '\n') {
        title[length] = (char)symbol;
        length++;
        if (length == lineWidth) {
            title[length] = '\0';
            centralPrint(length, title, outputFile);
            fputc('\n', outputFile);
            length = 0;
            title[0] = '\0';
            reseted = true;
        }
    }


    if (length > 0) {
        title[length] = '\0';
        centralPrint(length, title, outputFile);
        fputc('\n', outputFile);
        printf("Заголовок выведен\n");
    } else if (!reseted) {
        fputs("( Заголовок не найден )\n", outputFile);
        printf("Заголовок не найден\n");
    }

    fputc('\n', outputFile);
}

bool printParts(FILE *inputFile, FILE *outputFile) {
    int partNumber = 0;
    int lineLength = 0;
    bool hasPartRecord = false;
    bool success = true;
    char partName[maxName];

    while (fgets(partName, maxName, inputFile) != NULL) {
        int length = 0;
        hasPartRecord = true;

        while (partName[length] != '\0') {
            length++;
        }

        if (length == maxName - 1 && partName[length - 1] != '\n') {
            int symbol = fgetc(inputFile);

            if (symbol != '\n' && symbol != EOF) {
                while (symbol != '\n' && symbol != EOF) {
                    symbol = fgetc(inputFile);
                }
                if (lineLength > 0) {
                    fputc('\n', outputFile);
                }
                fputs("( Название файла слишком длинное )\n\n", outputFile);
                success = false;
                lineLength = 0;
                continue;
            }
        }
        removeNewLineChar(partName);

        FILE *part = fopen(partName, "r");

        if (part == NULL) {
            success = false;
            if (lineLength > 0) {
                fputc('\n', outputFile);
            }
            fputs("\n( Часть текста не найдена )\n\n", outputFile);
            lineLength = 0;
        } else {
            int symbol;
            
            while ((symbol = fgetc(part)) != EOF) {
                if (symbol != '\n' && lineLength == lineWidth) {
                    if (fputc('\n', outputFile) == EOF) {
                        break;
                    }
                    lineLength = 0;
                }
                if (fputc(symbol, outputFile) == EOF) {
                    break;
                }

                if (symbol == '\n') {
                    lineLength = 0;
                } else {
                    lineLength++;
                }
            }
            if (ferror(outputFile)) {
                printf("Ошибка записи результата. Сборка прервана.\n");
                fclose(part);
                return false;
            }
            if (ferror(part)) {
                printf("Ошибка чтения части: %s\n", partName);
                success = false;
                fputs("\n( Ошибка чтения части текста )\n\n", outputFile);
                lineLength = 0;
                continue;
            }
            if (fclose(part) == EOF) {
                printf("Ошибка закрытия части: %s\n", partName);
                success = false;
                fclose(part);
                continue;
            }

            partNumber++;
            printf("Часть %d добавлена в результат\n", partNumber);
        }
    }
    if (!hasPartRecord && !ferror(inputFile)) {
        fputs("( Текст не найден )\n", outputFile);
        success = false;
    }

    if (ferror(inputFile) || ferror(outputFile)) {
        success = false;
    }

    return success;
}

int assembleText(FILE *inputFile, FILE *outputFile) {
    bool readError = true;
    bool writeError = true;

    printTitle(inputFile, outputFile);

    if (ferror(inputFile)) {
        printf("Ошибка чтения заголовка\n");
        readError = false;
    }

    if (ferror(outputFile)) {
        writeError = false;
    }

    if (readError && writeError) {
        if (!printParts(inputFile, outputFile)) {
            readError = false;
        }

        if (ferror(inputFile)) {
            printf("Ошибка чтения списка частей\n");
            readError = false;
        }

        if (ferror(outputFile)) {
            writeError = false;
        }
    }

    if (fclose(inputFile) == EOF) {
        printf("Ошибка закрытия входного файла\n");
        readError = false;
    }

    if (fclose(outputFile) == EOF) {
        writeError = false;
    }

    if (!writeError) {
        printf("Ошибка: результат сохранён не полностью\n");
    }

    if (!readError || !writeError) {
        return 1;
    }

    return 0;
}

int main(void) {
    char inputName[maxName];
    char outputName[maxName];
    bool readFileError = true;
    readFileError = startIOReading(inputName, outputName);
    if (!readFileError || !namesErrorCompiler(inputName, outputName)) {
        return 1;
    }
    FILE *contents = fopen(inputName, "r");
    if (!fileErrorCompiler(contents, inputName)) {
        return 1;
    }
    FILE *result = fopen(outputName, "w");
    if (!fileErrorCompiler(result, outputName)) {
        fclose(contents);
        return 1;
    }
    printf("\n");

    return assembleText(contents, result);
}
