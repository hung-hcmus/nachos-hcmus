#include "syscall.h"
#include "copyright.h"
#define maxlen 100
#define stdin 0
#define stdout 1

int main(){
	int len;
	char buf[maxlen];
	len = Read(buf,maxlen,stdin);
	if(len != -1 && len != -2){
		Write(buf,len,stdout);
	}
}
