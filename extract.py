from PIL import Image
import sys, os


def main():
    # File Object Creations
    im = Image.open(sys.argv[1])
    outp = open(sys.argv[2],'w')
    lg = open('embedlog.log','r')

    # Initialisation
    pix = im.load()
    temp = 1
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
        # Process variables
        i, j, pixel, diff, pad, charNum = [int(i) if i not in 'rgb' else i for i in st.split()]
        r, g, b = pix[i, j]

        # Check if new charaacter in embed log is reached
        if temp != charNum:
            outp.write(chr(int(chrtr, 2)))
            chrtr = ''

        # Red pixel
        if pixel == 'r':
            binr = bin(r)

        # Green pixel
        elif pixel == 'g':
            binr = bin(g)

        # Blue pixel
        elif pixel == 'b':
            binr = bin(b)

        chrtr += binr[(len(binr) - diff) :]

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
