//Крылов М. И. ПС-22
//Задание № 24

//Имеется выражение в постфиксной форме, включающее операции '+', '-', '*', '/', '^', ~ (одноместный минус),  
//SIN, COS, EXP.  Переменные  заданы одной строчной латинской буквой. Запросить значения переменных и вычислить 
//выражение. Представить его в инфиксной форме со скобками. Лишние скобки присутствовать не должны 

// 11 баллов

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define stackSize 100
#define textSize 1000

void resetFile(double values[], bool used[]);

void resetValues(double values[], bool used[]) {
    for (int i = 0; i < 26; i++) {
        values[i] = 0;
        used[i] = false;
    }
}

int priority(int operation) {
    if (operation == '+' || operation == '-') return 1;
    if (operation == '*' || operation == '/') return 2;
    if (operation == '~') return 3;
    if (operation == '^') return 4;

    return 5;
}

bool needBrackets(int parent, int child, bool rightSide) {
    int parentPriority = priority(parent);
    int childPriority = priority(child);

    if (childPriority < parentPriority) return true;
    if (childPriority > parentPriority) return false;
    if (parent == '^') {
        return !rightSide;
    }
    if (parent == '~') {
        return true;
    }
    if (rightSide && (parent == '-' || parent == '/')) {
        return true;
    }

    return false;
}

bool makeExpression(int operation, const char left[], int leftOperation, const char right[], int rightOperation, char output[]) {
    int length;

    if (operation == 'S' || operation == 'C' || operation == 'E') {
        const char *name;

        if (operation == 'S') {
            name = "SIN";
        } else if (operation == 'C') {
            name = "COS";
        } else {
            name = "EXP";
        }

        length = snprintf(output, textSize, "%s(%s)", name, right);
    } else if (operation == '~') {
        bool brackets = needBrackets(operation, rightOperation, true);

        length = snprintf(output, textSize, "-%s%s%s", brackets ? "(" : "", right, brackets ? ")" : "");
    } else {
        bool leftBrackets = needBrackets(operation, leftOperation, false);

        bool rightBrackets = needBrackets(operation, rightOperation, true);

        length = snprintf(output, textSize, "%s%s%s %c %s%s%s", leftBrackets ? "(" : "", left, leftBrackets ? ")" : "", 
            operation, rightBrackets ? "(" : "", right, rightBrackets ? ")" : "");
    }

    return length >= 0 && length < textSize;
}

bool calculate(int operation, double left, double right, double *result) {
    switch (operation) {
        case '+':
            *result = left + right;
            break;
        case '-':
            *result = left - right;
            break;
        case '*':
            *result = left * right;
            break;
        case '/':
            if (right == 0) {
                printf("Ошибка: деление на ноль\n");
                return false;
            }
            *result = left / right;
            break;
        case '^':
            *result = pow(left, right);
            break;
        case '~':
            *result = -right;
            break;
        case 'S':
            *result = sin(right);
            break;
        case 'C':
            *result = cos(right);
            break;
        case 'E':
            *result = exp(right);
            break;
        default:
            printf("Ошибка: неизвестная операция\n");
            return false;
    }
    if (!isfinite(*result)) {
        printf("Ошибка вычисления: недопустимые аргументы или слишком большой результат\n");
        return false;
    }
    return true;
}

bool addOperation(int operation, double stack[], char expressions[][textSize], int operations[], int *count) {
    bool unary = operation == '~' || operation == 'S' || operation == 'C' || operation == 'E';
    int required = unary ? 1 : 2;

    if (*count < required) {
        printf("Ошибка: недостаточно аргументов для операции\n");
        return false;
    }

    int rightIndex = *count - 1;
    int leftIndex = *count - required;
    double leftValue = 0;
    const char *leftText = "";
    int leftOperation = 0;

    if (!unary) {
        leftValue = stack[leftIndex];
        leftText = expressions[leftIndex];
        leftOperation = operations[leftIndex];
    }

    double result;

    if (!calculate(operation, leftValue, stack[rightIndex], &result)) {
        return false;
    }

    char newText[textSize];

    if (!makeExpression(operation, leftText, leftOperation, expressions[rightIndex], operations[rightIndex], newText)) {
        printf("Ошибка: запись выражения длиннее %d символов\n", textSize - 1);
        return false;
    }

    stack[leftIndex] = result;
    operations[leftIndex] = operation;

    strcpy(expressions[leftIndex], newText);

    *count = leftIndex + 1;
    return true;
}

int readMathOperation(FILE *file, int symbol) {
    if (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/' || symbol == '^' || symbol == '~') {
        return symbol;
    }

    if (symbol >= 'A' && symbol <= 'Z') {
        char word[4];
        int length = 0;
        bool tooLong = false;

        while (symbol >= 'A' && symbol <= 'Z') {
            if (length < 3) {
                word[length++] = (char)symbol;
            } else {
                tooLong = true;
            }

            symbol = fgetc(file);
        }

        if (symbol != EOF && ungetc(symbol, file) == EOF) {
            return -1;
        }

        if (ferror(file) || tooLong || length != 3) {
            return -1;
        }

        word[length] = '\0';

        if (strcmp(word, "SIN") == 0) return 'S';
        if (strcmp(word, "COS") == 0) return 'C';
        if (strcmp(word, "EXP") == 0) return 'E';

        return -1;
    }

    return -1;
}

void doMath(FILE *file, double values[]) {
    double stack[stackSize];
    char expressions[stackSize][textSize];
    int operations[stackSize];
    int count = 0;

    if (fseek(file, 0, SEEK_SET) != 0) {
        printf("Ошибка: невозможно вернуться к началу файла\n");
        return;
    }

    int symbol;

    while ((symbol = fgetc(file)) != EOF && symbol != '\n') {
        if (symbol == ' ' || symbol == '\t' || symbol == '\r') {
            continue;
        }

        if (symbol >= 'a' && symbol <= 'z') {
            int index = symbol - 'a';

            if (!isfinite(values[index])) {
                printf("Ошибка: у переменной %c нет конечного значения\n", symbol);
                return;
            }

            if (count == stackSize) {
                printf("Ошибка: стек заполнен\n");
                return;
            }

            stack[count] = values[index];

            expressions[count][0] = (char)symbol;
            expressions[count][1] = '\0';

            operations[count] = 0;
            count++;
        } else {
            int operation = readMathOperation(file, symbol);

            if (operation == -1) {
                printf("Ошибка: неизвестный элемент или ошибка чтения\n");
                return;
            }

            if (!addOperation(operation, stack, expressions, operations, &count)) {
                return;
            }
        }
    }

    if (ferror(file)) {
        printf("Ошибка чтения выражения\n");
        return;
    }

    if (count != 1) {
        printf("Ошибка: выражение пустое или содержит лишние операнды\n");
        return;
    }

    printf("Инфиксная форма: %s\n", expressions[0]);
    printf("Результат: %g\n", stack[0]);
}

bool setValue(double values[], int symbol, int index) {
    while (true) {
        printf("Введите значение %c: ", symbol);
        int result = scanf("%lf", &values[index]);
        if (result == EOF) {
            return false;
        }
        int extra;
        while ((extra = getchar()) != '\n' && extra != EOF) {
            //снова скип хлама
        }
        if (result == 1) {
            return true;
        }
    }
}

void readExample(FILE *file, double values[], bool used[]) {
    int symbol;

    while ((symbol = fgetc(file)) != EOF && symbol != '\n') {
        if (symbol >= 'a' && symbol <= 'z') {
            used[symbol - 'a'] = true;
        }
    }

    if (ferror(file)) {
        printf("Ошибка чтения выражения\n");
        return;
    }

    for (int i = 0; i < 26; i++) {
        if (used[i]) {
            if (!setValue(values, 'a' + i, i)) {
                return;
            }
        }
    }

    doMath(file, values);
}

void resetFile(double values[], bool used[]) {
    while (true) {
        printf("\nВпишите выражение в example.txt\n");
        printf("Y — вычислить, N — выйти: ");

        int conf = getchar();

        if (conf == EOF) {
            return;
        }

        if (conf != '\n') {
            int glyph;

            while ((glyph = getchar()) != '\n' && glyph != EOF) {
                //чист скип остатка
            }
        }

        if (conf != 'y' && conf != 'Y') {
            printf("Введите Y или N\n");
            continue;
        }

        FILE *example = fopen("example.txt", "r");

        if (example == NULL) {
            printf("Ошибка: невозможно открыть example.txt\n");
            continue;
        }

        resetValues(values, used);
        readExample(example, values, used);

        fclose(example);
    }
}

int main(void) {
    double values[26];
    bool usedValues[26];
    printf("\nДобро пожаловать в калькулятор Abudab67MegaLulz\n\n");
    resetFile(values, usedValues); 
}
