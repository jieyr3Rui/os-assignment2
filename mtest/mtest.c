#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <alloca.h>
#include <stdlib.h>
extern void move(int ,char ,char ,char);

int bss_var;
int data_var = 42;
int main(int argc, char **argv){


	char *p, *b, *nb;
    printf("Text Locations:\n");
    printf("\tAddress of main: %p\n", main);
    printf("\tAddress of move: %p\n", move);

    printf("Stack Locations:\n");

	int N; 
	if(argc == 2) N = atoi(argv[1]);
	else N = 3;
	//printf("i got N = %d.\n", N);
	move(N,'A','B','C');

	p = (char *) alloca(32);
    if (p != NULL) {
        printf("\tStart of alloca()'ed array: %p\n", p);
        printf("\tEnd of alloca()'ed array: %p\n", p + 31);
    }
	printf("Data Locations:\n");
    printf("\tAddress of data_var: %p\n", & data_var);

    printf("BSS Locations:\n");
    printf("\tAddress of bss_var: %p\n", & bss_var);

    b = sbrk((ptrdiff_t) 32); /* grow address space */
    nb = sbrk((ptrdiff_t) 0);
    printf("Heap Locations:\n");
    printf("\tInitial end of heap: %p\n", b);
    printf("\tNew end of heap: %p\n", nb);

    b = sbrk((ptrdiff_t) -16); /* shrink it */
    nb = sbrk((ptrdiff_t) 0);
    printf("\tFinal end of heap: %p\n", nb);
	return 0;
}

void move(int n,char a,char b,char c){
	static int level = 0;
	auto int stack_var;
	if(n == 1){
		//printf("Move disk 1 from column %c to column %c.\n",a,c);
		return;
	}

	else{
		printf("\tStack level %d: address of stack_var: %p\n", level, & stack_var);
		move(n -1,a,c,b);
		//printf("Move disk %d from column %c to column %c.\n",n,a,c);
		move(n-1,b,a,c);
	}
}
