#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

// #define DEBUG

FILE *outFile;
char bits[8];
int total_bits=0;
int total_groups=0;

void getBits(int, char);
char convertToByte(char*);

int main(int argc, char *argv[])
{
    FILE *keyFile;
    FILE *imgFile;
    char *key;
    const int img_w = 1280, img_h = 720;
    int steg_w, steg_h;
    int nbits;

    if (argc < 4)
    {
        printf("Usage: ./main <steg-width> <steg-height> <nbits>\n");
        exit(0);
    }

    steg_w = atoi(argv[1]);
    steg_h = atoi(argv[2]);
    nbits = atoi(argv[3]);

    keyFile = fopen("res/key.txt", "r");
    if (keyFile == NULL)
    {
        printf("Couldn't open res/key.txt.\n");
        exit(0);
    }

    outFile = fopen("out.y", "w+");
    fprintf(outFile, "P5 %d %d 255 ", steg_w, steg_h);

    key = (char *) malloc(sizeof(char) * 100);
    fgets(key, 100, keyFile);

    // imgFile = fopen("res/teste_EsteganografiaLSB_1280x720_1bit.y", "r");
    imgFile = fopen("res/imagem_1280x720_imagemSteg_30x30_5bits.y", "r");
    if (imgFile == NULL)
    {
        printf("Couldn't open image.\n");
        exit(0);
    }

    int k=-1, pixels=0;
    for (int rowblock=0; rowblock < img_h/3; rowblock++)
    {
        for (int colblock=0; colblock < img_w/3; colblock++)
        {
            total_groups++;
            if (colblock < img_w) ++k;
            int row = (key[k%strlen(key)]-'0'-1)%3;
            if (row < 0) continue;
            int col = (key[k%strlen(key)]-'0'-1)/3;
            int posPixel = img_w*(rowblock*3 + row) + colblock*3 + col;
            fseek(imgFile, posPixel, SEEK_SET);
            char byte = fgetc(imgFile);
            getBits(nbits, byte);
            if (++pixels == steg_w * steg_h) break;
        }
    }

    #ifdef DEBUG
    printf("total de bits: %d\n", total_bits);
    printf("total de grupos: %d\n", total_groups);
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
