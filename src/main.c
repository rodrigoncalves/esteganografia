#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1280

using namespace std;

int main()
{

    FILE *file;
    FILE *out;
    char buffer[WIDTH*3];
    char *key;

    file = fopen("res/key.txt", "r");
    out = fopen("out", "w");
    if (file == NULL)
    {
        printf("Error opening file\n");
    }

    key = (char *) malloc(sizeof(char) * 100);
    fgets(key, 100, file);

    file = fopen("res/imagem_1280x720_imagemSteg_30x30_5bits.y", "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
    }

    while (!feof(file))
    {
        fgets(buffer, WIDTH*3+1, file);

        for (int i=0, j=0; j < WIDTH; i++, j+=3)
        {
            int line = (key[i%strlen(key)]-'0'-1)%3;
            
            if (line < 0)
            {
                continue;
            }

            int column = (key[i%strlen(key)]-'0'-1)/3;
            int pixel = WIDTH*line + j + column;
            fprintf(out, "%c", buffer[pixel]);
        }   
    }

    fclose(file);
    free(key);

    return 0;
}
