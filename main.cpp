#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

// #define DEBUG

FILE *outFile;
char bits[8];
int total_bits=0;
int total_grupos=0;

void getBits(int, char);
char convertToByte(char*);

int main(int argc, char *argv[])
{
    FILE *keyFile;
    FILE *imgFile;
    char *key;
    int width;
    int height;
    int nbits;

    if (argc < 4)
    {
        printf("Usage: ./main <width> <height> <bits>\n");
        exit(0);
    }

    width = atoi(argv[1]);
    height = atoi(argv[2]);
    nbits = atoi(argv[3]);

    keyFile = fopen("res/key.txt", "r");
    outFile = fopen("out.y", "w+");
    if (keyFile == NULL)
    {
        printf("Couldn't open res/key.txt.\n");
        exit(0);
    }

    key = (char *) malloc(sizeof(char) * 100);
    fgets(key, 100, keyFile);

    // imgFile = fopen("res/teste_EsteganografiaLSB_1280x720_1bit.y", "r");
    imgFile = fopen("res/imagem_1280x720_imagemSteg_30x30_5bits.y", "r");
    // imgFile = fopen("res/1280_720", "r");
    // imgFile = fopen("res/44_6", "r");
    if (imgFile == NULL)
    {
        printf("Couldn't open image.\n");
        exit(0);
    }

    int k=-1;
    for (int rowblock=0; rowblock < height/3; rowblock++)
    {
        for (int colblock=0; colblock < width/3; colblock++)
        {
            total_grupos++;
            if (colblock < width) ++k;
            int row = (key[k%strlen(key)]-'0'-1)%3;
            if (row < 0) continue;
            int col = (key[k%strlen(key)]-'0'-1)/3;
            int posPixel = width*(rowblock*3 + row) + colblock*3 + col;
            fseek(imgFile, posPixel, SEEK_SET);
            char byte = fgetc(imgFile);
            getBits(nbits, byte);
        }
    }

    #ifdef DEBUG
    printf("total de bits: %d\n", total_bits);
    printf("total de grupos: %d\n", total_grupos);
    #endif

    fclose(keyFile);
    fclose(imgFile);
    fclose(outFile);
    free(key);

    return 0;
}

void getBits(int nbits, char byte)
{
    for (int i = 0; i < nbits; ++i)
    {
        char bit = (byte >> i) & 0x01;
        bits[i] = bit;
        total_bits++;
    }

    char outByte = convertToByte(bits);
    fprintf(outFile, "%c", outByte);
}

char convertToByte(char* bits)
{
    unsigned char byte = 0x0;
    for (int i=7, j=0; i >= 0; --i, ++j)
    {
        if (bits[j] == 0x01)
        {
            byte = (byte >> i) | 0x01;
            byte <<= i;
        }
    }

    return byte;
}
