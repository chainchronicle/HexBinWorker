#include "StdAfx.h"
#include "IntelHex.h"
#include <string.h>


using namespace std;

const int FLASHVOLUME = 16; //Flash����������KΪ��λ 

bool IntelHex::open() {

	CT2A asciiFileName(fileName);
	pHexFile = fopen(asciiFileName, "r");

	if(pHexFile == NULL) {  
        printf("Open file error.\n");  
        return false;  
    }  
	return true;
}

bool IntelHex::formatParse(const char *src, const int lineNo) {
	//����ǰ��ʽ   :llaaaattddcc               :020000000828CE
	//�������ʽ   : ll aaaa tt [dd] cc        : 02 0000 00 [0828] CE
	//����ֱ���� 

	const int srcLength = strlen(src);
	int srcIndex = 0, dstIndex = 0; //������pΪsrc��������qΪdst����  
    //int index = 0;//ѭ����־ 

	HexRecord hexRecord;

	while (srcIndex <= srcLength) {  

		if (srcIndex == 0) {
			if (src[srcIndex] != ':') {
				printf("Wrong Format.\n"); // TODO:
				break;
			}
			srcIndex++;
		}

		if(srcIndex == 1) {   //��ȡ��λ��"ll"
			//hexRecord.dataLength = new char[3];

            for(int index = 0; index < 2; index++) { 	
				hexRecord.dataLength += src[srcIndex++];
            }  
			//hexRecord.dataLength[2] = '\0';
        }

		if(srcIndex == 3) {  //��ȡ��λ��"aaaa"
            for(int index = 0; index < 4; index++) {  
        
				hexRecord.startAddress[index] = src[srcIndex++];
            }
        }

		if(srcIndex == 7) {  //��ȡ��λ��"tt"
            for(int index = 0; index < 2; index++) {  
                
				hexRecord.recordType[index] = src[srcIndex++];
            }  
        }

		if(srcIndex == 9) { //��ȡ2Nλ��"[dd]"
			for(int index = 0; index < srcLength - 2 - srcIndex; index++) { 
				hexRecord.data[index] = src[srcIndex++];
			}
		}

		if (srcIndex == srcLength-1) { //��ȡ��λ��"cc"
			for(int index = 0; index < 2; index++) {  
                
				hexRecord.sumCheck[index] = src[srcIndex++];
            }  
		}

	}
	return true;
}

bool hexFormatParse(const char *src, char *dst) {
	//����ǰ��ʽ   :llaaaattddcc               :020000000828CE
	//�������ʽ   : ll aaaa tt [dd] cc        : 02 0000 00 [0828] CE
	int srcLength = strlen(src);
	int srcIndex = 0, dstIndex = 0; //������pΪsrc��������qΪdst����  
    int index = 0;//ѭ����־ 

	while (srcIndex <= srcLength) { //ѭ������src����ʽ���� 
		if (srcIndex == 0) {
			if (src[srcIndex] != ':') {
				printf("Wrong Format.\n");
				break;
			} else {  
                dst[dstIndex++] = ':'; 
				dst[dstIndex++] = ' ';  
                srcIndex++;
				continue;  
            }  
		}

		if(srcIndex == 1) {  
            for(index = 0; index < 2; index++) {  //��ȡ��λ��"ll"  
                dst[dstIndex++] = src[srcIndex++];  
            }  
            dst[dstIndex++] = ' ';
			continue;  
        }  

	}

	return false;
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
	
	int lineNo = 1;
	while (fscanf(pHexFile, "%s", lineBuffer) != EOF) {
		printf("%s\n", lineBuffer);
		formatParse(lineBuffer, lineNo);
		lineNo++;
	}

	delete(lineBuffer);
	delete(parseBuffer);
	delete(flashBuffer);

}