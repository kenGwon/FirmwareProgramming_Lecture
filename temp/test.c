#include <stdio.h>

int main(void)
{
    double list[5] = {1,2,3,4,5};

    for (int i = 0; i < 5; i++)
    {
        printf("%f ", list[i]);
    }

    return 0;
}