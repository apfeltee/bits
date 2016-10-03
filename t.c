
#include <stdio.h>
#include "n.h"

int main()
{
    fwrite(bin_data, sizeof(char), bin_size, stdout);
    return 0;
}
