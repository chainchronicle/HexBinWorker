#include "StdAfx.h"
#include "IntelHex.h"
#include <string.h>


using namespace std;

const int FLASHVOLUME = 16; //Flash����������KΪ��λ 

bool IntelHex::open() {

	pHexFile = fopen((LPSTR)(LPCTSTR)fileName, "r");  // TODO: ?
	if(pHexFile == NULL) {  
        printf("Open file error.\n");  
        return false;  
    }  
	return true;
}


void IntelHex::parse() {
	const int  flashSize = 2 * FLASHVOLUME * 1024 * sizeof(char); //flash�Ĵ洢��Ԫ����  
    char *lineBuffer = new char[sizeof(char) * 100];        //�洢hex�ļ���һ������  
    char *parseBuffer = new char[sizeof(char) * 200];       //�洢hex�ļ������������  
    char *flashBuffer = new char[flashSize];                //�洢Flash�е����� 

    if (lineBuffer == NULL|| parseBuffer == NULL || flashBuffer == NULL) {  
		printf("Apply for memory failed.!\n");  
        return;  
    }  

	memset(flashBuffer, 'F', flashSize); //��Flash��ʼʱȫ����1

	if (!open()) return;
	
	while (fscanf(pHexFile, "%s", lineBuffer) != EOF) {
		printf("%s\n", lineBuffer);

	}

	delete(lineBuffer);
	delete(parseBuffer);
	delete(flashBuffer);

}