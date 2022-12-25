#include <stdio.h>
#include <search.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define BUFFER_SIZE 512
// #define PERMISSION 0755 /* OWNER: WRX | GROUP: RX  | Other: RX */

struct details
{
    int vertical;   /* x location*/
    int horizontal; /* y location*/
    char species[BUFFER_SIZE];
    char date[BUFFER_SIZE];
    char status[BUFFER_SIZE];
} tree;

int main(int argc, char const *argv[])
{

    char *filename = "database.txt";
    int fd;
    int speciesChosen, state;

    // For SEARCH Coordinates
    FILE *fp;
    int line_num, find_result;
    char temp[BUFFER_SIZE];
    char search[BUFFER_SIZE];

    do
    {
        printf("Enter the X Coordinates of the tree: \n");
        scanf("%d", &tree.vertical);
        printf("Enter the Y Coordinates of the tree: \n");
        scanf("%d", &tree.horizontal);

        // Concatenate into string format x,y used for search
        sprintf(search, "%d,%d", tree.vertical, tree.horizontal);

        // INIT
        line_num = 1;
        find_result = 0;

        // Open file for reading
        if ((fp = fopen(filename, "r")) == NULL)
        {
            printf("Error opening the file %s for reading", filename);
            exit(0);
        }

        // read characters from the fp until the first new-line character (/n) and stores in string temp
        while (fgets(temp, 512, fp) != NULL)
        {
            if ((strstr(temp, search)) != NULL) // check if string search is in string temp
            {
                printf("A match found on line: %d\n\n", line_num);
                printf("\n%s\n", temp);
                find_result = 1;
                break;
            }
            line_num++;
        }

        if (find_result)
        {
            printf("\nThe following location already exists!\nTry Again!\n\n\n");
        }
        fclose(fp);
    } while (find_result != 0);

    do
    {
        printf("Enter the species of the tree (1) Deciduous (2) Coniferous: \n");
        scanf("%d", &speciesChosen);
    } while (speciesChosen > 2 || speciesChosen < 1);

    if (speciesChosen == 1)
        strncpy(tree.species, "Deciduous", 10);
    else
        strncpy(tree.species, "Coniferous", 11);

    printf("Enter the date planted: \n");
    scanf("%s", &tree.date);

    do
    {
        printf("Enter the status of the tree (1) Alive (2) Dead: \n");
        scanf("%d", &state);
    } while (state > 2 || state < 1);

    if (state == 1)
        strncpy(tree.status, "Alive", 6);
    else
        strncpy(tree.status, "Dead", 5);

    /* open file for writing */
    FILE *of;
    of = fopen(filename, "a");
    if (of == NULL)
    {
        printf("Error opening the file %s", filename);
        exit(0);
    }

    fprintf(of, "%d,%d\t\t\t%s\t\t%s\t%s\n", tree.vertical, tree.horizontal, tree.species, tree.date, tree.status);

    // fprintf generally use for the text file and fwrite generally use for a binary file.

    fclose(of);
    return 0;
}