#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "pix.h"

/* Global Variables */

#define FILE_SIZE 4096

int completed = 0;
int charNum = 0;
int count = 0;
char *paddbits = "0000000";
char *bits = "";

// Obtain substring from string wrt index

char *substr(char const *inp, size_t start, size_t len) { 
    char *ret = calloc(8, len+1);
    memcpy(ret, inp+start, len);
    ret[len]  = '\0';
    return ret;
}

// Converts char to a binary string

char* chartobin ( char c ) {
    char *temp = (char*)calloc(8, sizeof(char*));
    int index = 0;
    for (int i = 7; i >= 0; --i) {
        temp[7-i] = ( (c & (1 << i)) ? '1' : '0');
    }
    temp[8] = '\0';
    for (int i = 0; i < strlen(temp); i++) {
        if (temp[i] != '0') {
            index = i;
            break;
        } else if (i == strlen(temp)-1) {
            index = strlen(temp) - 1;
        }
    }
    return substr(temp, index, strlen(temp) - index);
}

// Converts binary string to int

int bintochar ( char* temp ) {
    return (int)strtol(temp, (char**)NULL, 2);
}

// Classify pixels based on the difference in pixel value to the number of bits to be substituted to LSB

int classify(int pvd) {
    int nbits = 0;
    if (pvd < 16) {
        nbits = 2;
    } else if (pvd > 16 && pvd < 32) {
        nbits = 3;
    } else {
        nbits = 4;
    }
    return nbits;
}

// Calculate embedding capacity of the given cover image

int calcCapacity(int lix, int liy, bitmap_t output) {
    int r, g, b, rref, gref, bref, rdif, gdif, bdif, capacity = 0;

    // Divide pixels to [3 x 3] matrix
#pragma omp parallel for
    for (int i=0; i < lix * 3; i+=3) {
        for (int j=0; j < liy * 3; j+=3) {
            // Obtain pixel values of ref. pixel
            pixel_t * pixelref = pixel_at (& output, i+1, j+1);
            rref = pixelref->red;
            gref = pixelref->green;
            bref = pixelref->blue;

            // For all pixels in the matrix
            for (int k=i; k<i+3; k++) {
                if (k >= output.height) break;
                for (int l=j; l<j+3; l++) {
                    if (k == i+1 && l == j+1) continue;
                    if (l >= output.width) break;

                    // Calculate the difference in pixel values
                    pixel_t * pixel = pixel_at (& output, k, l);
                    r = pixel->red;
                    g = pixel->green;
                    b = pixel->blue;
                    rdif = abs(r - rref);
                    gdif = abs(g - gref);
                    bdif = abs(b - bref);

                    // Cumulative capacity
                    capacity += classify(rdif) + classify(gdif) + classify(bdif);
                }
            }
        }
    }

    // Return capacity
    return capacity;
}

// Function to embed data to pixel

int embedbits(int i, int j, char pixel, int diff, int colorpixel, FILE *lg, char* input) {
    // Initialise
    int pad = 0;
    int nb = diff;
    char *sequence = (char*)calloc(8, sizeof(char));
    // printf("%d %ld\n", nb, strlen(bits));

    switch (pixel) {
        case 'r':
            sequence[0] = '0';
            sequence[1] = '0';
            break;
        case 'g':
            sequence[0] = '0';
            sequence[1] = '1';
            break;
        case 'b':
            sequence[0] = '1';
            sequence[1] = '0';
            break;
        default:
            puts("Invalid pixel argument!");
            exit(1);
    }

    switch (diff) {
        case 2:
            sequence[2] = '0';
            sequence[3] = '0';
            break;
        case 3:
            sequence[2] = '0';
            sequence[3] = '1';
            break;
        case 4:
            sequence[2] = '1';
            sequence[3] = '0';
            break;
        default:
            puts("Invalid pixel argument!");
            exit(1);
    }

    // If the number of bits required is less than the number of bits in the data(char.) to be Embedded
    if (nb < strlen(bits)) {
        // Initialise
        char *newbits = substr(bits,0,nb);
        bits = substr(bits,nb,strlen(bits)-nb);
        int val = colorpixel;
        char *data = newbits;
        char *bival = chartobin(val);
        char newbival[8];
        if (strlen(bival)>strlen(data)) {
            char *tmp = substr(bival,0,(strlen(bival) - strlen(data)));
            strcat(tmp, data);
            strcpy(newbival, tmp);
        } else {
            strcpy(newbival, data);
        }
        sequence[4] = sequence[5] = '0';
        sequence[6] = (charNum%2) ? '1' : '0';
        // printf("less %s %s %s %s %s\n", bits, newbits, data, bival, newbival);

        // Write data to log File for extraction
        fprintf(lg, "%d %d %d \n", i, j, bintochar(sequence));

        // Return new pixel value after embedding
        int c = bintochar(newbival);
        free(sequence);
        return (c<0) ? 256 - c : c;
    } else {
    // If the number of bits required is greater than the number of bits in the data(char.) to be Embedded
        // Apply padding
        char newbits[8];
        strcpy(newbits, bits);
        strcat(newbits, substr(paddbits,0,(nb - strlen(bits))));
        int pad = nb - strlen(bits);
        int val = colorpixel;
        char *data = newbits;
        char *bival = chartobin(val);
        char newbival[8];
        if (strlen(bival)>strlen(data)) {
            char *tmp = substr(bival,0,(strlen(bival) - strlen(data)));
            strcat(tmp, data);
            strcpy(newbival, tmp);
        } else {
            strcpy(newbival, data);
        }
        switch (pad) {
            case 0:
                sequence[4] = '0';
                sequence[5] = '0';
                break;
            case 1:
                sequence[4] = '0';
                sequence[5] = '1';
                break;
            case 2:
                sequence[4] = '1';
                sequence[5] = '0';
                break;
            case 3:
                sequence[4] = '1';
                sequence[5] = '1';
                break;
            default:
                puts("Invalid padding argument!");
                exit(1);
        }
        sequence[6] = (charNum%2) ? '1' : '0';
        // printf("more %s %s %s %s %s %d\n", bits, newbits, data, bival, newbival, pad);
        count += 1;

        // Write data to log File for extraction
        fprintf(lg, "%d %d %d \n", i, j, bintochar(sequence));

        // Read new char. for embedding
        // Increment the char count of embedded data
        charNum++;
        char binval = input[charNum];

        // Check if file containing data to embed reached its end
        if (binval == '\x00') {
            // Activate complete flag
            completed = 1;

            // Return new pixel value after embedding
            int c = bintochar(newbival);
            free(sequence);
            return (c<0) ? 256 - c : c;
        }

        // Check if file containing data to embed havent reached its end
        bits = chartobin(binval);

        // Return new pixel value after embedding
        int c = bintochar(newbival);
        free(sequence);
        return (c<0) ? 256 - c : c;
    }
}

/* Takes adjacent pixels and average their RGB values for smoother outputs */

bitmap_t antialias(bitmap_t temp) {
    // Create output bitmap
    bitmap_t output;
    output.width = temp.width;
    output.height = temp.height;

    output.pixels = calloc (output.width * output.height, sizeof (pixel_t));

    if (! output.pixels) {
        puts("Error in allocation!");
        exit(1);
    }

    // Create temporary bitmap for edge cases
    pixel_t * black = (pixel_t*)malloc(sizeof(pixel_t*));
    black->red = 0; black->blue = 0; black->green = 0;

    // Check if edge case
    int is_edge = 0;

#pragma omp parallel for private(is_edge)
    for (int x = 0; x < output.width; x++) {
        for (int y = 0; y < output.height; y++) {
            // Solve for edge cases
            pixel_t * pixelxp = (x==output.width-1) ? black : pixel_at (& temp, x+1, y);
            pixel_t * pixelxm = (x==0) ? black : pixel_at (& temp, x-1, y);
            pixel_t * pixelyp = (y==output.height-1) ? black : pixel_at (& temp, x, y+1);
            pixel_t * pixelym = (y==0) ? black : pixel_at (& temp, x, y-1);
            pixel_t * pixel = pixel_at (& output, x, y);
            // Average for edge cases
#pragma omp critical
            {
                if (x==output.width-1||x==0||y==output.height-1||y==0) is_edge = 1;
                pixel->red = (pixelxp->red + pixelxm->red + pixelyp->red + pixelym->red)/((is_edge) ? 3 : 4);
                pixel->blue = (pixelxp->blue + pixelxm->blue + pixelyp->blue + pixelym->blue)/((is_edge) ? 3 : 4);
                pixel->green = (pixelxp->green + pixelxm->green + pixelyp->green + pixelym->green)/((is_edge) ? 3 : 4);
                is_edge = 0;
            }
        }
    }

    free(black);

    return output;
}

/* Verify generated fractal bitmap is coloured */

int verify(bitmap_t output) {
    int sum = 0;
    for (int x = 0; x < output.width; x++) {
        for (int y = 0; y < output.height; y++) {
            pixel_t * pixel = pixel_at (& output, x, y);
            sum += pixel->red + pixel->blue + pixel->green;
            if (sum > 0) return 1;
        }
    }
    // Will return 0 if entire bitmap is black
    return 0;
}

// Embed bits with randomized index
int handlebits(int k, int l, bitmap_t output, int rref, int gref, int bref, FILE *fptr, char* input) {
    int r, g, b, rdif, gdif, bdif;
    int newr, newg, newb;

    // Calculate the difference in pixel values
    pixel_t * pixel = pixel_at (& output, k, l);
    r = pixel->red;
    g = pixel->green;
    b = pixel->blue;
    rdif = abs(r - rref);
    gdif = abs(g - gref);
    bdif = abs(b - bref);
    // printf("%d %d %d, %d %d %d\n", r, g, b, rdif, gdif, bdif);

    // Till embedding gets completed
    if (!completed) {
        newr = embedbits(k, l, 'r', classify(rdif), r, fptr, input);
    }
    if (!completed) {
        newg = embedbits(k, l, 'g', classify(gdif), g, fptr, input);
    }
    if (!completed) {
        newb = embedbits(k, l, 'b', classify(bdif), b, fptr, input);
    }
    // printf("%d %d %d, %d %d %d, [%d, %d]\n", r, g, b, newr, newg, newb, k, l);

    // Embedding completed
    if (completed) {
        // Assign modified pixel values
        pixel->red = newr;
        pixel->green = newg;
        pixel->blue = newb;

        // Save embedded image
        if (save_png_to_file (& output, "embedded.png")) {
            fprintf (stderr, "Error writing file.\n");
            exit(1);
        } else {
            puts("Embedded image outputted to ./embedded.png");
        }

        // Exit program
        return 0;
    }

    // Assign modified pixel values
    pixel->red = newr;
    pixel->green = newg;
    pixel->blue = newb;
    return 1;
}

// Handle embedding pixels using PVD
int handle(int i, int j, bitmap_t output, int rref, int gref, int bref, FILE *fptr, char* input) {
    int handlebits_ret;
    // For all pixels in the matrix
#pragma omp parallel
    for (int k=i; k<i+3; k++) {
        if (k >= output.height) break;
        for (int l=j; l<j+3; l++) {
            if (k == i+1 && l == j+1) continue;
            if (l >= output.width) break;
            handlebits_ret = handlebits(k, l, output, rref, gref, bref, fptr, input);

            // Check handlebits return value
            if (handlebits_ret == 0)
                return handlebits_ret;
        }
    }
}
