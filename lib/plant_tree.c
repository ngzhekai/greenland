#include "plant_tree.h"

// Client
void init_plant_tree(int *pipe)
{
    char buffer[1024];

    printf("################################\n");
    printf("#                              #\n");
    printf("#         PLANT TREE           #\n");
    printf("#                              #\n");
    printf("################################\n\n");
    printf("\tEnter tree name/species: ");
    bzero(buffer, 1024);
    scanf("%s", buffer);
    write(*pipe, buffer, 1024);

    printf("\tEnter tree planted date (YYYY-MM-DD) [Exp: 2022-12-31]: ");
    bzero(buffer, 1024);
    scanf("%s", buffer);
    write(*pipe, buffer, 1024);

    printf("\tEnter tree location (X Y) [Exp: 40 100]: ");
    int x, y;
    scanf("%d", &x);
    scanf("%d", &y);
    bzero(buffer, 1024);
    sprintf(buffer, "%d", x);
    write(*pipe, buffer, 1024);
    bzero(buffer, 1024);
    sprintf(buffer, "%d", y);
    write(*pipe, buffer, 1024);
}

// Server