/*
 < ���ͳ� �˻��� ���� ������ ������ ���� �κ�...>

 (1) �����ڸ� ���ÿ� �ְ� ���� �������� "������ �켱����" ���̵� �����ؾ� �Ѵٴ� ��.
 (2) �ϼ��� ����ǥ��� ���ڿ��� �տ������� �о���µ� "n�� �ڸ��� ���ڿ�"�� �����ڸ� ������ ������ "���� ����ü"�� �о�� ���ؼ���
      <string.h>�� atoi() ���� ���� �ڵ带 ��¦ �����Ͽ� ���� �ڵ忡 �°� �����ؾ� �Ѵٴ� ��.

 < �Ѱ� >

 �� �ڵ�� ���� ����� �Ǽ�(float) ���·� ������� ��,
 >> ((222 + 4) * 55) - 100 / 7 * 5 - 5 * 10 = 12308.571429

 ���� �Է� �������� �Ǽ�(float) ������ ���� �Է��ϴ� ���� ������ ����.
 >> ((222.5 + 4.3)* 55.0 ) - 100.2/ 7.3 *5.0 - 5.9 *10.0 = [occur fatal error!!!]

*/

/*
 <�۵� ����>

 *  �۾� ���� ��: cmd /C C:\ken\temp\�ǰ���_postfix

 ������ �Է��ϼ���>> ((222 + 4)* 55 ) - 100/ 7 *5 - 5 *10

 �Էµ� ����ǥ��� ����: ((222+4)*55)-100/7*5-5*10
 ����ǥ��� ��ȯ ���: 222 4 + 55 * 100 7 / 5 * - 5 10 * -
 �Է¼��� ��� ���: 12308.571429

 *  �͹̳��� �۾����� �ٽ� ���˴ϴ�. �������� �ƹ� Ű�� ��������.
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

    printf("������ �Է��ϼ���>> ");
    fgets(infix_expression, sizeof(infix_expression), stdin);

    infix_expression[strlen(infix_expression) - 1] = '\0'; // fgets()�� �޾ƿ� ���ڿ��� ������ \n ����
    deleteCharFromString(infix_expression, ' ');           // �Էµ� ���Ŀ��� ���鹮�� ��� ����
    printf("\n�Էµ� ����ǥ��� ����: %s\n", infix_expression);

    convertInfixToPostfix(postfix_expression, infix_expression);
    printf("����ǥ��� ��ȯ ���: %s\n", postfix_expression);

    result = calculateExpression(postfix_expression);
    printf("�Է¼��� ��� ���: %f\n", result);

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
                // �����ߴ� ������ �ٽ� �ִ� ������
                // ������ ���� ������ ���� ����ǥ��� ��� ��
                // ������ �������� n�� �ڸ��� ���ڿ��� atoi�ϱ� ������ (���� ����)
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
    *postfix = '\0'; // postfix_expression ���ڿ� ������ �������� ���ڿ� ���� �˸��� �ι��� ����
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
            // <string.h>�� atoi() ���� ���� �ڵ带 ���� �����Ͽ� ���α׷��� �°� ����
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
        else // ������ ��� ���� ������.. ������ �ֱ⿡ ��μ� ���� atoiȰ�� �ڵ尡 �ùٸ� ��Ģ���� �����ϰ� ��..
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
        printf("������ ������.\n");
        return -1;
    }

    stack[++top] = item;
    return item;
}

double pop(void)
{
    if (top < 0)
    {
        printf("������ �����.\n");
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
