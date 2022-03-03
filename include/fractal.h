#include <stdlib.h>
#include "pix.h"

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
                    puts("Invalid fractal argument!");
                    exit(1);
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
                    puts("Invalid fractal argument!");
                    exit(1);
            }
        }
    }
    return output;
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
