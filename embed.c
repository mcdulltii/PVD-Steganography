#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* A coloured pixel. */

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
pixel_t;

/* A picture. */
    
typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;
    
/* Global Variables */

int completed = 0;
int charNum = 0;
int count = 0;
char *paddbits = "0000000";
char *bits = "";
char *input;

/* Given "bitmap", this returns the pixel of bitmap at the point 
   ("x", "y"). */

static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}
    
/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
   success, non-zero on error. */

static int save_png_to_file (bitmap_t *bitmap, const char *path)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;
    
    fp = fopen (path, "wb");
    if (! fp) {
        goto fopen_failed;
    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    
    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    
    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */

    row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            pixel_t * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    
    /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;
    
    for (y = 0; y < bitmap->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}

/* Given "value" and "max", the maximum value which we expect "value"
   to take, this returns an integer between 0 and 255 proportional to
   "value" divided by "max". */

static int pix (int value, int max)
{
    if (value < 0) {
        return 0;
    }
    return (int) (256.0 *((double) (value)/(double) max));
}

// Obtain substring from string wrt index

char *substr(char const *input, size_t start, size_t len) { 
    char *ret = calloc(8, len+1);
    memcpy(ret, input+start, len);
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

int calcCapacity(int capacity, int lix, int liy, bitmap_t output) {
    int r, g, b, rref, gref, bref, rdif, gdif, bdif;

    // Divide pixels to [3 x 3] matrix
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

int embedbits(int i, int j, char pixel, int diff, int colorpixel, FILE *lg) {
    // Initialise
    int pad = 0;
    int nb = diff;
    // printf("%d %ld\n", nb, strlen(bits));

    // If the number of bits required is less than the number of bits in the data(char.) to be Embedded
    if (nb < strlen(bits)) {
        // Initialise
        char *newbits = substr(bits,0,nb);
        bits = substr(bits,nb,strlen(bits)-nb);
        int val = colorpixel;
        char *data = newbits;
        char *bival = chartobin(val);
        char newbival[8];
        memset(newbival, '\0', sizeof(newbival));
        if (strlen(bival)>strlen(data)) {
            char *tmp = substr(bival,0,(strlen(bival) - strlen(data)));
            strcat(tmp, data);
            strcpy(newbival, tmp);
        } else {
            strcpy(newbival, bival);
        }
        // printf("less %s %s %s %s %s\n", bits, newbits, data, bival, newbival);

        // Write data to log File for extraction
        fprintf(lg, "%d %d %c %d %d %d \n", i, j, pixel, diff, pad, charNum+1);

        // Return new pixel value after embedding
        int c = bintochar(newbival);
        return (c<0) ? 256 - c : c;
    } else {
    // If the number of bits required is greater than the number of bits in the data(char.) to be Embedded
        // Apply padding
        char newbits[8];
        memset(newbits, '\0', sizeof(newbits));
        strcpy(newbits, bits);
        strcat(newbits, substr(paddbits,0,(nb - strlen(bits))));
        int pad = nb - strlen(bits);
        int val = colorpixel;
        char *data = newbits;
        char *bival = chartobin(val);
        char newbival[8];
        memset(newbival, '\0', sizeof(newbival));
        if (strlen(bival)>strlen(data)) {
            char *tmp = substr(bival,0,(strlen(bival) - strlen(data)));
            strcat(tmp, data);
            strcpy(newbival, tmp);
        } else {
            strcpy(newbival, bival);
        }
        // printf("more %s %s %s %s %s %d\n", bits, newbits, data, bival, newbival, pad);
        count += 1;

        // Write data to log File for extraction
        fprintf(lg, "%d %d %c %d %d %d \n", i, j, pixel, diff, pad, charNum+1);

        // Read new char. for embedding
        // Increment the char count of embedded data
        charNum++;
        char binval = input[charNum];

        // Check if file containing data to embed reached its end
        if (binval == '\x0a') {
            // Activate complete flag
            completed = 1;

            // Return new pixel value after embedding
            int c = bintochar(newbival);
            return (c<0) ? 256 - c : c;
        }

        // Check if file containing data to embed havent reached its end
        bits = chartobin(binval);

        // Return new pixel value after embedding
        int c = bintochar(newbival);
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
        exit(0);
    }

    // Create temporary bitmap for edge cases
    pixel_t * black = (pixel_t*)malloc(sizeof(pixel_t*));
    black->red = 0; black->blue = 0; black->green = 0;

    // Check if edge case
    int is_edge = 0;

    for (int x = 0; x < output.width; x++) {
        for (int y = 0; y < output.height; y++) {
            // Solve for edge cases
            pixel_t * pixelxp = (x==output.width-1) ? black : pixel_at (& temp, x+1, y);
            pixel_t * pixelxm = (x==0) ? black : pixel_at (& temp, x-1, y);
            pixel_t * pixelyp = (y==output.height-1) ? black : pixel_at (& temp, x, y+1);
            pixel_t * pixelym = (y==0) ? black : pixel_at (& temp, x, y-1);
            pixel_t * pixel = pixel_at (& output, x, y);
            // Average for edge cases
            if (x==output.width-1||x==0||y==output.height-1||y==0) is_edge = 1;
            pixel->red = (pixelxp->red + pixelxm->red + pixelyp->red + pixelym->red)/((is_edge) ? 3 : 4);
            pixel->blue = (pixelxp->blue + pixelxm->blue + pixelyp->blue + pixelym->blue)/((is_edge) ? 3 : 4);
            pixel->green = (pixelxp->green + pixelxm->green + pixelyp->green + pixelym->green)/((is_edge) ? 3 : 4);
            is_edge = 0;
        }
    }

    free(black);

    return output;
}

/* Generate fractal using choice as switch case */

bitmap_t fractal(bitmap_t output, int choice) {
    int x, y;
    float dx, dy;
    float xoff, yoff;
    float ias[output.height], ibs[output.width];
    float ic, id;

    // Ensure 0<=choice<6
    choice = choice % 6;

    dy = 4;
    dx = dy * output.width / output.height;
    yoff = - dy / 2;
    xoff = - dx / 2;

    // Initial Setups
    for (y = 0; y < output.height; y++) {
        ias[y] = yoff + y * dy / output.height;
    }
    for (x = 0; x < output.width ; x++) {
        ibs[x] = xoff + x * dx / output.width;
    }

    float ia, ib, a, b, c, d, ta, tb, tc, td;
    // Random offset quartenion direction
    ic = rand()%4-2;
    id = rand()%4-2;

    for (x = 0; x < output.width; x++) {
        for (y = 0; y < output.height; y++) {
            ia = ias[y];
            ib = ibs[x];

            a = ia;
            b = ib;
            c = ic;
            d = id;

            switch (choice) {
                case 0:
                    // fractal 0
                    for (int i = 0; i < 8; i++) {
                        ta = c*c+ia;
                        tb = d*d+ib;
                        tc = b/a+ic;
                        td = a/b+id;
                        a = ta;
                        b = tb;
                        c = tc;
                        d = td;
                    }
                    break;
                case 1:
                    // fractal 1
                    for (int i = 0; i < 2; i++) {
                        ta = a+b+c+d+ia;
                        tb = a/c+ib;
                        tc = a/d+ic;
                        td = a/b+id;
                        a = ta;
                        b = tb;
                        c = tc;
                        d = td;
                    }
                    break;
                case 2:
                    // fractal 2
                    for (int i = 0; i < 5; i++) {
                        ta = a+b+c+d+ia;
                        tb = b/a+ib;
                        tc = c/a+ic;
                        td = d/a+id;
                        a = ta;
                        b = tb;
                        c = tc;
                        d = td;
                    }
                    break;
                case 3:
                    // fractal 3
                    for (int i = 0; i < 4; i++) {
                        ta = a*a+b*b+c*c+d*d+ia;
                        tb = a/b+ib;
                        tc = a/c+ic;
                        td = a/d+id;
                        a = ta;
                        b = tb;
                        c = tc;
                        d = td;
                    }
                    break;
                case 4:
                    // fractal 4
                    for (int i = 0; i < 4; i++) {
                        ta = c*c+ia;
                        tb = d*d+ib;
                        tc = a+b+ic;
                        td = a-b+id;
                        a = ta;
                        b = tb;
                        c = tc;
                        d = td;
                    }
                    break;
                case 5:
                    // fractal 5
                    for (int i = 0; i < 4; i++) {
                        ta = c/a+ia;
                        tb = d/a+ib;
                        tc = c/b+ic;
                        td = d/b+id;
                        a = ta;
                        b = tb;
                        c = tc;
                        d = td;
                    }
                    break;
                default:
                    puts("Error!");
                    exit(0);
            }

            pixel_t * pixel = pixel_at (& output, x, y);

            // Colour in pixels using generated fractal values
            switch (choice) {
                case 0:
                    // fractal 0
                    pixel->red = abs(a) * 64;
                    pixel->blue = abs(d) * 64;
                    pixel->green = abs(d) * 64;
                    break;
                case 1:
                case 2:
                    // fractal 1 & 2
                    pixel->red = abs(b-ib) * 64;
                    pixel->blue = abs(c-ic) * 64;
                    pixel->green = abs(d-id) * 64;
                    break;
                case 3:
                    // fractal 3
                    pixel->red = abs(c-ic) * 64;
                    pixel->blue = abs(c-ic) * 64;
                    pixel->green = abs(d-id) * 64;
                    break;
                case 4:
                case 5:
                    // fractal 4 & 5
                    pixel->red = abs(c-ic) * 64;
                    pixel->blue = abs(d-id) * 64;
                    pixel->green = abs(d-id) * 64;
                    break;
                default:
                    puts("Error!");
                    exit(0);
            }
        }
    }
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

int main() {
    srand(time(NULL));

    // Initialisation
    bitmap_t output;
    int dimensions = 800;
    input = (char*)calloc(2048, sizeof(char*));

    // Create an image
    output.width = dimensions;
    output.height = dimensions;

restart:
    output.pixels = calloc (output.width * output.height, sizeof (pixel_t));

	if (! output.pixels) {
		return -1;
    }

    /* Fractal Algorithm */

    output = fractal(output, (int)rand());

    /* Anti-aliasing Algorithm */

    for (int i=0; i < 2; i++) output = antialias(output);

    /* Verify Fractal */

    if (!verify(output)) {
        free (output.pixels);
        // Restart fractal generation if entire bitmap is black
        goto restart;
    }

    // Save embedded image
    if (save_png_to_file (& output, "output.png")) {
        fprintf (stderr, "Error writing file.\n");
        return -1;
    }

    // Get input string to be obfuscated
    puts("Enter string input for PVD obfuscation:");
    if (!fgets(input, 2048, stdin)) exit(0);

    // Convert first char to binary string
    char binval = input[0];
    if (!binval) {
        puts("Error!");
        exit(0);
    }
    bits = chartobin(binval);

    // More Initialisation
    FILE *fptr;
    fptr = fopen("embedlog.log","w");
    int capacity = 0;
    int lix = floor((float)output.height/3);
    int liy = floor((float)output.width/3);

    // Initialise counter containing num of bits embedded till embedding ends
    int embedded = 0;

    // Print total Embedding capacity
    printf("Total Embd. Capacity: %d\n", calcCapacity(capacity, lix, liy, output));

    int r, g, b, rref, gref, bref, rdif, gdif, bdif;
    int newr, newg, newb;

    // Divide pixels to [3 x 3] matrix
    for (int i=0; i < lix*3; i+=3) {
        for (int j=0; j < liy*3; j+=3) {
            // Obtain pixel values of ref. pixel
            pixel_t * pixelref = pixel_at (& output, i+1, j+1);
            rref = pixelref->red;
            gref = pixelref->green;
            bref = pixelref->blue;
            // printf("%d %d %d\n", rref, gref, bref);

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
                    // printf("%d %d %d, %d %d %d\n", r, g, b, rdif, gdif, bdif);

                    // Till embedding gets completed
                    if (!completed) {
                        newr = embedbits(k, l, 'r', classify(rdif), r, fptr);
                    }
                    if (!completed) {
                        newg = embedbits(k, l, 'g', classify(gdif), g, fptr);
                    }
                    if (!completed) {
                        newb = embedbits(k, l, 'b', classify(bdif), b, fptr);
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
                            return -1;
                        }

                        // Close log file
                        fclose(fptr);
                        printf("Embedded: %d bits\n", embedded);

                        // Exit program
                        puts("Original image outputted to ./output.png");
                        puts("Embedded image outputted to ./embedded.png");
                        return 0;
                    }

                    // Calculate the number of bits embedded
                    embedded += classify(rdif) + classify(gdif) + classify(bdif);

                    // Assign modified pixel values
                    pixel->red = newr;
                    pixel->green = newg;
                    pixel->blue = newb;
                }
            }
        }
    }

    // Exit if Data size greater than embedding capacity
    fclose(fptr);
    puts("Exiting... Data size greater than embedding capacity!!");
    return -1;
}
