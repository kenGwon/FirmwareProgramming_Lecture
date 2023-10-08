/*
*** �ڷᱸ�� stack�� Ȱ�� �Ͽ� ���� ����� ****

�Ʒ��� ������ ����ϴ� ���α׷��� C��� �ڷᱸ�� stack�� Ȱ�� �Ͽ� ���α׷��� �ۼ� �Ͻÿ�
����: ���� �� double type���� �����Ѵ�.

1. �Ʒ��� ������ fgets(inbuff, 100, stdin)���� �Է��� �޾� ���� ��
(( 222 + 4 ) * 55 ) - 100 / 7 * 5 - 5 * 10 =

2. ��� ����� �Ľ�(�����м�)�Ͽ� ���� ǥ���(��ǻ�Ͱ� ����ϱ⿡ ���� ������ ���� 222 4 +)
 ���� stack�� ���� �� �� ��� �ϴ� ���� ���α׷��� �ۼ� �Ѵ�.

����� ��� ����� 12418.571428571
*/

/*
�� ���α׷��� ���, �Է� "(( 222 + 4 ) * 55 ) - 100 / 7 * 5 - 5 * 10 =" ����
������ "="�� �Է����� ������ �ȵ�.
"(( 222 + 4 ) * 55 ) - 100 / 7 * 5 - 5 * 10" �̷� ���·� ���;� ��.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define debug

typedef struct stack
{
    int top;
    int *item_arr;
    int size;
} t_Stack;

void initStack(t_Stack *stack, int size);
bool isEmpty(t_Stack *stack);
void push(t_Stack *stack, int item);
int pop(t_Stack *stack);
int peek(t_Stack *stack);
void freeStack(t_Stack *stack);

int checkOperatorPeriority(int op);
void eliminateCharFromString(char *str, char ch);
bool runCalculator(void);
char *getExpression(void);
char *parshingToPostfix(char *expreesion);
double calculateExpression(char *expression);

int main()
{
    while (1)
    {
        if (!runCalculator())
        {
            printf("StackCalculator�� �����մϴ�.\n");
            break;
        }
    }

    return 0;
}

bool runCalculator(void)
{
    char *expression;
    double result;

    printf("������ �Է��ϼ���(����: ctrl +z): ");
    expression = getExpression();

    if (expression == NULL)
    {
        return false;
    }

    expression = parshingToPostfix(expression);
    result = calculateExpression(expression);

    printf("= %.4f\n\n", result);

    return true;
}

char *parshingToPostfix(char *expression)
{
    t_Stack stack;
    unsigned int stack_size, i;
    // char original_expression[300];
    char postfix_expression[300];
    char *result;

    stack_size = 100;
    initStack(&stack, stack_size);

    // memset(original_expression, 0, sizeof(char) * 300);
    // memset(postfix_expression, '\0', sizeof(char) * 300);

    printf("*expression: %c\n", *expression); // �켱 �°� ����

    i = 0;
    while(*expression != '\0')
    {
        if (*expression == '(')
        {
            push(&stack, *expression);
            expression++;
        }
        else if (*expression == ')')
        {
            while (peek(&stack) != '(')
            {
                postfix_expression[i] = pop(&stack);
                i++;
                postfix_expression[i] = ' ';
                i++;
            }

            pop(&stack); // �ʿ������ '(' ����
            expression++;
        }
        else if (*expression == '+' || *expression == '-' || *expression == '*' || *expression == '/')
        {
            while(!isEmpty(&stack) && (checkOperatorPeriority(peek(&stack) >= checkOperatorPeriority(*expression))))
            {
                postfix_expression[i] = pop(&stack);
                i++;
                postfix_expression[i] = ' ';
                i++;
            }

            push(&stack, *expression);
            expression++;
        }
        else if (*expression >= '0' && *expression <= '9')
        {
            do{
                postfix_expression[i] = *expression;
                i++;
                expression++;
            } while (*expression >= '0' && *expression <= '9');

            postfix_expression[i] = ' ';
            i++;
        }
        else
        {
            expression++;
        }
    }

    while(!isEmpty(&stack))
    {
        postfix_expression[i] = pop(&stack);
        i++;
        postfix_expression[i] = ' ';
        i++;
    }

    // dst--;
    // *dst = 0;

    printf("postfix_expression: %s\n", postfix_expression);

}

int checkOperatorPeriority(int op)
{
    if (op == '(')
        return 0;
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    else
        return 3;
}

double calculateExpression(char *expression)
{
    double result;

    return result;
}

char *getExpression(void)
{
    char input_buffer[300];
    char *result;
    int i;

    fgets(input_buffer, sizeof(input_buffer), stdin);
    if (input_buffer == NULL)
    {
        return NULL; // ������� ctrl + z �Է�
    }

    input_buffer[strlen(input_buffer) - 1] = '\0'; // fgets()�� �޾ƿ� ���ڿ��� ������ \n ����
    eliminateCharFromString(input_buffer, ' ');    // �Է� ���Ŀ��� ���� ����

    result = input_buffer;

    return result;
}

void eliminateCharFromString(char *str, char ch)
{
    for (; *str != '\0'; str++) // ���� ���ڸ� ���� ������ �ݺ�
    {
        if (*str == ch) // ch�� ���� ������ ��
        {
            strcpy(str, str + 1);
            str--;
        }
    }
}

void initStack(t_Stack *stack, int size)
{
    stack->item_arr = (int *)malloc(sizeof(int) * size);
    if (stack->item_arr == NULL)
    {
        printf("main() return error code1: �޸� �����Ҵ� ����\n");
        exit(1);
    }

    stack->top = -1;
    stack->size = size;
}

bool isEmpty(t_Stack *stack)
{
    if (stack->top == -1)
        return true;
    else
        return false;
}

void push(t_Stack *stack, int item)
{
    if (stack->top < stack->size - 1)
    {
        stack->top++;
        stack->item_arr[stack->top] = item;
    }
    else
    {
        stack->size = stack->size * 2;
        stack->item_arr = (int *)realloc(stack->item_arr, sizeof(int) * stack->size);
        push(stack, item);
    }
}

int pop(t_Stack *stack)
{
    int result;

    if (stack->top >= 0)
    {
        result = stack->item_arr[stack->top];
        stack->top--;
        return result;
    }

    printf("������ �����\n");
    return 0;
}

int peek(t_Stack *stack)
{
    int result = stack->item_arr[stack->top];
    return result;
}

void freeStack(t_Stack *stack)
{
    free(stack);
}
