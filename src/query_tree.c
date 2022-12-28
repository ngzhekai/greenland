#include <stdio.h>
#include <fcntl.h>
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

// Function Declaration
int query_tree(char *filename, struct details tree, struct details trees[], int *treeIndex);
void display_tree(char *filename, struct details trees[], int treeIndex);

// get_coordinates
struct details get_coordinates(struct details tree)
{
    printf("Enter the X Coordinates of the tree: \n");
    scanf("%d", &tree.vertical);
    printf("Enter the Y Coordinates of the tree: \n");
    scanf("%d", &tree.horizontal);
    return tree;
}

int main(int argc, char const *argv[])
{
    char *filename = "test.txt";
    int find_result = 0;
    int treeIndex = -1;
    char response[BUFFER_SIZE];

    do
    {
        tree = get_coordinates(tree);
        find_result = query_tree(filename, tree, trees, &treeIndex);

        if (!find_result)
        {
            printf("\nNo tree with the given coordinates was found in the database.\n");
            printf("Enter '1' to try again or 'no' to exit: \n\n");
            scanf("%s", &response);
            // printf("%s", response);
            if (strcmp(response, "no") == 0)
                break;
        }
    } while (!find_result);

    if (find_result)
    {
        display_tree(filename, trees, treeIndex);
    }

    printf("The program ended\n");
    return 0;
}

int query_tree(char *filename, struct details tree, struct details trees[], int *treeIndex)
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
            printf("The coordinates found was located on line %d\n", *treeIndex + 1);
            return 1;
        }
        numTrees++;
    }
    fclose(fp);

    return 0;
}

void display_tree(char *filename, struct details trees[], int treeIndex)
{
    // int numTrees = 0;
    char temp[BUFFER_SIZE];
    printf("%d %d %s %s %s\n", trees[treeIndex].vertical, trees[treeIndex].horizontal, trees[treeIndex].species, trees[treeIndex].date, trees[treeIndex].status);
}