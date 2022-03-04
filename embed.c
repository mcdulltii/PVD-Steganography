#include <stdio.h>
#include <stdlib.h>
#include <pix.h>
#include <embed.h>


/* Global Variables */

char *input;
bitmap_t output;
FILE *fptr, *fp;
int width = 0, height = 0;


bitmap_t initialize(char *filename) {
    /* Initialisation */
    input = (char*)calloc(FILE_SIZE, sizeof(char*));

    // Read image
    png_bytep *row_pointers = read_png_file(filename, &width, &height);

    output.width = width;
    output.height = height;
    
    output.pixels = calloc (output.width * output.height, sizeof (pixel_t));
    if (! output.pixels) {
        puts("Error allocating bitmap");
        exit(1);
    }

    for (int y=0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for (int x=0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            pixel_t * pixel = pixel_at (& output, x, y);
            pixel->red = px[0];
            pixel->green = px[1];
            pixel->blue = px[2];
        }
    }

    return output;
}

void read_input() {
    char ch;
    int cnt = 0;
    while ((ch = fgetc(fp)) != EOF) {
        input[cnt] = ch;
        cnt++;
    }
    fclose(fp);
}


void embed_string() {
    // Initialisation
    int handle_ret, choice, rref, gref, bref, pixelcnt = 0, inputlen = strlen(input);
    int *coord = (int *)calloc(inputlen * 2, sizeof(int *));
    int divdimension = floor(((float)height / 3));
    int buffer[divdimension];

    // Create indexed array
#pragma omp parallel for
    for (int i=0; i < divdimension; i++) buffer[i] = i;

    // Mix indexed array
#pragma omp parallel for
    for (int i=0; i < inputlen * 2; i++) {
regen:
       choice = rand() % divdimension;
       if (buffer[choice] == choice) {
           coord[i] = choice;
           if (i%2 != 0) buffer[choice] = -1;
       } else {
           goto regen;
       }
    }

    // Divide pixels to [3 x 3] matrix
#pragma omp parallel
    while (pixelcnt <= inputlen) {
        // Obtain pixel values of ref. pixel
        pixel_t * pixelref = pixel_at (& output, coord[pixelcnt]*3+1, coord[pixelcnt+1]*3+1);
        rref = pixelref->red;
        gref = pixelref->green;
        bref = pixelref->blue;
        // printf("%d %d %d\n", rref, gref, bref);

        // Embed bits with input string
#pragma omp critical
        {
            handle_ret = handle(coord[pixelcnt], coord[pixelcnt+1], output, rref, gref, bref, fptr, input);
            if (handle_ret == 0)
                return;
            pixelcnt += 2;
        }
    }
}


int main(int argc, char *argv[]) {
    /* Check for cmdline argument */

    if (argc != 3) {
        puts("Invalid arguments!");
        return -1;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error reading '%s' file.\n", argv[1]);
        return -1;
    }


    /* Initialization */

    // Read cover image
    output = initialize(argv[2]);


    /* Steganography initialization */

    // Read input string
    read_input();

    // Convert first char to binary string
    char binval = input[0];
    if (!binval) {
        puts("Error writing char!");
        exit(1);
    }

    // Set bits global variable in embed.h
    bits = chartobin(binval);

    // More Initialisation
    fptr = fopen("embed.log","w");
    int lix = floor((float)output.height/3);
    int liy = floor((float)output.width/3);

    // Print total Embedding capacity
    printf("Total Embd. Capacity: %d\n", calcCapacity(lix, liy, output));


    /* Steganography embedding */

    // Embed string into output image
    embed_string();

    // Free pointers
    fclose(fptr);
    free (output.pixels);
    free (input);
    return 0;
}
