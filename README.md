# C Implementation of Pixel Value Differencing based Steganography
[![](https://img.shields.io/badge/Category-Steganography-E5A505?style=flat-square)]() [![](https://img.shields.io/badge/Language-C-E5A505?style=flat-square)]() [![](https://img.shields.io/badge/Version-1.0-E5A505?style=flat-square&color=green)]()

Reference: [Python Implementation](https://github.com/TonyJosi97/pvd_steganography)

LSB substitution and PVD are applied. In PVD, adaptive non-overlapping 3x3 pixel blocks or a combination of 3x3 and 2x2 blocks are used in raster fashion.

As of now extraction is done using the generated log file containing data locations using Python.

## Prerequisites

- libpng-dev (C PNG Library)
- libmath (C Inbuilt Math Library)

## Usage: Embedding

> Usage: make

> Embed data Log can be found as: embedlog.log

## Usage: Extraction

> Usage: python3 extract.py Embedded_Image Output_File 

> Eg:    python3 extract.py embedded.png cipher.txt

Ensure that embedlog.log is in the same directory.

## Reading Materials

- [Capacity Enlargement Of The PVD Steganography Method Using The GLM Technique](https://arxiv.org/ftp/arxiv/papers/1601/1601.00299.pdf)
- [Digital Image Steganography Using LSB Substitution, PVD, and EMD](http://downloads.hindawi.com/journals/mpe/2018/1804953.pdf)
- [High Capacity Image Steganography Using Modified LSB Substitution and PVD against Pixel Difference Histogram Analysis](http://downloads.hindawi.com/journals/scn/2018/1505896.pdf)
- [Adaptive PVD Steganography Using Horizontal, Vertical, and Diagonal Edges in Six-Pixel Blocks](http://downloads.hindawi.com/journals/scn/2017/1924618.pdf)
- [Digital Image Steganography Using Eight-Directional PVD against RS Analysis and PDH Analysis](http://downloads.hindawi.com/journals/am/2018/4847098.pdf)
- [A Steganographic Method Based on Pixel-Value Differencing and the Perfect Square Number](http://downloads.hindawi.com/journals/jam/2013/189706.pdf)
- [An RGB colour image steganography scheme using overlapping block-based pixel-value differencing](https://royalsocietypublishing.org/doi/10.1098/rsos.161066)

