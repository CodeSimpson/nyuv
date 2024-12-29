#!/bin/bash

TEST_COUNT=10
for i in $(seq 1 $((TEST_COUNT))); do

    ./testintreface data - 500 500 YUV420SPNV12
    ./testintreface data - 500 500 YUV420SPNV21
    ./testintreface data - 500 500 Bayer8888GRBG
    ./testintreface data - 500 500 Bayer8888RGGB
    ./testintreface data - 500 500 Bayer8888GBRG
    ./testintreface data - 500 500 Bayer8888BGGR
    ./testintreface data - 500 500 MIPI10GRBG
    ./testintreface data - 500 500 MIPI10RGGB
    ./testintreface data - 500 500 MIPI10GBRG
    ./testintreface data - 500 500 MIPI10BGGR
    ./testintreface data - 500 500 UPRGB14
    ./testintreface data - 500 500 UPRGBA14
    ./testintreface data - 500 500 UPARGB14
    ./testintreface data - 500 500 RGB16
    ./testintreface data - 500 500 RGBA16
    ./testintreface data - 500 500 ARGB16

done