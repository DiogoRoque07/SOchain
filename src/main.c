#include "../inc/main.h"

void printSEITUDO(void){
    write(1, "A ECLIPSE SABE TUDO\n",21);
}

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;
    printSEITUDO();
    return 0;
}
