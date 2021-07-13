// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#define MaxFileLength 32

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

char* User2System(int virtAddr,int limit){
	int i,oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit+1];
	if (kernelBuf == NULL){
		return kernelBuf;
	}
	memset(kernelBuf,0,limit+1);
	for(i=0;i<limit;i++){
		machine->ReadMem(virtAddr+i,1,&oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virAddr,int len,char* buffer){
	if (len<0) return -1;
	if (len==0) return len;
	int i,oneChar;
	i=0;
	oneChar = 0;
	do{
		oneChar = (int)buffer[i];
		machine->WriteMem(virAddr+i,1,oneChar);
		i++;
	}while(i<len && oneChar != 0);
	return i;
}

void IncrementPC(){
	int pc = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg,pc);
	pc = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg,pc);
	machine->WriteRegister(NextPCReg,pc + 4);
}

void
ExceptionHandler(ExceptionType which)
{
    	int type = machine->ReadRegister(2);
	switch (which) {
      		case PageFaultException:
			DEBUG('a',"\nNo valid translation found.");
			printf("\nNo valid translation found.");
			break;
	      	case ReadOnlyException:
			DEBUG('a',"\nWrite attempted to page marked read-only.");
			printf("\nWrite attempted to page marked read-only.");
			break;
	      	case BusErrorException:
			DEBUG('a',"\nTranslation resulted in an invalid physical address.");
			printf("\nTranslation resulted in an invalid physical address.");
			break;
	      	case AddressErrorException:
			DEBUG('a',"\nUnaligned reference or one that was beyond the end of the address space.");
			printf("\nUnaligned reference or one that was beyond the end of the address space.");
			break;
	      	case OverflowException:
			DEBUG('a',"\nInteger overflow in add or sub.");
			printf("\nInteger overflow in add or sub.");
			break;
	      	case IllegalInstrException:
			DEBUG('a',"\nUnimplemented or reserved instr.");
			printf("\nUnimplemented or reserved instr.");
			break;
	      	case NumExceptionTypes:
			DEBUG('a',"\nNumber Exception Types.");
			printf("\nNumber Exception Types.");
			break;
		case NoException:
			return;
		case SyscallException:
			switch (type){
				case SC_Halt:
					DEBUG('a',"\n Shutdown, initiated by user program.");
					printf("\n\n Shutdown, initiated by user program.\n");
					interrupt->Halt();
					break;
				case SC_Create:{
					// int Create(char* filename);
					int virAddr = machine->ReadRegister(4);
					char* filename = User2System(virAddr,MaxFileLength+1);
					DEBUG('a',"\n SC_Create call ...");
					DEBUG('a',"\n Reading virtual address of filename");
					DEBUG('a',"\n Reading filename.");


					if (strlen(filename) == 0)
					{
						printf("\nFile NAME IS NOT VALID.\n");
						machine->WriteRegister(2, -1); 
						delete filename;
						IncrementPC();
						return;
					}

					if (filename == NULL){
						printf("\n NOT ENOUGH MEMORY IN SYSTEM.\n");
						machine->WriteRegister(2,-1);
						delete filename;
						IncrementPC();
						return;
					}
					DEBUG('a',"\n Finish reading filename.");
					if(!fileSystem->Create(filename,0)){
						printf("\n ERROR CREATE FILE %s.\n",filename);
						machine->WriteRegister(2,-1);
						delete filename;
						IncrementPC();
						return;
					}
					printf("\nSUCCEED CREATE FILE %s.\n",filename);
					machine->WriteRegister(2,0);
					delete filename;
					IncrementPC();
					return;
				}
				case SC_Open:{
// OpenFileId(char* filename, int type)
					int FileBuf = machine->ReadRegister(4);
					int type = machine->ReadRegister(5);
					char* FileName = User2System(FileBuf,MaxFileLength+1);


					if (strlen(FileName) == 0)
					{
						printf("\nFILE NAME IS NOT VALID.\n");
						machine->WriteRegister(2, -1); 
						delete FileName;
						IncrementPC();
						return;
					}


					if (type != 0 && type != 1){
						printf("\nTYPE OF FILE NOT VALID.\n");
						machine->WriteRegister(2,-1);
						delete FileName;
						IncrementPC();
						return;
					}
					
					if (fileSystem->index > 9){
						printf("\nOPENED OVER 10 FILES.\n");
						machine->WriteRegister(2,-1);
						delete FileName;
						IncrementPC();
						return;
					}

					if(strcmp(FileName, "stdout") == 0){
// open file STDOUT
						//printf("\n OPEN SUCCEED FILE %s.",FileName);
						machine->WriteRegister(2,1);
						delete FileName;
						IncrementPC();
						return;
					}

					if(strcmp(FileName, "stdin") == 0){
// open file STDINT
						//printf("\n\n OPEN SUCCEED FILE %s.",FileName);
						machine->WriteRegister(2,0);
						delete FileName;
						IncrementPC();
						return;
					}
					
					if ((fileSystem->openfile[fileSystem->index] = fileSystem->Open(FileName,type)) == NULL){
						printf("\n %s NOT FOUND.\n",FileName);
						machine->WriteRegister(2,-1);
						delete FileName;
						IncrementPC();
						return;
					}else{
						printf("\n %s OPEN SUCCESSFULLY.\n", FileName);
						machine->WriteRegister(2,fileSystem->index-1);
						delete FileName;
						IncrementPC();
						return;
					}

				}
				case SC_Close:{
					int id = machine->ReadRegister(4);
					if (id>9 || id<0){
						printf("\nID FILE NOT VALID.\n");
						machine->WriteRegister(2,-1);
						IncrementPC();
						return;
					}else{
						if (fileSystem->openfile[id] != NULL){
							delete fileSystem->openfile[id];
							fileSystem->openfile[id] = NULL;
							printf("\n CLOSE ID FILE SUCCESSFULLY.\n");
							machine->WriteRegister(2,0);
							IncrementPC();
							return;
						}else{
							printf("\n ID FILE NOT EXIST.\n");
							machine->WriteRegister(2,-1);
							IncrementPC();
							return;
						}
					}
				}
				case SC_Read:{
					char *KenBuf;
					int size;
					int ByteRead;
					int DataBuf = machine->ReadRegister(4);
					int CharCount = machine->ReadRegister(5);
					int id = machine->ReadRegister(6);
					if (id > 9 || id < 0){
						printf("\n ID FILE OUT OF RANGE.\n");
						machine->WriteRegister(2,-1);
						IncrementPC();
						return;
					}
					
					if (id == 1){
//cant read file stdout	
						printf("\n CANT READ FILE STDOUT.\n");
						machine->WriteRegister(2,-1);
						IncrementPC();
						return;
					}
					if (fileSystem->openfile[id] == NULL){
						printf("\n FILE NOT ESIXT.\n");
						machine->WriteRegister(2,-1);
						IncrementPC();
						return;
					}
					KenBuf = User2System(DataBuf,CharCount);
					if (id == 0){
// read file stdin
						size = gSynchConsole->Read(KenBuf,CharCount);
						System2User(DataBuf,size,KenBuf);
						machine->WriteRegister(2,size);
						delete KenBuf;
						IncrementPC();
						return;
					}
// read file
					ByteRead = fileSystem->openfile[id]->Read(KenBuf,CharCount);
					if (ByteRead == 0){
						printf("\n AT THE END OF FILE.\n");
						machine->WriteRegister(2,-2);
						delete KenBuf;
						IncrementPC();
						return;
					}else{
						printf("\n BYTE READ: %d.\n",ByteRead);
////////////// Save Data of File to DataBuf
						System2User(DataBuf,ByteRead,KenBuf);
//////////////
						machine->WriteRegister(2,ByteRead);
						delete KenBuf;
						IncrementPC();
						return;
					}
				}
				case SC_Write:{
					char *KenBuf;
					int i_stdout;
					int ByteWrite;
					int DataBuf = machine->ReadRegister(4);
					int CharCount = machine->ReadRegister(5);
					int id = machine->ReadRegister(6);
					if (id > 9 || id < 0){
						printf("\n ID FILE OUT OF RANGE.\n");
						machine->WriteRegister(2,-1);
						IncrementPC();
						return;
					}
					
					if (id == 0){
//file stdin cant write to console	
						printf("\n CANT READ FILE STDOUT.\n");
						machine->WriteRegister(2,-1);
						IncrementPC();
						return;
					}

					if (fileSystem->openfile[id] == NULL){
						printf("\n FILE NOT ESIXT.\n");
						machine->WriteRegister(2,-1);
						IncrementPC();
						return;
					}

					if(fileSystem->openfile[id]->type == 1){
// cant write file only read
						printf("\n CANT WRITE FILE ONLY READ.\n");
						machine->WriteRegister(2,-1);
						IncrementPC();
						return;
					}
					KenBuf = User2System(DataBuf,CharCount);
// write file stdout
					if (id == 1){
						i_stdout = 0;
						while(KenBuf[i_stdout] != '\0' && KenBuf[i_stdout] != '\n'){
							gSynchConsole->Write(KenBuf+i_stdout,1);
							i_stdout++;
						}
						KenBuf[i_stdout] = '\n';
						machine->WriteRegister(2,i_stdout-1);
						delete KenBuf;
						IncrementPC();
						return;
					}
//write file
					ByteWrite = fileSystem->openfile[id]->Write(KenBuf,CharCount);
					printf("\n BYTE WRITE: %d.\n",ByteWrite);
					System2User(DataBuf,CharCount,KenBuf);
					machine->WriteRegister(2,ByteWrite);
					delete KenBuf;
					IncrementPC();
					return;
				}
///////////////////////
				case SC_Seek: 
				{
					int len;
					int pos = machine->ReadRegister(4);
					int openf_id = machine->ReadRegister(5);

					if (openf_id < 1 || openf_id > fileSystem->index || fileSystem->openfile[openf_id] == NULL)
					{
						printf("\n CAN NOT OPEN THIS FILE.\n");
						machine->WriteRegister(2, -1);
						IncrementPC();
						return;
					}

					len = fileSystem->openfile[openf_id]->Length(); // file size or length 

					if (pos > len)	// try to move file ptr to pos, pos > len --> wrong
					{
						printf("\nOUT OF RANGE.\n");
						machine->WriteRegister(2, -1);
						IncrementPC();
						return;
					}
// moving file ptr to the end of the file
					if (pos == -1) pos = len;

					fileSystem->openfile[openf_id]->Seek(pos);
					
					machine->WriteRegister(2, pos);// return value to the register
					printf("\n MOVE SUCCEED TO %d.\n",pos);
					IncrementPC();
					return;
				}
///////// test
				case SC_Delete:{
					int NameAddr = machine->ReadRegister(4);
					char* NameBuf = User2System(NameAddr,MaxFileLength);

					if(fileSystem->Runing(NameBuf)){
						printf("\n FILE %s IS OPENING.\n",NameBuf);
						machine->WriteRegister(2, -1);
						IncrementPC();
						return;
			
					}
					
					if(fileSystem->Remove(NameBuf)){
						printf("\n SUCCEED DELETE FILE %s.\n",NameBuf);
						machine->WriteRegister(2, 0);
					}else {
						printf("\n FILE %s NOT ESIXT.\n",NameBuf);
						machine->WriteRegister(2, -1);
					}
					delete NameBuf;
					IncrementPC();
					return;
				}
				default:
					printf("\nUnexpected user mode exception %d %d\n", which, type);
					interrupt->Halt();
			}
	}
}

/*
if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
    } else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    }
*/
