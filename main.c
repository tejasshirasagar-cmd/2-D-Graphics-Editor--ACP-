#include <stdio.h>

#define MAX 50

char canvas[MAX][MAX];

/* Function Prototypes */
void initializeCanvas(int rows, int cols);
void displayCanvas(int rows, int cols);
void drawPixel(int rows, int cols);
void clearCanvas(int rows, int cols);

int main()
{
    int rows, cols, choice;

    printf("Enter canvas rows: ");
    scanf("%d", &rows);

    printf("Enter canvas columns: ");
    scanf("%d", &cols);

    if(rows <= 0 || cols <= 0 || rows > MAX || cols > MAX)
    {
        printf("Invalid canvas size\n");
        return 0;
    }

    initializeCanvas(rows, cols);

    do
    {
        printf("\n----- 2D Graphics Editor -----\n");
        printf("1. Display Canvas\n");
        printf("2. Draw Pixel\n");
        printf("3. Clear Canvas\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                displayCanvas(rows, cols);
                break;

            case 2:
                drawPixel(rows, cols);
                break;

            case 3:
                clearCanvas(rows, cols);
                printf("Canvas Cleared\n");
                break;

            case 4:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid Choice\n");
        }

    } while(choice != 4);

    return 0;
}

/* Initialize Canvas */
void initializeCanvas(int rows, int cols)
{
    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            canvas[i][j] = '_';
        }
    }
}

/* Display Canvas */
void displayCanvas(int rows, int cols)
{
    printf("\nCanvas:\n");

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            printf("%c ", canvas[i][j]);
        }
        printf("\n");
    }
}

/* Draw Pixel */
void drawPixel(int rows, int cols)
{
    int x, y;

    printf("Enter row index: ");
    scanf("%d", &x);

    printf("Enter column index: ");
    scanf("%d", &y);

    if(x >= 0 && x < rows && y >= 0 && y < cols)
    {
        canvas[x][y] = '*';
    }
    else
    {
        printf("Invalid Position\n");
    }
}

/* Clear Canvas */
void clearCanvas(int rows, int cols)
{
    initializeCanvas(rows, cols);
}
