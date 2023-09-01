#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <utils/hello.h>
int main(int argc, char* argv[]) {
	say_hello("hola");
	char **arr=string_array_new();
    puts("Hello world!!");
    return 0;
}
