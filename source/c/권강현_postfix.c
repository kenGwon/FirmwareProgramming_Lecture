/*
 < 인터넷 검색을 통해 결정적 도움을 받은 부분...>

 (1) 연산자를 스택에 넣고 빼는 과정에서 "연산자 우선순위" 아이디어를 적용해야 한다는 것.
 (2) 완성된 후위표기법 문자열을 앞에서부터 읽어나가는데 "n의 자릿수 문자열"을 연산자를 만나기 전까지 "숫자 그자체"로 읽어내기 위해서는
      <string.h>의 atoi() 내부 구현 코드를 살짝 가공하여 본인 코드에 맞게 적용해야 한다는 것.

 < 한계 >

 본 코드는 연산 결과를 실수(float) 형태로 출력해줄 뿐,
 >> ((222 + 4) * 55) - 100 / 7 * 5 - 5 * 10 = 12308.571429

 수식 입력 과정에서 실수(float) 형태의 수를 입력하는 것은 허용되지 않음.
 >> ((222.5 + 4.3)* 55.0 ) - 100.2/ 7.3 *5.0 - 5.9 *10.0 = [occur fatal error!!!]

*/

/*
 <작동 예시>

 *  작업 실행 중: cmd /C C:\ken\temp\권강현_postfix

 수식을 입력하세요>> ((222 + 4)* 55 ) - 100/ 7 *5 - 5 *10

 입력된 중위표기법 수식: ((222+4)*55)-100/7*5-5*10
 후위표기법 변환 결과: 222 4 + 55 * 100 7 / 5 * - 5 10 * -
 입력수식 계산 결과: 12308.571429

 *  터미널이 작업에서 다시 사용됩니다. 닫으려면 아무 키나 누르세요.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 100
#define EXPRESSION_SIZE 200

double stack[STACK_SIZE];
int top;

void initStack(void);
double push(double item);
double pop(void);
double peek(void);
bool isEmpty(void);

void deleteCharFromString(char *str, char ch);
void convertInfixToPostfix(char *postfix, char *infix);
int checkPriority(int operator);
double calculateExpression(char *postfix);

int main(void)
{
    double result;
    char infix_expression[EXPRESSION_SIZE];
    char postfix_expression[EXPRESSION_SIZE];

    printf("수식을 입력하세요>> ");
    fgets(infix_expression, sizeof(infix_expression), stdin);

    infix_expression[strlen(infix_expression) - 1] = '\0'; // fgets()로 받아온 문자열의 마지막 \n 제거
    deleteCharFromString(infix_expression, ' ');           // 입력된 수식에서 공백문자 모두 제거
    printf("\n입력된 중위표기법 수식: %s\n", infix_expression);

    convertInfixToPostfix(postfix_expression, infix_expression);
    printf("후위표기법 변환 결과: %s\n", postfix_expression);

    result = calculateExpression(postfix_expression);
    printf("입력수식 계산 결과: %f\n", result);

    return 0;
}

void deleteCharFromString(char *str, char ch)
{
    while (*str != '\0')
    {
        if (*str == ch)
        {
            strcpy(str, str + 1);
            str--;
        }

        str++;
    }
}

void convertInfixToPostfix(char *postfix, char *infix)
{
    initStack();

    while (*infix != '\0')
    {
        if (*infix == '(')
        {
            push((double)*infix);
            infix++;
        }
        else if (*infix == ')')
        {
            while ((int)peek() != '(')
            {
                *postfix++ = (int)pop();
                *postfix++ = ' ';
                // 삭제했던 공백을 다시 넣는 이유는
                // 정제된 공백 삽입을 통해 후위표기법 계산 시
                // 공백을 기준으로 n의 자릿수 숫자열을 atoi하기 위함임 (이하 동일)
            }
            pop();
            infix++;
        }
        else if (*infix == '+' || *infix == '-' || *infix == '*' || *infix == '/')
        {
            while (!isEmpty() && (checkPriority((int)peek()) >= checkPriority(*infix)))
            {
                *postfix++ = (int)pop();
                *postfix++ = ' ';
            }
            push((double)*infix);
            infix++;
        }
        else if (*infix >= '0' && *infix <= '9')
        {
            do
            {
                *postfix++ = *infix++;
            } while (*infix >= '0' && *infix <= '9');
            *postfix++ = ' ';
        }
        else
        {
        }
    }

    while (!isEmpty())
    {
        *postfix++ = (int)pop();
        *postfix++ = ' ';
    }
    postfix--;
    *postfix = '\0'; // postfix_expression 문자열 제작이 끝났으니 문자열 끝을 알리는 널문자 삽입
}

int checkPriority(int operator)
{
    if (operator== '(')
        return 0;
    else if (operator== '+' || operator== '-')
        return 1;
    else if (operator== '*' || operator== '/')
        return 2;
    else
        return 3;
}

double calculateExpression(char *postfix)
{
    double atoi_num;
    int digit_num;
    initStack();

    while (*postfix != '\0')
    {
        if (*postfix >= '0' && *postfix <= '9')
        {
            // <string.h>의 atoi() 내부 구현 코드를 조금 수정하여 프로그램에 맞게 구현
            atoi_num = 0;
            do
            {
                digit_num = *postfix - '0';
                atoi_num = atoi_num * 10 + digit_num;
                postfix++;
            } while (*postfix >= '0' && *postfix <= '9');

            push((double)atoi_num);
        }
        else if (*postfix == '+')
        {
            push(pop() + pop());
            postfix++;
        }
        else if (*postfix == '-')
        {
            atoi_num = pop();
            push(pop() - atoi_num);
            postfix++;
        }
        else if (*postfix == '*')
        {
            push(pop() * pop());
            postfix++;
        }
        else if (*postfix == '/')
        {
            atoi_num = pop();
            push(pop() / atoi_num);
            postfix++;
        }
        else // 공백인 경우 다음 루프로.. 공백이 있기에 비로소 위에 atoi활용 코드가 올바른 규칙으로 동작하게 됨..
        {
            postfix++;
        }
    }
    return pop();
}

void initStack(void)
{
    top = -1;
}

double push(double item)
{
    if (top >= STACK_SIZE - 1)
    {
        printf("스택이 가득참.\n");
        return -1;
    }

    stack[++top] = item;
    return item;
}

double pop(void)
{
    if (top < 0)
    {
        printf("스택이 비었음.\n");
        return -1;
    }
    return stack[top--];
}

double peek(void)
{
    if (top < 0)
        return -1;
    else
        return stack[top];
}

bool isEmpty(void)
{
    if (top < 0)
        return true;
    else
        return false;
}
