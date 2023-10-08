/*
*** 자료구조 stack을 활용 하여 계산기 만들기 ****

아래의 수식을 계산하는 프로그램을 C언어 자료구조 stack을 활용 하여 프로그램을 작성 하시오
조건: 변수 는 double type으로 선언한다.

1. 아래의 수식을 fgets(inbuff, 100, stdin)으로 입력을 받아 들인 후
(( 222 + 4 ) * 55 ) - 100 / 7 * 5 - 5 * 10 =

2. 상기 명령을 파싱(구문분석)하여 후위 표기법(컴퓨터가 계산하기에 편한 수식의 형태 222 4 +)
 으로 stack에 저장 한 후 계산 하는 계산기 프로그램을 작성 한다.

참고로 상기 결과는 12418.571428571
*/

/*
내 프로그램의 경우, 입력 "(( 222 + 4 ) * 55 ) - 100 / 7 * 5 - 5 * 10 =" 에서
마지막 "="은 입력으로 들어오면 안됨.
"(( 222 + 4 ) * 55 ) - 100 / 7 * 5 - 5 * 10" 이런 형태로 들어와야 함.
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
            printf("StackCalculator를 종료합니다.\n");
            break;
        }
    }

    return 0;
}

bool runCalculator(void)
{
    char *expression;
    double result;

    printf("수식을 입력하세요(종료: ctrl +z): ");
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

    printf("*expression: %c\n", *expression); // 우선 맞게 들어옴

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

            pop(&stack); // 필요없어진 '(' 제거
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
        return NULL; // 사용자의 ctrl + z 입력
    }

    input_buffer[strlen(input_buffer) - 1] = '\0'; // fgets()로 받아온 문자열의 마지막 \n 제거
    eliminateCharFromString(input_buffer, ' ');    // 입력 수식에서 공백 제거

    result = input_buffer;

    return result;
}

void eliminateCharFromString(char *str, char ch)
{
    for (; *str != '\0'; str++) // 종료 문자를 만날 때까지 반복
    {
        if (*str == ch) // ch와 같은 문자일 때
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
        printf("main() return error code1: 메모리 동적할당 실패\n");
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

    printf("스택이 비었음\n");
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
