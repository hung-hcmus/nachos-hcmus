#include "syscall.h"
#include "copyright.h"
#define maxlen 32

int main(){
/*	char buf[2]="he";
	int len;
	Open("text.txt",1);

	if (Open("test.txt",1) == -1) Write(buf,2,1);
*/
	Open("text.txt",0);
	Delete("text.txt");
	Delete("test.txt");
	
}
