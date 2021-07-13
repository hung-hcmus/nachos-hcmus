#include "syscall.h"
#include "copyright.h"
#define maxlen 100
#define stdin 0
#define stdout 1

int main(){
	char NameFileFrom[maxlen],NameFileTo[maxlen],DataFile[maxlen];
	int HaveOpenedFrom,HaveOpenedTo;
	int LengthNameFrom,LengthNameTo,LengthDataFile;
	LengthNameFrom = Read(NameFileFrom,maxlen,stdin);
	LengthNameTo = Read(NameFileTo,maxlen,stdin);
	if (LengthNameTo != -1 && LengthNameTo != -2 && LengthNameTo != 0 && LengthNameFrom != -1 && LengthNameFrom != -2 && LengthNameFrom != 0){
		HaveOpenedFrom = Open(NameFileFrom,1);
		HaveOpenedTo = Open(NameFileTo,0);
		if (HaveOpenedTo != -1 && HaveOpenedFrom != -1){
//			Seek(-1,HaveOpenedFrom);
			LengthDataFile = Read(DataFile,maxlen,HaveOpenedFrom);
			if (LengthDataFile != -1 && LengthDataFile != -2){
				Write(DataFile,LengthDataFile,HaveOpenedTo);
			}
			Close(HaveOpenedFrom);
			Close(HaveOpenedTo);
		}
	}
}
