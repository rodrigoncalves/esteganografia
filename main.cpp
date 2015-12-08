#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <vector>

FILE *outFile, *hashFile;
std::vector<char> hash;
char bits[8];
bool hash_mode = 0;

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
        printf("Usage: ./main <steg-width> <steg-height> <bits>\n");
        exit(0);
    }

    steg_w = atoi(argv[1]);
    steg_h = atoi(argv[2]);
    nbits = atoi(argv[3]);

    keyFile = fopen("res/key2.txt", "r");
    if (keyFile == NULL)
    {
        printf("Couldn't open res/key.txt.\n");
        exit(0);
    }

    hashFile = fopen("hash.txt", "w+");
    outFile = fopen("out.y", "w+");
    fprintf(outFile, "P5 %d %d 255 ", steg_w, steg_h);

    key = (char *) malloc(sizeof(char) * 100);
    fgets(key, 100, keyFile);

    imgFile = fopen("res/imagem_1280x720_imagemSteg_30x30_5bits.y", "r");
    imgFile = fopen("res/video_1280x720_imagemSteg_1920x1080_4bits_chave1_comHash.y", "r");
    imgFile = fopen("res/video_1280x720_imagemSteg_512x512_3bits_chave2_comHash.y", "r");
    if (imgFile == NULL)
    {
        printf("Couldn't open image.\n");
        exit(0);
    }

    long k=-1, pixels=0;
    for (int row_block=0; pixels < steg_w * steg_h + 32; row_block++)
    {
        for (int col_block=0; col_block < img_w/3; col_block++)
        {
            if (col_block < img_w) ++k;
            int row = (key[k%strlen(key)]-'0'-1)%3;
            if (row < 0) continue;
            int col = (key[k%strlen(key)]-'0'-1)/3;
            int posPixel = img_w*(row_block*3 + row) + col_block*3 + col;
            fseek(imgFile, posPixel, SEEK_SET);
            char byte = fgetc(imgFile);
            getBits(nbits, byte);
            ++pixels;
            if (hash_mode)
                printf("%d: %d\n", posPixel, key[k%strlen(key)]-'0');
            if (pixels == steg_w * steg_h) {
                hash_mode = 1;
            } else if (pixels == steg_w * steg_h + 32) {
                break;
            }
        }
    }

    fclose(keyFile);
    fclose(imgFile);
    fclose(outFile);
    free(key);

    return 0;
}

void getBits(int nbits, char byte)
{
    if (hash_mode) {
        for (int i = 0; i < nbits; ++i)
        {
            char bit = (byte >> i) & 0x01;
            hash.push_back(bit);

            if (hash.size() == CHAR_BIT)
            {
                char byte = convertToByte(&hash[0]);
                fprintf(hashFile, "%c", byte);
                hash.clear();
            }
        }
    } else {
        for (int i = 0; i < nbits; ++i)
        {
            char bit = (byte >> i) & 0x01;
            bits[i] = bit;
        }
        char outByte = convertToByte(bits);
        fprintf(outFile, "%c", outByte);
    }
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
