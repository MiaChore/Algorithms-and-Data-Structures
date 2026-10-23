//Крылов М. И. ПС-22

//Задание № 22
//Имеется некоторое  алгебраическое выражение с операциями '+', '-', '*', '/', '^', SIN, COS, EXP. Возможен унарный минус.
//Операнды  заданы  идентификаторами.  Операции   выполняются  по приоритетам, допускается использование круглых скобок. Построить
//и выдать в наглядном виде бинарное дерево, представляющее данное выражение. Запросить в диалоге значения переменных  и  вычислить
//выражение

// 11 баллов

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define maxText 5001
#define maxNodes 5000
#define maxDepth 300

typedef struct {
    char operation;
    int left;
    int right;
} Node;
Node tree[maxNodes];
char expression[maxText];
int nodeCount = 0;
int root = -1;
bool used[26];
double values[26];

bool isFunction(int operation) {
    return operation == 'S' || operation == 'C' || operation == 'E';
}

const char *functionName(int operation) {
    if (operation == 'S') return "SIN";
    if (operation == 'C') return "COS";
    return "EXP";
}

int treeError(const char message[]) {
    printf("Ошибка: %s\n", message);
    return -1;
}

int newNode(char operation, int left, int right) {
    if (nodeCount == maxNodes) {
        return treeError("Превышено допустимое количество узлов");
    }
    int index = nodeCount++;

    tree[index].operation = operation;
    tree[index].left = left;
    tree[index].right = right;

    return index;
}

bool isUnaryMinus(int index, int start) {
    if (index == start) {
        return true;
    }
    char previous = expression[index - 1];

    return previous == '(' || previous == '+' || previous == '-' || previous == '*' || previous == '/' || previous == '^';
}

int buildTree(int start, int end, int depth) {
    if (start > end) {
        return treeError("Отсутствует аргумент операции");
    }
    if (depth > maxDepth) {
        return treeError("Превышена допустимая глубина дерева");
    }

    while (expression[start] == '(' && expression[end] == ')') {
        int balance = 0;
        int closing = start;

        for (; closing <= end; closing++) {
            if (expression[closing] == '(') balance++;
            if (expression[closing] == ')') balance--;

            if (balance == 0) break;
        }

        if (closing != end) break;

        start++;
        end--;

        if (start > end) return treeError("Пустые скобки");
    }

    int split = -1;

    for (int pass = 0; pass < 2 && split == -1; pass++) {
        int balance = 0;

        for (int i = end; i >= start; i--) {
            char symbol = expression[i];

            if (symbol == ')') {
                balance++;
            } else if (symbol == '(') {
                balance--;
            } else if (balance == 0) {
                bool suitable;

                if (pass == 0) {
                    suitable = symbol == '+' || (symbol == '-' && !isUnaryMinus(i, start));
                } else {
                    suitable = symbol == '*' || symbol == '/';
                }

                if (suitable) {
                    split = i;
                    break;
                }
            }
        }
    }

    if (split != -1) {
        int left = buildTree(start, split - 1, depth + 1);

        if (left == -1) return -1;

        int right = buildTree(split + 1, end, depth + 1);

        if (right == -1) return -1;

        return newNode(expression[split], left, right);
    }

    if (expression[start] == '-') {
        int argument = buildTree(start + 1, end, depth + 1);

        if (argument == -1) return -1;

        return newNode('~', -1, argument);
    }

    int balance = 0;

    for (int i = start; i <= end; i++) {
        char symbol = expression[i];

        if (symbol == '(') {
            balance++;
        } else if (symbol == ')') {
            balance--;
        } else if (balance == 0 && symbol == '^') {
            int left = buildTree(start, i - 1, depth + 1);

            if (left == -1) return -1;

            int right = buildTree(i + 1, end, depth + 1);

            if (right == -1) return -1;

            return newNode('^', left, right);
        }
    }

    if (expression[start] >= 'A' && expression[start] <= 'Z') {
        char operation = 0;

        if (end - start >= 2) {
            if (strncmp(expression + start, "SIN", 3) == 0) operation = 'S';
            if (strncmp(expression + start, "COS", 3) == 0) operation = 'C';
            if (strncmp(expression + start, "EXP", 3) == 0) operation = 'E';
        }

        if (operation == 0) {
            return treeError("Неизвестная функция: допустимы SIN, COS и EXP");
        }
        if (end - start < 4 || expression[start + 3] != '(' || expression[end] != ')') {
            return treeError("После функции требуется аргумент в круглых скобках");
        }

        int argument = buildTree(start + 3, end, depth + 1);

        if (argument == -1) return -1;

        return newNode(operation, -1, argument);
    }

    if (start == end &&
        expression[start] >= 'a' &&
        expression[start] <= 'z') {

        return newNode(expression[start], -1, -1);
    }

    return treeError("Пропущена операция или неверно записан аргумент");
}

void skipInput(void) {
    int symbol;

    while ((symbol = getchar()) != '\n' && symbol != EOF) {
    }
}

void loadExpression(void) {
    char fileName[500];

    printf("Имя файла с выражением: ");

    if (fgets(fileName, sizeof fileName, stdin) == NULL) {
        return;
    }

    int length = 0;

    while (fileName[length] != '\0' && fileName[length] != '\n') {
        length++;
    }

    if (fileName[length] != '\n' && !feof(stdin)) {
        skipInput();
        printf("Ошибка: слишком длинное имя файла\n");
        return;
    }

    fileName[length] = '\0';

    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
        printf("Ошибка: невозможно открыть файл\n");
        return;
    }

    root = -1;
    nodeCount = 0;
    length = 0;

    int symbol;
    while ((symbol = fgetc(file)) != EOF) {
        if (symbol == ' ' || symbol == '\t' || symbol == '\r' || symbol == '\n') {
            continue;
        }

        if (length == maxText - 1) {
            printf("Ошибка: выражение слишком длинное\n");
            fclose(file);
            return;
        }

        bool allowed = (symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z') || symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/' || symbol == '^' || symbol == '(' || symbol == ')';

        if (!allowed) {
            printf("Ошибка: недопустимый символ в выражении\n");
            fclose(file);
            return;
        }

        expression[length++] = (char)symbol;
    }

    if (ferror(file)) {
        printf("Ошибка: не удалось прочитать файл\n");
        fclose(file);
        return;
    }

    fclose(file);
    expression[length] = '\0';

    if (length == 0) {
        printf("Ошибка: выражение пустое\n");
        return;
    }

    int balance = 0;

    for (int i = 0; i < length; i++) {
        if (expression[i] == '(') balance++;
        if (expression[i] == ')') balance--;

        if (balance < 0) {
            printf("Ошибка: лишняя закрывающая скобка\n");
            return;
        }
    }

    if (balance > 0) {
        printf("Ошибка: не закрыта скобка\n");
        return;
    }

    root = buildTree(0, length - 1, 0);

    if (root == -1) {
        return;
    }

    for (int i = 0; i < 26; i++) {
        used[i] = false;
    }

    for (int i = 0; i < nodeCount; i++) {
        char operation = tree[i].operation;

        if (operation >= 'a' && operation <= 'z') {
            used[operation - 'a'] = true;
        }
    }

    printf("Выражение загружено, дерево построено\n");
}

void showTree(int index, int depth) {
    if (index == -1) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        putchar('.');
    }

    char operation = tree[index].operation;

    if (isFunction(operation)) {
        printf("%s\n", functionName(operation));
    } else {
        printf("%c\n", operation == '~' ? '-' : operation);
    }

    showTree(tree[index].left, depth + 1);
    showTree(tree[index].right, depth + 1);
}

bool askValues(void) {
    char input[101];

    for (int i = 0; i < 26; i++) {
        if (!used[i]) continue;

        while (true) {
            printf("Введите значение %c: ", 'a' + i);

            if (fgets(input, sizeof input, stdin) == NULL) {
                return false;
            }

            if (strchr(input, '\n') == NULL && !feof(stdin)) {
                skipInput();
                printf("Ошибка: слишком длинный ввод\n");
                continue;
            }

            if (sscanf(input, "%lf", &values[i]) == 1 && isfinite(values[i])) {
                break;
            }

            printf("Ошибка: введите конечное число\n");
        }
    }

    return true;
}

bool calculate(int index, double *result) {
    char operation = tree[index].operation;

    if (operation >= 'a' && operation <= 'z') {
        *result = values[operation - 'a'];
        return true;
    }

    double left = 0;
    double right;

    if (tree[index].left != -1 && !calculate(tree[index].left, &left)) {
        return false;
    }

    if (!calculate(tree[index].right, &right)) {
        return false;
    }

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
            return false;
    }

    if (!isfinite(*result)) {
        printf("Ошибка вычисления: недопустимые аргументы или слишком большой результат\n");
        return false;
    }

    return true;
}

int main(void) {
    while (true) {
        printf("\n1 — Загрузить выражение из файла\n"
               "2 — Показать дерево\n"
               "3 — Ввести значения и вычислить\n\n"
               "0 — Выйти\n"
               "Ваш выбор: ");

        int choice = getchar();

        if (choice == EOF) return 0;
        if (choice != '\n') skipInput();
        if (choice == '0') return 0;

        if (choice == '1') {
            loadExpression();
        } else if (choice >= '2' && choice <= '3') {
            if (root == -1) {
                printf("Сначала загрузите выражение\n");
                continue;
            }

            if (choice == '2') {
                showTree(root, 0);
            } else if (choice == '3') {
                if (askValues()) {
                    double result;

                    if (calculate(root, &result)) {
                        printf("Результат: %.15g\n", result);
                    }
                }
            }
        } else {
            printf("Выберите пункт от 0 до 3\n");
        }
    }
}
