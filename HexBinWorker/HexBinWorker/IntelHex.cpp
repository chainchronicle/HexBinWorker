#include "StdAfx.h"
#include "IntelHex.h"
#include <string.h>
#include <regex> 

using namespace std;

const int FLASHVOLUME = 64; //Flash����������KΪ��λ 

bool IntelHex::open() {

	CT2A asciiFileName(fileName);
	pHexFile = fopen(asciiFileName, "r");

	if(pHexFile == NULL) {  
        printf("Open file error.\n");  
        return false;  
    }  
	return true;
}

bool IntelHex::checkLine(const char *src) {
	
	string checkPattern("[:0-9A-F\\r\\n]*");
	string lineString = src;

	std::regex_constants::syntax_option_type optionType = std::regex_constants::icase;
	std::regex regExpress(checkPattern, optionType);
	//std::smatch matchResult;

	if(std::regex_match(lineString, regExpress))  
    {  
        return matchLine(src);  
    }  
    else  
    {  
        return false;  
    }  
}

bool IntelHex::matchLine(const char *src) {
	string matchPattern("^:(\\w{2})(\\w{4})(\\w{2})(\\w*)(\\w{2})$");
	string lineString = src;
	std::regex_constants::syntax_option_type optionType = std::regex_constants::icase;
	std::regex regExpress(matchPattern, optionType);
	std::smatch matchResult;

	if(std::regex_match(lineString, matchResult, regExpress))  
    {  
		HexRecord hexRecord;
		HexLine hexLine;

		// ���ַ�����ֵ
		hexRecord.dataLength = string(matchResult[1].first, matchResult[1].second);
		hexRecord.startAddress = string(matchResult[2].first, matchResult[2].second);
		hexRecord.recordType = string(matchResult[3].first, matchResult[3].second);
		hexRecord.data = string(matchResult[4].first, matchResult[4].second);
		hexRecord.sumCheck = string(matchResult[5].first, matchResult[5].second);
		hexRecord.origRecord = lineString;

		// ������
		bool isPass = verifyLine(hexRecord);
		if (isPass) {

			hexLine.hexRecord = hexRecord;
			fileContent.push_back(hexLine);
		}
        return true;  
    }  
    else  
    {  
        return false;  
    }  

}

bool IntelHex::verifyLine(const HexRecord& hexRecord) {
	// ��֤���ݳ���
	const unsigned int dateSize = hexRecord.data.size();
	unsigned int dataLength = hexToDec(hexRecord.dataLength);

	if (dateSize != dataLength * 2) {
		return false;
	}
	
	// У���
	const int origRecordSize = hexRecord.origRecord.size();
	unsigned int sumCheck = 0;
	string origRecordCopy = hexRecord.origRecord;
	typedef string::const_iterator cIter;
	for (cIter i = origRecordCopy.begin()+1 ; i < origRecordCopy.end() - 2; i += 2) {
		string hexStr = string(i, i+2);
		sumCheck += hexToDec(hexStr);
		sumCheck &= 0xFF;
	}
	sumCheck = (~sumCheck + 1) & 0xFF;  // 206 -> CE

	string hexSumCheckStr = "0x" + hexRecord.sumCheck;
	unsigned int decSumCheck = strtol(hexSumCheckStr.c_str(), NULL, 16);
	
	if (sumCheck != decSumCheck) {
		return false;
	}

	return true;
}

void IntelHex::hexStringToDec() {
	// �ַ��� -> 16���Ʊ�ʾ
	//const int fileLines = fileContent.size();
	//for (int i=0; i<fileLines; i++) {
	//	//typedef string::iterator Iter;
	//	//string dataCopy = fileLines.hexR
	//	//for (Iter i = fileLines..begin()+1 ; i < origRecordCopy.end() - 2; i += 2) {
	//	//string hexStr = string(i, i+2);

	//	//HexRecord.datas.push_back();

	//	//string hexData = "0x" + fileContent[i].hexRecord.data;
	//	//long n = strtol(data.c_str(), NULL, 16);
	//}
}

unsigned int IntelHex::hexToDec(const string& str) {

	unsigned int result = 0;
	
	string strCopy = str;
	transform(strCopy.begin(), strCopy.end(), strCopy.begin(), ::toupper);
	const int strSize = strCopy.size();

	for (int i = 0; i < strSize; i++) {
		if (strCopy[i] >= '0' && strCopy[i] <= '9') {
			result = result * 16 + (unsigned int)(strCopy[i] - '0');
		} else if (str[i] >= 'A' && str[i] <= 'F') {
			result = result * 16 + (unsigned int)(strCopy[i] - 'A' + 10); 
		} else {
			result = result * 16 + (unsigned int)(strCopy[i] - 'A' + 10); 
		}
	}

    return result;
}

bool IntelHex::formatParse(const char *src, const int lineNo) {
	//����ǰ��ʽ   :llaaaattddcc               :020000000828CE
	//�������ʽ   : ll aaaa tt [dd] cc        : 02 0000 00 [0828] CE
	//����ֱ���� 

	const int srcLength = strlen(src);
	int srcIndex = 0, dstIndex = 0; //������pΪsrc��������qΪdst����  
    //int index = 0;//ѭ����־ 

	HexRecord hexRecord;

	while (srcIndex < srcLength) {  

		if (srcIndex == 0) {
			if (src[srcIndex] != ':') {
				printf("Wrong Format.\n"); // TODO:
				break;
			}
			srcIndex++;
		}

		if(srcIndex == 1) {   //��ȡ��λ��"ll"
            for(int index = 0; index < 2; index++) { 	
				hexRecord.dataLength += src[srcIndex++];
            }  
        }

		if(srcIndex == 3) {  //��ȡ��λ��"aaaa"
            for(int index = 0; index < 4; index++) {  
				hexRecord.startAddress += src[srcIndex++];
            }
        }

		if(srcIndex == 7) {  //��ȡ��λ��"tt"
            for(int index = 0; index < 2; index++) {  
				hexRecord.recordType += src[srcIndex++];
            }  
        }

		if(srcIndex == 9) { //��ȡ2Nλ��"[dd]"
			const int dataLen = srcLength - 2 - srcIndex;
			for(int index = 0; index < dataLen; index++) { 
				hexRecord.data += src[srcIndex++];
			}
		}

		if (srcIndex == srcLength-2) { //��ȡ��λ��"cc"
			for(int index = 0; index < 2; index++) {  
				hexRecord.sumCheck += src[srcIndex++];
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
		checkLine(lineBuffer);
		//formatParse(lineBuffer, lineNo);
		lineNo++;
	}

	hexStringToDec();

	delete(lineBuffer);
	delete(parseBuffer);
	delete(flashBuffer);

}