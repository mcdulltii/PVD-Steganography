#include <stdio.h>
#include <stdlib.h>
#include <embed.h>
#include <fractal.h>


/* Global Variables */

char *input;
bitmap_t output;
FILE *fptr, *fp;

int dimensions = 800;
int capacity = 0;


bitmap_t initialize() {
    /* Initialisation */
    input = (char*)calloc(FILE_SIZE, sizeof(char*));

    // Create an image
    output.width = dimensions;
    output.height = dimensions;

    return output;
}


int generate_fractal_img() {
    /* Initialisation */
    output = initialize();

restart:
    output.pixels = calloc (output.width * output.height, sizeof (pixel_t));
	if (! output.pixels) {
		return -1;
    }


    /* Fractal Algorithm */

    // Set random seed
    srand(time(NULL));

    // Generate fractal bitmap
    output = fractal(output, (int)rand());


    /* Anti-aliasing Algorithm */

    // Blur bitmap twice
    for (int i=0; i < 2; i++) output = antialias(output);


    /* Verify Fractal */

    if (!verify(output)) {
        free (output.pixels);
        // Restart fractal generation if entire bitmap is black
        goto restart;
    }


    /* Save embedded image */

    if (save_png_to_file (& output, "output.png")) {
        fprintf (stderr, "Error writing file.\n");
        return -1;
    }

    return 0;
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
    int choice, rref, gref, bref, pixelcnt = 0, inputlen = strlen(input);
    int *coord = (int *)calloc(inputlen * 2, sizeof(int *));
    int divdimension = floor(((float)dimensions / 3));
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
            handle(coord[pixelcnt], coord[pixelcnt+1], output, rref, gref, bref, fptr, input);
            pixelcnt += 2;
        }
    }
}


int main(int argc, char *argv[]) {
    /* Check for cmdline argument */

    if (argc != 2) {
        puts("Invalid arguments!");
        return -1;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error reading '%s' file.\n", argv[1]);
        return -1;
    }


    /* Generate placeholder image */

    if (generate_fractal_img() == -1) {
        puts("Error generating image!");
        exit(0);
    }


    /* Steganography initialization */

    // Read input string
    read_input();

    // Convert first char to binary string
    char binval = input[0];
    if (!binval) {
        puts("Error!");
        exit(0);
    }

    // Set bits global variable in embed.h
    bits = chartobin(binval);

    // More Initialisation
    fptr = fopen("embed.log","w");
    int lix = floor((float)output.height/3);
    int liy = floor((float)output.width/3);

    // Print total Embedding capacity
    printf("Total Embd. Capacity: %d\n", calcCapacity(capacity, lix, liy, output));


    /* Steganography embedding */

    // Will exit in handlebits() when fully embedded
    embed_string();

    // Data size if greater than embedding capacity
    fclose(fptr);
    free (output.pixels);
    free (input);
    puts("Exiting... Data size greater than embedding capacity!!");
    return -1;
}
