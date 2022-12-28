/* update_tree.c (refactored) into several sub functions */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_TREES 100
#define BUFFER_SIZE 512

struct details
{
    int vertical;   /* x location*/
    int horizontal; /* y location*/
    char species[BUFFER_SIZE];
    char date[BUFFER_SIZE];
    char status[BUFFER_SIZE];
} tree, trees[MAX_TREES];

/* function declaration */
struct details get_coordinates(struct details tree);
int check_tree_exist(char *filename, struct details tree, struct details trees[], int *treeIndex);
void copy_tree(char *filename, struct details *trees, struct details tree, int *treeIndex);
void update_tree(char *filename, struct details trees[], int *treeIndex);
void write_tree(char *filename, struct details trees[], int treeIndex);

int main(int argc, char const *argv[])
{
    char *filename = "test.txt";
    int find_result = 0;
    int treeIndex = -1;

    do
    {
        tree = get_coordinates(tree);
        // printf("x coordinates debug mode : %d\n\n", tree.vertical); // for debug
        find_result = check_tree_exist(filename, tree, trees, &treeIndex);

        // Prompt the user (no tree located in the specified location)
        if (!find_result)
            printf("\nNo tree with the given coordinates was found in the database.\nTry Again!\n\n\n");

    } while (!find_result);

    copy_tree(filename, trees, tree, &treeIndex);
    update_tree(filename, trees, &treeIndex);

    return 0;
}

struct details get_coordinates(struct details tree)
{
    printf("Enter the X Coordinates of the tree: \n");
    scanf("%d", &tree.vertical);
    printf("Enter the Y Coordinates of the tree: \n");
    scanf("%d", &tree.horizontal);
    return tree;
}

int check_tree_exist(char *filename, struct details tree, struct details trees[], int *treeIndex)
{
    FILE *fp;

    // variable INIT
    int numTrees = 0;
    char temp[BUFFER_SIZE];

    // Open file for reading
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Error opening the file %s for reading", filename);
        exit(0);
    }

    // Read tree data from the file and store in the trees array
    while (fgets(temp, 512, fp) != NULL)
    {
        sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &trees[numTrees].vertical, &trees[numTrees].horizontal, &trees[numTrees].species, &trees[numTrees].date, &trees[numTrees].status);
        if (trees[numTrees].vertical == tree.vertical && trees[numTrees].horizontal == tree.horizontal)
        {
            *treeIndex = numTrees;
            // treeIndex can be used to show which line is the search tree located in the textfile;
            printf("The coordinates found was located on line %d\n", *treeIndex + 1);
            return 1;
        }
        numTrees++;
    }
    fclose(fp);

    return 0;
}

void copy_tree(char *filename, struct details *trees, struct details tree, int *treeIndex)
{
    FILE *fp;

    // Open file for reading
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("Error opening the file %s for reading", filename);
        exit(0);
    }

    // variable INIT
    int numTrees = 0;
    char temp[BUFFER_SIZE];

    // Open file for copying into temporary text file
    FILE *tmp;
    tmp = fopen("temp.txt", "a");
    if (tmp == NULL)
    {
        printf("Error opening the file %s for temporary writing", filename);
        exit(0);
    }

    // Read tree data from the file and store in the trees array
    while (fgets(temp, 512, fp) != NULL)
    {
        sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &trees[numTrees].vertical, &trees[numTrees].horizontal, &trees[numTrees].species, &trees[numTrees].date, &trees[numTrees].status);
        if (!(trees[numTrees].vertical == tree.vertical && trees[numTrees].horizontal == tree.horizontal))
        {
            fprintf(tmp, "%d,%d\t\t%s\t\t%s\t\t%s\n", trees[numTrees].vertical, trees[numTrees].horizontal, trees[numTrees].species, trees[numTrees].date, trees[numTrees].status);
        }
        numTrees++;
    }

    fclose(tmp);
    fclose(fp);
}

void update_tree(char *filename, struct details trees[], int *treeIndex)
{
    // variable INIT
    int speciesChosen = 0;
    int state = 0;

    // check user input
    do
    {
        printf("Enter the species of the tree (1) Deciduous (2) Coniferous: \n");
        scanf("%d", &speciesChosen);
    } while (speciesChosen > 2 || speciesChosen < 1);

    if (speciesChosen == 1)
        strncpy(trees[*treeIndex].species, "Deciduous", 10);
    else
        strncpy(trees[*treeIndex].species, "Coniferous", 11);

    printf("Enter the date planted: \n");
    scanf("%s", trees[*treeIndex].date);

    // check user input
    do
    {
        printf("Enter the status of the tree (1) Alive (2) Dead: \n");
        scanf("%d", &state);
    } while (state > 2 || state < 1);

    if (state == 1)
        strncpy(trees[*treeIndex].status, "Alive", 6);
    else
        strncpy(trees[*treeIndex].status, "Dead", 5);

    write_tree(filename, trees, *treeIndex);
}

void write_tree(char *filename, struct details trees[], int treeIndex)
{
    /* open file for writing */
    FILE *of;
    of = fopen("temp.txt", "a");
    if (of == NULL)
    {
        printf("Error opening the file %s", filename);
        exit(0);
    }

    // append the array data for [treeIndex] into the text file
    fprintf(of, "%d,%d\t\t%s\t\t%s\t\t%s\n", trees[treeIndex].vertical, trees[treeIndex].horizontal, trees[treeIndex].species, trees[treeIndex].date, trees[treeIndex].status);

    fclose(of);

    // rename temp.txt to test.txt
    if (rename("temp.txt", filename) == 0)
    {
        printf("Database %s is updated successfully\n", filename);
        unlink("temp.txt");
    }
    else
        perror("Error: has occurred\n");
    exit(1);
}
