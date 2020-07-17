#include <ctype.h>
#include <stdio.h>
#include <string.h>

char *test_tab = "*/5 * *"

int main(void)
{
    int value;
    char *pchar;
    pchar = test_tab;
    if(memcmp(pchar, "*", 1) == 0)
    {
        ref->sec = -1;
    }
    else if(memcmp(pchar, "*/", 2) == 0)
    {
        sscanf(pchar, "*/%d ", &value);
        if((value >= 60)||(value < 0)) return -1; 
        ref->sec = value;
    }
    else if(isdigit(pchar[0]))
    {
        sscanf(pchar, "*/%d ", &value);
        if((value >= 60)||(value < 0)) return -1; 
        ref->sec = value;
    }
    else
    {
        return -1;
    }
    printf
    return 0;
}
