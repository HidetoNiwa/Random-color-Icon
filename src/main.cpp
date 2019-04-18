#include "stdio.h"
#include <png.h>

#define X 10
#define Y 10

int main() {

    FILE *fp;
    fp =fopen("Icon.png","wb");
    fclose(fp);

    while(1){
        printf("ping\n");
    }
    return 0;
}