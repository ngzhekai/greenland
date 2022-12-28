/* plant_tree.c (code refactored) */
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
void update_tree(char *filename, struct details trees[], int *treeIndex);
void write_tree(char *filename, struct details trees[], int treeIndex);

int main(int argc, char const *argv[])
{

    // variable INIT
    char *filename = "test.txt";
    int find_result = 1; // initialize to true
    int treeIndex = -1;

    do
    {
        tree = get_coordinates(tree);
        find_result = check_tree_exist(filename, tree, trees, &treeIndex);
        if (find_result)
            printf("\nA tree was found in the database at the given location.\nTry a new coordinate!\n\n\n");

        // ideas: maybe can create something like if a user entered a coordinates where tree already exist, can give them the option to update it.
    } while (find_result);

    update_tree(filename, trees, &treeIndex);

    return 0;
}

struct details get_coordinates(struct details tree)
{
    printf("Enter the X Coordinates of the tree: ");
    scanf("%d", &tree.vertical);
    printf("Enter the Y Coordinates of the tree: ");
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
        sscanf(temp, "%d,%d\t\t%s\t\t%s\t\t%s\n", &trees[numTrees].vertical, &trees[numTrees].horizontal, trees[numTrees].species, trees[numTrees].date, trees[numTrees].status);
        if (trees[numTrees].vertical == tree.vertical && trees[numTrees].horizontal == tree.horizontal)
        {
            *treeIndex = numTrees;
            // treeIndex can be used to show which line is the search tree located in the textfile;
            // printf("The coordinates found was located on line %d\n", *treeIndex + 1); // for debug
            return 1;
        }
        numTrees++;
    }

    // initialize trees[*treeIndex].xxxxx  to be used when writing
    trees[*treeIndex].vertical = tree.vertical;
    trees[*treeIndex].horizontal = tree.horizontal;
    fclose(fp);

    return 0;
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
    of = fopen(filename, "a");

    if (of == NULL)
    {
        printf("Error opening the file %s", filename);
        exit(0);
    }

    // append the array data for trees[treeIndex] into the text file
    fprintf(of, "%d,%d\t\t%s\t\t%s\t\t%s\n", trees[treeIndex].vertical, trees[treeIndex].horizontal, trees[treeIndex].species, trees[treeIndex].date, trees[treeIndex].status);

    fclose(of);
}