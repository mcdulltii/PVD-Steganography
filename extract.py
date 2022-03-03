#! /usr/bin/python3
from PIL import Image
import sys, os


# Ensure binary strings are at least 0bXX
def checkbin(bin_val):
    assert bin_val[:2] == '0b'
    if len(bin_val) == 3:
        return bin_val[:2] + '0' + bin_val[2]
    return bin_val


def main():
    # File Object Creations
    im = Image.open(sys.argv[1])
    outp = open(sys.argv[2],'w')
    lg = open('embed.log','r')

    # Initialisation
    pix = im.load()
    temp = 0
    chrtr = ''

    while 1:
        # Read each line from log file
        st = lg.readline()

        # Check if log file reached its end
        if len(st) == 0:
            # Write extracted data to file
            outp.write(chr(int(chrtr, 2)))
            break

        # Unpack line read from log file to variables
        # Process variables (Refer to embed.h for logging format)
        i, j, sequence = [int(i) for i in st.split()]
        pixel, diff, pad = [i for i in map(''.join, zip(*[iter(bin(sequence)[2:].zfill(7))]*2))]
        pixel, diff, pad = 'rgb'[int(pixel, 2)], int('234'[int(diff, 2)]), int('0123'[int(pad, 2)])
        charNum = int(bin(sequence)[-1])
        r, g, b = pix[i, j]

        # Check if new charaacter in embed log is reached
        if temp != charNum:
            outp.write(chr(int(chrtr, 2)))
            chrtr = ''

        # Red pixel
        if pixel == 'r':
            binr = checkbin(bin(r))

        # Green pixel
        elif pixel == 'g':
            binr = checkbin(bin(g))

        # Blue pixel
        elif pixel == 'b':
            binr = checkbin(bin(b))

        # Check if binary value is stripped
        if len(binr[2:]) < diff:
            binr = binr[:2] + '0'*(diff - len(binr[2:])) + binr[2:]

        chrtr += binr[(len(binr) - diff) :]
        # print(binr, chrtr)

        # Unpad if padding is done
        if pad != 0:
            chrtr = chrtr[: (len(chrtr) - pad)]

        # Check if character has changed in embed file
        temp = charNum

    # Close File Objects
    outp.close()
    lg.close()
    print("Extracted to {}. Now exiting!".format(sys.argv[2]))


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python3 extract.py embedded.png <Output file>")
        sys.exit()
    main()
