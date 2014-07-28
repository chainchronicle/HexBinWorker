/*******************************************************************************
* IntelHex - class definitions                                                 *
*                                                                              *
* A class to handle the encoding and decoding of an Intel HEX format file as   *
* generated by many tool chains for embedded processors and microcontrollers.  *
*                                                                              *
********************************************************************************/

#pragma once
#include <iostream>
#include <map>
#include <list>
using namespace std;

class IntelHex
{
	friend ostream& operator<< (ostream& dataOut, IntelHex& ihLocal);
	friend istream& operator>> (istream& dataIn, IntelHex& ihLocal);

private:
// holding the addresses found in the Intel HEX file and the associated data byte stored at that address 
	map<unsigned long, unsigned char> ihContent; 
	map<unsigned long, unsigned char>::iterator ihIterator;

	unsigned long segmentBaseAddress;

	struct {
            unsigned short  csRegister;
            unsigned short  ipRegister;
            bool            exists;
    } startSegmentAddress;

	struct {
            unsigned long   eipRegister;
            bool            exists;
    } startLinearAddress;

	struct {
            list<string> ihWarnings;
            unsigned long noOfWarnings;
    } msgWarning;

	struct {
            list<string> ihErrors;
            unsigned long noOfErrors;
    } msgError;

	bool foundEof;
	bool verbose;
	bool segmentAddressMode;

	unsigned char stringToHex(string value);
	string ulToHexString(unsigned long value);
	string ucToHexString(unsigned char value);
	string ulToString(unsigned long value);

	void decodeDataRecord(unsigned char recordLength,
                              unsigned long loadOffset, 
                              string::const_iterator data);

	void addWarning(string warningMessage);
	void addError(string errorMessage);

public:
	IntelHex(void) {
		/* Initialise the segment base address to zero                    */
            segmentBaseAddress = 0;
            /* Clear content of register variables used with the 'Start Segment'
            *  and 'Start Linear' address records                             */
            startSegmentAddress.ipRegister = 0;
            startSegmentAddress.csRegister = 0;
            startSegmentAddress.exists = false;
            startLinearAddress.eipRegister = 0;
            startLinearAddress.exists = false;
            /* Set up error and warning handling variables                    */
            msgWarning.noOfWarnings = 0;
            msgError.noOfErrors = 0;
            /* Note that the EOF record has not been found yet                */
            foundEof = false;
            /* Set verbose mode to off                                        */
            verbose = false;
            /* Set segment address mode to false (default)                    */
            segmentAddressMode = false;
            /* Ensure ihContent is cleared and point ihIterator at it         */
            ihContent.clear();
            ihContent.begin();
            ihIterator = ihContent.begin();
	}


	~IntelHex(void) { }


	IntelHex(const IntelHex &ihSource)
        {
            /* Initialise the segment base address                            */
            segmentBaseAddress = ihSource.segmentBaseAddress;
            /* Initialise content of register variables used with the 'Start Segment'
            *  and 'Start Linear' address records                             */
            startSegmentAddress.ipRegister = ihSource.startSegmentAddress.ipRegister;
            startSegmentAddress.csRegister = ihSource.startSegmentAddress.csRegister;
            startSegmentAddress.exists = ihSource.startSegmentAddress.exists;
            startLinearAddress.eipRegister = ihSource.startLinearAddress.eipRegister;
            startLinearAddress.exists = ihSource.startLinearAddress.exists;
            /* Set up error and warning handling variables                    */
            msgWarning.noOfWarnings = ihSource.msgWarning.noOfWarnings;
            msgWarning.ihWarnings = ihSource.msgWarning.ihWarnings;
            msgError.noOfErrors = ihSource.msgError.noOfErrors;
            msgError.ihErrors = ihSource.msgError.ihErrors;
            /* Note that the EOF record has not been found yet                */
            foundEof = ihSource.foundEof;
            /* Set verbose mode to off                                        */
            verbose = ihSource.verbose;
            /* Set segment address mode to false (default)                    */
            segmentAddressMode = ihSource.segmentAddressMode;
            /* Copy HEX file content variables                                */
            ihContent = ihSource.ihContent;
            ihIterator = ihSource.ihIterator;
        }

	IntelHex& operator= (const IntelHex &ihSource) {
            /* Check that we are not trying to assign ourself to ourself      */
            /* i.e. are the source/destination addresses the same like        */
            /* myData = myData;                                               */
            if (this == &ihSource)
                return *this;
            
            /* Initialise the segment base address                            */
            segmentBaseAddress = ihSource.segmentBaseAddress;
            /* Initialise content of register variables used with the 'Start Segment'
            *  and 'Start Linear' address records                             */
            startSegmentAddress.ipRegister = ihSource.startSegmentAddress.ipRegister;
            startSegmentAddress.csRegister = ihSource.startSegmentAddress.csRegister;
            startSegmentAddress.exists = ihSource.startSegmentAddress.exists;
            startLinearAddress.eipRegister = ihSource.startLinearAddress.eipRegister;
            startLinearAddress.exists = ihSource.startLinearAddress.exists;
            /* Set up error and warning handling variables                    */
            msgWarning.noOfWarnings = ihSource.msgWarning.noOfWarnings;
            msgWarning.ihWarnings = ihSource.msgWarning.ihWarnings;
            msgError.noOfErrors = ihSource.msgError.noOfErrors;
            msgError.ihErrors = ihSource.msgError.ihErrors;
            /* Note that the EOF record has not been found yet                */
            foundEof = ihSource.foundEof;
            /* Set verbose mode to off                                        */
            verbose = ihSource.verbose;
            /* Set segment address mode to false (default)                    */
            segmentAddressMode = ihSource.segmentAddressMode;
            /* Copy HEX file content variables                                */
            ihContent = ihSource.ihContent;
            ihIterator = ihSource.ihIterator;
            
            return *this;
        }

	 IntelHex& operator++() {
            ++ihIterator;
            
            return(*this);
        }


	  const IntelHex operator++ (int)
        {
            IntelHex tmp(*this);
            ++(*this);
            return(tmp);
        }

	  IntelHex& operator--()
        {
            --ihIterator;
            
            return(*this);
        }

	  const IntelHex operator--(int)
        {
            IntelHex tmp(*this);
            --(*this);
            return(tmp);
        }

	  void begin()
        {
            if (ihContent.size() != 0)
            {
                ihIterator = ihContent.begin();
            }
        }

	  void end()
        {
            if (!ihContent.empty())
            {
                ihIterator = ihContent.end();
                --ihIterator;
            }
        }

	   unsigned long size()
		{
			return static_cast<unsigned long>(ihContent.size());
		}

	   bool endOfData()
        {
            /* Return true if there is no data anyway                         */
            bool result = true;
            
            if (!ihContent.empty())
            {
				map<unsigned long, unsigned char>::iterator it \
					                                         = ihContent.end();

				--it;

                if (it != ihIterator)
                {
                    result = false;
                }
            }
            return result;
        }
        
        bool empty()
        {
            return ihContent.empty();
        }

		bool jumpTo(unsigned long address)
        {
            bool result = false;
            
            if (ihContent.size() != 0)
            {
                map<unsigned long, unsigned char>::iterator it;
                it = ihContent.find(address);
                if (it != ihContent.end())
                {
                    result = true;
                    ihIterator = it;
                }
            }
            return result;
        }

		bool incrementAddress()
        {
            bool result = false;

            /* If we have data */            
            if (ihContent.size() != 0)
            {
                /* If we're not already pointing to the end */
                if (ihIterator != ihContent.end())
                {
                    /* Increment iterator */
                    ihIterator++;
                    
                    /* If we still haven't reached the end... */
                    if (ihIterator != ihContent.end())
                    {
                        /* Everything is ok! */
                        result = true;
                    }
                }
            }

            /* If incrementation of the iterator was successful, return true  */
            return result;
        }

		bool decrementAddress()
        {
            bool result = false;

            /* If we have data */            
            if (ihContent.size() != 0)
            {
                /* If we're not already pointing to the start */
                if (ihIterator != ihContent.begin())
                {
                    /* Decrement iterator */
                    ihIterator--;
                    
                    /* Everything is ok! */
                    result = true;
                }
            }

            /* If incrementation of the iterator was successful, return true  */
            return result;
        }

		unsigned long currentAddress()
        {
            return ihIterator->first;
        }

		bool startAddress(unsigned long * address)
        {
            if (ihContent.size() != 0)
            {
                map<unsigned long, unsigned char>::iterator it;
            
                it = ihContent.begin();
                *address = (*it).first;
                return true;
            }
            
            return false;
        }

		 bool endAddress(unsigned long * address)
        {
            if (ihContent.size() != 0)
            {
                map<unsigned long, unsigned char>::reverse_iterator rit;
            
                rit = ihContent.rbegin();
                *address = (*rit).first;
                return true;
            }
            
            return false;
        }

		 bool getData(unsigned char * data)
        {
            if (!ihContent.empty() && (ihIterator != ihContent.end()))
            {
                *data = ihIterator->second;
                return true;
            }
            return false;
        }

		 bool getData(unsigned char * data, unsigned long address)
		{
			bool found = false;
			map<unsigned long, unsigned char>::iterator localIterator;

            if (!ihContent.empty())
            {
				localIterator = ihContent.find(address);

				if (localIterator != ihContent.end())
				{
					found = true;
					ihIterator = localIterator;
					*data = ihIterator->second;
				}
            }

            return found;
        }

		 bool insertData(unsigned char data);
        bool insertData(unsigned char data, unsigned long address);
        
        void overwriteData(unsigned char data);
        void overwriteData(unsigned char data, unsigned long address);
        
        bool blankFill(unsigned char data);
        
        bool blankFill(unsigned char * const data, unsigned long sizeOfData);
        
        void blankFill(unsigned char * const data, unsigned long sizeOfData,
                       unsigned long endAddress);
        
        bool blankFillRandom();
        
        void blankFillRandom(unsigned long endAddress);
        
        bool blankFillAddressLowByte();
        
        void blankFillAddressLowByte(unsigned long endAddress);

		unsigned long getNoWarnings()
        {
            return msgWarning.noOfWarnings;
        }

		unsigned long getNoErrors()
        {
            return msgError.noOfErrors;
        }

		bool popNextWarning(string& warning)
        {
            if (msgWarning.noOfWarnings > 0)
            {
                warning = msgWarning.ihWarnings.front();
                
                msgWarning.ihWarnings.pop_front();
            
                msgWarning.noOfWarnings = msgWarning.ihWarnings.size();
                
                return true;
            }
            else
            {
                return false;
            }
        }

		bool popNextError(string& error)
        {
            if (msgError.noOfErrors > 0)
            {
                error = msgError.ihErrors.front();
                
                msgError.ihErrors.pop_front();
            
                msgError.noOfErrors = msgError.ihErrors.size();
                
                return true;
            }
            else
            {
                return false;
            }
        }

		bool getStartSegmentAddress(unsigned short * ipRegister, 
                                    unsigned short * csRegister)
        {
            if (startSegmentAddress.exists == true)
            {
                *ipRegister = startSegmentAddress.ipRegister;
                *csRegister = startSegmentAddress.csRegister;
            }
            
            return startSegmentAddress.exists;
        }

		bool getStartLinearAddress(unsigned long * eipRegister)
        {
            if (startLinearAddress.exists == true)
            {
                *eipRegister = startLinearAddress.eipRegister;
            }
            
            return startLinearAddress.exists;
        }

		 void setStartSegmentAddress(unsigned short ipRegister, 
                                    unsigned short csRegister)
        {
            startSegmentAddress.ipRegister = ipRegister;
            startSegmentAddress.csRegister = csRegister;
            startSegmentAddress.exists = true;
        }

		 void setStartLinearAddress(unsigned long eipRegister)
        {
            startLinearAddress.eipRegister = eipRegister;
            startLinearAddress.exists = true;
        }

		 void segmentAddressingOn()
        {
            segmentAddressMode = true;
        }

		  void linearAddressingOn()
        {
            segmentAddressMode = false;
        }

		  void verboseOn()
        {
            verbose = true;
        }

		  void verboseOff()
        {
            verbose = false;
        }


};
