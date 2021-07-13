#include "syscall.h"
#include "copyright.h"
#define maxlen 100
#define stdin 0
#define stdout 1

int main(){
	int text;
	int len,lentext;
	char buf[maxlen],buflen[maxlen];
	len = Read(buf,maxlen,stdin);
	if (len != -1 && len != -2){
		text = Open(buf,1);
		if (text != -1){
			lentext = Read(buflen,maxlen,text);
			if(lentext != -1 && lentext != -2){
				Write(buflen,lentext,stdout);
			}
		}
		Close(text);
	}
}
