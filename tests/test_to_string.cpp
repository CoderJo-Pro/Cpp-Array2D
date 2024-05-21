#include "log.h"
#include <to_string.h>

int main()
{
    using arr2d::to_string;

    log(to_string("Hello World for ", 100, " times!"));
    return 0;
}
