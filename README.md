# C Implementation of Pixel Value Differencing based Steganography
[![](https://img.shields.io/badge/Category-Steganography-E5A505?style=flat-square)]() [![](https://img.shields.io/badge/Language-C-E5A505?style=flat-square)]() [![](https://img.shields.io/badge/Version-1.2-E5A505?style=flat-square&color=green)]()

Reference: [Python Implementation](https://github.com/TonyJosi97/pvd_steganography)

LSB substitution and PVD are applied. In PVD, adaptive non-overlapping 3x3 pixel blocks or a combination of 3x3 and 2x2 blocks are used in raster fashion.

As of now extraction is done using the generated log file containing data locations using Python.

## Prerequisites

- libpng-dev (C PNG Library)
- libmath (C Inbuilt Math Library)
- openmp (C Parallelization)

## Usage: Embedding

Note: Makefile uses `text` as input to C binary

```shell
$ echo 'Hello world!' > text
$ make (icc/strip-icc/gcc/strip-gcc/parallel-gcc/secure-gcc/all-gcc) (run) (verify)
```

> Embed data Log can be found as: embed.log

## Usage: Extraction

Note: Makefile uses `embedded.png` and `text` as inputs below

```shell
$ python3 extract.py (Embedded_Image) (Output_File) 
```

> extract.py uses embed.log from the same directory

## Implementation

1. Fractal bitmap generation
    1. The binary uses `srand(time(NULL))` to choose a random fractal algorithm.
    2. Randomly generate 2 of the quartenion vector directions
    3. Use fractal algorithm to iterate bitmap pixel colours of set dimensions
2. Check bitmap generation
    1. Anti-alias bitmap with adjacent pixels
    2. Verify bitmap is not generated entirely black
3. Randomly choose 3x3 grids within bitmap to embed
    1. Using floored division of the bitmap dimensions, generate non-repeating `(x//3, y//3)` coordinates
    2. Use randomly chosen `(x//3, y//3) * 3` 3x3 matrix to embed bits

    Example:
    <table>
    <tr><th>i.</th><th>ii.</th></tr>
    <tr><td>

    |1|-|-|...|
    |-|-|-|-|
    |-|2|-|...|
    |-|3|-|...|
    |...|...|...|...|

    </td><td>

    |1|1|1|-|-|-|-|-|-|...|
    |-|-|-|-|-|-|-|-|-|-|
    |1|1|1|-|-|-|-|-|-|...|
    |1|1|1|-|-|-|-|-|-|...|
    |-|-|-|2|2|2|-|-|-|...|
    |-|-|-|2|2|2|-|-|-|...|
    |-|-|-|2|2|2|-|-|-|...|
    |-|-|-|3|3|3|-|-|-|...|
    |-|-|-|3|3|3|-|-|-|...|
    |-|-|-|3|3|3|-|-|-|...|
    |...|...|...|...|...|...|...|...|...|...|

    </td></tr> </table>

4. Embed bits
    1. Classify bits to determine how much to embed
    2. Determine capacity of RGB bits can be embedded using reference to center of the 3x3 matrix
    3. Embed bits based on chosen 3x3 matrices (or 2x2 within 3x3 matrices)

    Example:

    |x|x|x|-|-|-|-|-|-|...|
    |-|-|-|-|-|-|-|-|-|-|
    |x|x|x|-|-|-|-|-|-|...|
    |x|x|x|-|-|-|-|-|-|...|
    |-|-|-|x|x|-|-|-|-|...|
    |-|-|-|x|x|-|-|-|-|...|
    |-|-|-|-|-|-|-|-|-|...|
    |-|-|-|x|x|x|-|-|-|...|
    |-|-|-|x|x|x|-|-|-|...|
    |-|-|-|x|x|x|-|-|-|...|
    |...|...|...|...|...|...|...|...|...|...|

5. Log embedded bits
    1. Log variables (x, y), RGB pixel bit used, embedded length and amount of padding used, change in index of input
    2. Log variables as concatenated binary bits to reduce logging
6. Extract embedded bits
    1. Using index of input embedded, it can determine how many bits are used for each input character
    2. Retrieve 3x3 matrix used from (x, y), or 2x2 within 3x3 matrix
    3. As some RGB pixels may have values smaller than the bits to embed, splice the embedded bits into its original length 

## Reading Materials

- [Capacity Enlargement Of The PVD Steganography Method Using The GLM Technique](https://arxiv.org/ftp/arxiv/papers/1601/1601.00299.pdf)
- [Digital Image Steganography Using LSB Substitution, PVD, and EMD](http://downloads.hindawi.com/journals/mpe/2018/1804953.pdf)
- [High Capacity Image Steganography Using Modified LSB Substitution and PVD against Pixel Difference Histogram Analysis](http://downloads.hindawi.com/journals/scn/2018/1505896.pdf)
- [Adaptive PVD Steganography Using Horizontal, Vertical, and Diagonal Edges in Six-Pixel Blocks](http://downloads.hindawi.com/journals/scn/2017/1924618.pdf)
- [Digital Image Steganography Using Eight-Directional PVD against RS Analysis and PDH Analysis](http://downloads.hindawi.com/journals/am/2018/4847098.pdf)
- [A Steganographic Method Based on Pixel-Value Differencing and the Perfect Square Number](http://downloads.hindawi.com/journals/jam/2013/189706.pdf)
- [An RGB colour image steganography scheme using overlapping block-based pixel-value differencing](https://royalsocietypublishing.org/doi/10.1098/rsos.161066)

