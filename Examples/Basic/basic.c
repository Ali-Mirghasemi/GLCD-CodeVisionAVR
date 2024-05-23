#include <io.h>
#include "..\..\Src\GLCD.h"

void main(void)
{

    GLCD_init();
    GLCD_clear();
    
    GLCD_puts("Hello World", 0, 0, GLCD_FONT1, 1);

while (1)
    {
    // Please write your application code here

    }
}
