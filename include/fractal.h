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
                    puts("Invalid fractal argument!");
                    exit(0);
            }
        }
    }
    return output;
}
