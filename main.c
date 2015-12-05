#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: ./main <width> <height> <bits>\n");
        exit(0);
    }

    int width = atoi(argv[1]);
    int nbits = atoi(argv[2]);

    FILE *file;
    FILE *out;
    FILE *bits;
    char *buffer;
    char *key;

    file = fopen("res/key.txt", "r");
    out = fopen("out", "w");
    bits = fopen("bits", "w");
    if (file == NULL)
    {
        printf("Couldn't open res/key.txt.\n");
        exit(0);
    }

    buffer = (char *) malloc(sizeof(char) * width*3);
    key = (char *) malloc(sizeof(char) * 100);
    fgets(key, 100, file);

    file = fopen("res/teste_EsteganografiaLSB_1280x720_1bit.y", "r");
    // file = fopen("res/1280_720", "r");
    if (file == NULL)
    {
        printf("Couldn't open image.\n");
        exit(0);
    }

    char byte;
    while (!feof(file))
    {
        fgets(buffer, width*3, file);
        fprintf(out, "%s", buffer);
        int i, j;
        for (i=0, j=0; j < width; i++, j+=3)
        {
            int line = (key[i%strlen(key)]-'0'-1)%3;
            
            if (line < 0) continue;

            int column = (key[i%strlen(key)]-'0'-1)/3;
            int posPixel = width*line + j + column;
            // fseek(file, posPixel, SEEK_SET);
            // char pixel = fgetc(file);
            char pixel = buffer[posPixel];
            // pixel & 0x01
            // byte <<= 1 | pixel;

            // fprintf(out, "%c", byte);
        }

    }

    fclose(file);
    free(buffer);
    free(key);

    return 0;
}
