#include <stdio.h>

void move(int ,char ,char ,char);


int main(){
	int N;
	
	scanf("%d",&N);
	move(N,'A','B','C');
	return 0;
}

void move(int n,char a,char b,char c){
	if(n == 1)printf("Move disk 1 from column %c to column %c.\n",a,c);
	else{
		move(n -1,a,c,b);

		printf("Move disk %d from column %c to column %c.\n",n,a,c);
		move(n-1,b,a,c);

	}
}
