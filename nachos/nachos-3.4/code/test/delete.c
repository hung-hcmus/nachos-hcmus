#include "syscall.h"
#include "copyright.h"
#define maxlen 32
#define stdin 0
#define stdout 1

int main(){
	Open("text.txt",0);
	Delete("text.txt");
	Delete("test.txt");
	
}
