#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i;
    int *arr1, *arr2, *temp;

    /* malloc() */
    arr1 = (int *)malloc(3 * sizeof(int));

    if (arr1 == NULL)
    {
        printf("malloc() allocation failed.\n");
        return 1;
    }

    for (i = 0; i < 3; i++)
        arr1[i] = (i + 1) * 10;

    printf("Memory allocated using malloc():\n");
    for (i = 0; i < 3; i++)
        printf("%d ", arr1[i]);

    printf("\n\n");

    /* calloc() */
    arr2 = (int *)calloc(3, sizeof(int));

    if (arr2 == NULL)
    {
        printf("calloc() allocation failed.\n");
        free(arr1);
        return 1;
    }

    printf("Memory allocated using calloc():\n");
    for (i = 0; i < 3; i++)
        printf("%d ", arr2[i]);

    printf("\n\n");

    /* realloc() */
    temp = (int *)realloc(arr1, 6 * sizeof(int));

    if (temp == NULL)
    {
        printf("realloc() allocation failed.\n");
        free(arr1);
        free(arr2);
        return 1;
    }

    arr1 = temp;

    for (i = 3; i < 6; i++)
        arr1[i] = (i + 1) * 10;

    printf("After realloc(), array contains:\n");
    for (i = 0; i < 6; i++)
        printf("%d ", arr1[i]);

    printf("\n\n");

    /* free() */
    free(arr1);
    free(arr2);

    printf("Memory released successfully using free().\n");

    return 0;
}
