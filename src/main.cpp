#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <openssl/md5.h>
#include <vector>
#include "client.c"

#define HASH_SIZE 32

FILE *imgFile, *outFile, *keyFile, *hashFile;
std::vector<char> hashBits, stegHash;
char stegBits[8];
char *key;
bool hash_mode = 0;
int steg_w, steg_h, nbits;

void init(char* argv[]);
void getBits(int, char);
char convertToByte(char*);
std::vector<char> md5sum(FILE*);
void quit();

int main(int argc, char *argv[])
{
    const int img_w = 1280, img_h = 720;
    std::vector<char> digest;

    if (argc < 6)
    {
        printf("Usage: ./client <input-image> <steg-width> <steg-height> <bits> <key-file> \n");
        exit(0);
    }

    init(argv);

    long k=-1, pixels=0;
    for (int row_block=0; pixels < steg_w * steg_h + HASH_SIZE; row_block++)
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

            if (hash_mode) {
                int pos = ftell(imgFile);
                char restoredByte = byte >> nbits << nbits;
                fseek(imgFile, pos-1, SEEK_SET);
                fwrite(&restoredByte, sizeof(char), sizeof(char), imgFile);
                fseek(imgFile, pos, SEEK_SET);
            }

            if (pixels == steg_w * steg_h) {
                hash_mode = 1;
            } else if (pixels == steg_w * steg_h + HASH_SIZE) {
                break;
            }
        }
    }

    digest = md5sum(imgFile);

    if (digest != stegHash) {
        printf("The file is unhealthy. The program will stop.\n");
    } else {
        printf("The file is OK. Sending secret file to server...\n");
        sendToServer(outFile);
    }

    quit();

    return 0;
}

void init(char* argv[])
{
    imgFile = fopen(argv[1], "r+b");
    if (imgFile == NULL)
    {
        printf("Couldn't open %s.\n", argv[1]);
        exit(0);
    }

    steg_w = atoi(argv[2]);
    steg_h = atoi(argv[3]);
    nbits = atoi(argv[4]);

    keyFile = fopen(argv[5], "r");
    if (keyFile == NULL)
    {
        printf("Couldn't open %s.\n", argv[5]);
        exit(0);
    }

    key = (char *) malloc(sizeof(char) * 100);
    fgets(key, 100, keyFile);

    hashFile = fopen("hash.txt", "w+");
    outFile = fopen("out.y", "w+");
    fprintf(outFile, "P5 %d %d 255 ", steg_w, steg_h);
}

void getBits(int nbits, char byte)
{
    if (hash_mode) {
        for (int i = 0; i < nbits; ++i)
        {
            char bit = (byte >> i) & 0x01;
            hashBits.push_back(bit);

            if (hashBits.size() == CHAR_BIT)
            {
                char byte = convertToByte(&hashBits[0]);
                fprintf(hashFile, "%c", byte);
                stegHash.push_back(byte);
                hashBits.clear();
            }
        }
    } else {
        for (int i = 0; i < nbits; ++i)
        {
            char bit = (byte >> i) & 0x01;
            stegBits[i] = bit;
        }
        char stegByte = convertToByte(stegBits);
        fprintf(outFile, "%c", stegByte);
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

std::vector<char> md5sum(FILE *file)
{

    fseek(file, 0, SEEK_SET);
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];
    unsigned char c[MD5_DIGEST_LENGTH];
    std::vector<char> digest;
    MD5_Init(&mdContext);
    while ((bytes = fread (data, 1, 1024, file)) != 0) {
        MD5_Update(&mdContext, data, bytes);
    }
    MD5_Final(c, &mdContext);
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        digest.push_back(c[i]);
        // printf("%02x", c[i]);
    }
    // printf("\n");

    return digest;
}

void quit()
{
    fclose(keyFile);
    fclose(imgFile);
    fclose(outFile);
    fclose(hashFile);
    free(key);
}