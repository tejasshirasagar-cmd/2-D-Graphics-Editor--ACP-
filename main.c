#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "canvas.h"

Canvas* create_canvas(int width, int height) {
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    Canvas *canvas = (Canvas *)malloc(sizeof(Canvas));
    if (!canvas) {
        return NULL;
    }

    canvas->width = width;
    canvas->height = height;

    canvas->pixels = (char **)malloc(height * sizeof(char *));
    if (!canvas->pixels) {
        free(canvas);
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        canvas->pixels[i] = (char *)malloc(width * sizeof(char));
        if (!canvas->pixels[i]) {
            // Cleanup previously allocated rows
            for (int j = 0; j < i; j++) {
                free(canvas->pixels[j]);
            }
            free(canvas->pixels);
            free(canvas);
            return NULL;
        }
        // Initialize to background character '_'
        memset(canvas->pixels[i], '_', width);
    }

    return canvas;
}

void free_canvas(Canvas *canvas) {
    if (!canvas) {
        return;
    }
    for (int i = 0; i < canvas->height; i++) {
        free(canvas->pixels[i]);
    }
    free(canvas->pixels);
    free(canvas);
}

void clear_canvas(Canvas *canvas) {
    if (!canvas) {
        return;
    }
    for (int i = 0; i < canvas->height; i++) {
        memset(canvas->pixels[i], '_', canvas->width);
    }
}

void display_canvas(const Canvas *canvas) {
    if (!canvas) {
        printf("Error: No canvas to display.\n");
        return;
    }

    // Print top ruler - Tens digit if width > 9
    printf("     ");
    for (int x = 0; x < canvas->width; x++) {
        if (x >= 10) {
            printf("%d ", x / 10);
        } else {
            printf("  ");
        }
    }
    printf("\n");

    // Print top ruler - Ones digit
    printf("     ");
    for (int x = 0; x < canvas->width; x++) {
        printf("%d ", x % 10);
    }
    printf("\n");

    // Print border line
    printf("   +-");
    for (int x = 0; x < canvas->width; x++) {
        printf("--");
    }
    printf("\n");

    // Print rows
    for (int y = 0; y < canvas->height; y++) {
        printf("%2d | ", y); // Row index
        for (int x = 0; x < canvas->width; x++) {
            printf("%c ", canvas->pixels[y][x]);
        }
        printf("|\n");
    }

    // Print bottom border
    printf("   +-");
    for (int x = 0; x < canvas->width; x++) {
        printf("--");
    }
    printf("\n");
}

int set_pixel(Canvas *canvas, int x, int y, char ch) {
    if (!canvas || x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) {
        return 0;
    }
    canvas->pixels[y][x] = ch;
    return 1;
}

char get_pixel(const Canvas *canvas, int x, int y) {
    if (!canvas || x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) {
        return '\0';
    }
    return canvas->pixels[y][x];
}

int resize_canvas(Canvas *canvas, int new_width, int new_height) {
    if (!canvas || new_width <= 0 || new_height <= 0) {
        return 0;
    }

    // Allocate new row array
    char **new_pixels = (char **)malloc(new_height * sizeof(char *));
    if (!new_pixels) {
        return 0;
    }

    for (int i = 0; i < new_height; i++) {
        new_pixels[i] = (char *)malloc(new_width * sizeof(char));
        if (!new_pixels[i]) {
            for (int j = 0; j < i; j++) {
                free(new_pixels[j]);
            }
            free(new_pixels);
            return 0;
        }
        // Default initialize new row to background
        memset(new_pixels[i], '_', new_width);
    }

    // Copy content from old canvas where overlapping
    int copy_height = (canvas->height < new_height) ? canvas->height : new_height;
    int copy_width = (canvas->width < new_width) ? canvas->width : new_width;

    for (int y = 0; y < copy_height; y++) {
        memcpy(new_pixels[y], canvas->pixels[y], copy_width);
    }

    // Free old pixels
    for (int i = 0; i < canvas->height; i++) {
        free(canvas->pixels[i]);
    }
    free(canvas->pixels);

    // Update canvas structure
    canvas->pixels = new_pixels;
    canvas->width = new_width;
    canvas->height = new_height;

    return 1;
}

int save_canvas(const Canvas *canvas, const char *filename) {
    if (!canvas || !filename) {
        return 0;
    }
    FILE *f = fopen(filename, "w");
    if (!f) {
        return 0;
    }
    fprintf(f, "%d %d\n", canvas->width, canvas->height);
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            fputc(canvas->pixels[y][x], f);
        }
        fputc('\n', f);
    }
    fclose(f);
    return 1;
}

Canvas* load_canvas(const char *filename) {
    if (!filename) {
        return NULL;
    }
    FILE *f = fopen(filename, "r");
    if (!f) {
        return NULL;
    }
    int w = 0, h = 0;
    if (fscanf(f, "%d %d", &w, &h) != 2 || w <= 0 || h <= 0) {
        fclose(f);
        return NULL;
    }
    
    // Consume characters up to the end of the line
    int ch;
    while ((ch = fgetc(f)) != '\n' && ch != EOF);

    Canvas *canvas = create_canvas(w, h);
    if (!canvas) {
        fclose(f);
        return NULL;
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            ch = fgetc(f);
            if (ch == EOF) {
                fclose(f);
                free_canvas(canvas);
                return NULL;
            }
            if (ch == '\n') {
                // If we hit a newline prematurely, it means the row was too short.
                // We back up and set the x-coordinate to fill the remainder with background,
                // or treat it as an error. For robust loading of hand-edited files, let's fill
                // the rest of the row with '_' and start the next row.
                for (int fill_x = x; fill_x < w; fill_x++) {
                    canvas->pixels[y][fill_x] = '_';
                }
                break;
            }
            canvas->pixels[y][x] = (char)ch;
        }
        // If we didn't hit '\n' yet, consume characters up to the end of the line
        if (ch != '\n') {
            while ((ch = fgetc(f)) != '\n' && ch != EOF);
        }
    }

    fclose(f);
    return canvas;
}
