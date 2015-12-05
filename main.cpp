#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <vector>

using namespace std;

FILE *outFile;
vector<char> bits;
int total_bits=0;
int total_grupos=0;

void getBits(int, char);
char convertToByte(vector<char>);

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: ./main <width> <height> <bits>\n");
        exit(0);
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int nbits = atoi(argv[3]);

    FILE *keyFile;
    FILE *imgFile;
    char *key;

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

    for (int lineblock=0; lineblock < height/3; lineblock++)
    {
        for (int i=0, j=0; i<width/3; i++, j+=3)
        {
            total_grupos++;
            int line = (key[i%strlen(key)]-'0'-1)%3;
            if (line < 0) continue;
            int column = (key[i%strlen(key)]-'0'-1)/3;
            int posPixel = width*(lineblock*3) + line*width + j + column;
            fseek(imgFile, posPixel, SEEK_SET);
            char byte = fgetc(imgFile);
            getBits(nbits, byte);
        }
    }

    printf("total de bits: %d\n", total_bits);
    printf("total de grupos: %d\n", total_grupos);

    if (not bits.empty())
    {
        int sizeToMove = CHAR_BIT - bits.size();

        for (int i=0; i < bits.size()-CHAR_BIT; ++i)
        {
            printf("%d", i+1);
            bits.push_back(0);
        }
        printf("\n");

        char byte = convertToByte(bits);
        byte >>= sizeToMove;
        fprintf(outFile, "%c", byte);
    }

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
        bits.push_back(bit);
        total_bits++;

        if (bits.size() == CHAR_BIT)
        {
            char byte = convertToByte(bits);
            fprintf(outFile, "%c", byte);
            bits.clear();
        }
    }
}

char convertToByte(vector<char> bits)
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
