#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
    FILE *outFile;
    char *buffer;
    char *key;

    keyFile = fopen("res/key.txt", "r");
    outFile = fopen("out", "w");
    if (keyFile == NULL)
    {
        printf("Couldn't open res/key.txt.\n");
        exit(0);
    }

    buffer = (char *) malloc(sizeof(char) * width*3);
    key = (char *) malloc(sizeof(char) * 100);
    fgets(key, 100, keyFile);

    imgFile = fopen("res/teste_EsteganografiaLSB_1280x720_1bit.y", "r");
    // imgFile = fopen("res/1280_720", "r");
    // imgFile = fopen("res/44_6", "r");
    if (imgFile == NULL)
    {
        printf("Couldn't open image.\n");
        exit(0);
    }

    char byte;
    int lineblock;
    for (lineblock=0; lineblock < height/3; lineblock++)
    {
        int i, j;
        for (i=0, j=0; i<width/3; i++, j+=3)
        {
            int line = (key[i%strlen(key)]-'0'-1)%3;
            if (line < 0) continue;
            int column = (key[i%strlen(key)]-'0'-1)/3;
            int posPixel = width*(lineblock*3) + line*width + j + column;
            fseek(imgFile, posPixel, SEEK_SET);
            char byte = fgetc(imgFile);
            fprintf(outFile, "%c", byte);
        }
    }

    fclose(keyFile);
    fclose(imgFile);
    fclose(outFile);
    free(buffer);
    free(key);

    return 0;
}

