#include <stdio.h>
#include "log.h"

using namespace std;

int main()
{
    LogCreate();
    FILE *img = fopen("res/imagem_1280x720_imagemSteg_30x30_5bits.y", "r");
    if (img == NULL)
    {
        LogErr("Error opening file");
    }

    return 0;
}