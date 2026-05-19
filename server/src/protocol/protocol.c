#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "protocol.h"

void generate_receipt(char *buffer)
{
    srand(time(NULL));

    sprintf(
        buffer,
        "REC-%d-%d",
        rand() % 100000,
        rand() % 100000
    );
}
