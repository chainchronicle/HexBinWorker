#include "StdAfx.h"
#include "IntelHex.h"
#include <string>
#include <regex> 

using namespace std;

/*
�򿪲���ȡ�ļ�
���У�����У��
���� match hex ����
У�����ݳ��ȼ�У���
	ͨ�������� recordType������ 04 ���;�ֱ�� new һ���� block
		  ʵʱ�������ݵ���Ч����
		  �Ƿ�Ҫ��¼ÿ����¼�ĳ��ȣ���
	���˳��� hex ת�� bin д��
���ϣ�Hex -> Bin 
- - -
��֮�� bin -> hex 
ֱ����伴�ɣ����ȹ̶� 8 bit * 16 block
��ʼ��ַ 0x00
8bit ������ת16����

*/



bool IntelHex::openHexFile(CString& hexFileName) {

	CT2A asciiFileName(hexFileName);
	//_pHexFileHandler = fopen(asciiFileName, "r");
	fopen_s(&_pHexFileHandler, asciiFileName, "r");

	if(!_pHexFileHandler) {  
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

	if(std::regex_match(lineString, regExpress)) {  
        return true;  
    } else {  
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
		hexRecord.dataLength = string(matchResult[1].first, matchResult[1].second);
		hexRecord.startAddress = string(matchResult[2].first, matchResult[2].second);
		hexRecord.recordType = string(matchResult[3].first, matchResult[3].second);
		hexRecord.data = string(matchResult[4].first, matchResult[4].second);
		hexRecord.sumCheck = string(matchResult[5].first, matchResult[5].second);
		hexRecord.origRecord = lineString;

		// ������
		bool isPass = verifyLine(hexRecord);
		if (isPass) {
			
			unsigned int recordTypeInt = hexToDec(hexRecord.recordType);

			switch (recordTypeInt)
			{
			case 0: {
				if (_hexBlocks.empty()) {
					HexBlock newHexBlock = HexBlock();
					_hexBlocks.push_front(newHexBlock);
				}

				list<HexBlock>::iterator hexBlocksIterator = _hexBlocks.begin();
				const int startAddr = hexToDec(hexRecord.startAddress);
				const int dataLength = hexToDec(hexRecord.dataLength);

				vector<BYTE> splitedData;
				splitHexData(hexRecord.data, splitedData);

				for (int i = startAddr; i < startAddr + dataLength; i++) {
					hexBlocksIterator->datas[i] = splitedData[i-startAddr];
				}

				hexBlocksIterator->validLength += dataLength;
				
				break;
			}
			case 1: { //end of file
				return true;
				break;
			}
			case 2: {
				break;
			}
			case 4: {
				HexBlock newHexBlock = HexBlock();

				// parse type 04 here


				_hexBlocks.push_front(newHexBlock);
				break;
			}
			case 5: {
				break;
			}

			}
		} else {  
		
			printf("Hex Recoad IS NOT avaliable format");
			return false;  
		}  
		return true;
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
void IntelHex::hexStringToByte(const char* src, const int srcLen, BYTE* dst) {
    
    for (int i = 0; i < srcLen; i += 2)
    {
        BYTE highByte = toupper(src[i]);
        BYTE lowByte  = toupper(src[i+1]);

        if (highByte > 0x39) {
            highByte -= 0x37;
		} else {
            highByte -= 0x30;
		}

        if (lowByte > 0x39) {
            lowByte -= 0x37;
		} else {
            lowByte -= 0x30;
		}

        dst[i/2] = (highByte << 4) | lowByte;
    }
}
void IntelHex::byteToHexString(BYTE* source, char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;


    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;


        highByte += 0x30;


        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;


        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    return ;
}
void IntelHex::splitHexData(const string& inData, vector<BYTE>& outData) {

	typedef string::const_iterator cIter;
	for (cIter i = inData.begin() ; i < inData.end(); i += 2) {

		const string hexStr = string(i, i+2);
		BYTE decByte = (BYTE)strtol(hexStr.c_str(), NULL, 16);  // hex -> dec
		outData.push_back(decByte);
	}
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
bool IntelHex::hexFormatParse(const char *src, char *dst) {
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
void IntelHex::byteToBin(BYTE *pByte, char* pBin){

	int n = (int)*pByte;

	for (int i=0; i<8; i++) {
		int p = (int)(pow(2.0, i));
		if (n&p) {
			pBin[7-i] = '1';
		} else {
			pBin[7-1] = '0';
		}
	}
}
void IntelHex::writeToBinFile(FILE* fileHandler) { 
	
	if (fileHandler == NULL) {  
        printf("Open file error.\n");  
        return;  
    }

	 //��ʼд��  
	typedef list<HexBlock>::reverse_iterator ListRevIter;
	for(ListRevIter rIter = _hexBlocks.rbegin(); rIter != _hexBlocks.rend(); rIter++) {
		int validLength = rIter->validLength;
		fwrite(rIter->datas, 1, validLength, fileHandler);
	}

	fclose(fileHandler);
	fileHandler = NULL;
}


// - Interface


void IntelHex::parse() {

    char *lineBuffer = new char[sizeof(char) * 100];        //�洢hex�ļ���һ������  


    if (lineBuffer == NULL ) {  
		printf("Apply for memory failed.!\n");  
        return;  
    }  

	if (!openHexFile(_fileName)) return;

	while (fscanf(_pHexFileHandler, "%s", lineBuffer) != EOF) {
		printf("%s\n", lineBuffer);
		bool checkPass = checkLine(lineBuffer);
		if (checkPass) {
			bool isVerify = matchLine(lineBuffer);
			if (!isVerify)
			{
				delete [] lineBuffer;
				return;
			}
			_hexEditField += lineBuffer;
			_hexEditField += "\r\n";
			
		}
	}

	delete [] lineBuffer;
}


string IntelHex::getEditFieldText() {
	return _hexEditField;
}


//string IntelHex::getBinEditFieldText() {
//
//	const int block = 16;
//
//	typedef list<HexBlock>::reverse_iterator ListRevIter;
//	for(ListRevIter rIter = hexBlocks.rbegin(); rIter != hexBlocks.rend(); rIter++) {
//		
//		char ch[3];
//		int validLength = rIter->validLength;
//
//		for (int i = 0; i < validLength; i++) {
//			BYTE b = rIter->datas[i];
//			sprintf_s(ch, 3, "%02X", b);
//			_binEditField += ch;
//			_binEditField += " ";
//
//			if (i % block == block-1) {
//				_binEditField += "\r\n";
//			}
//		}
//	}
//
//	return _binEditField;
//}


string IntelHex::getFilePath() {
	string filePathStr = CT2A(_fileName);
	return filePathStr;
}