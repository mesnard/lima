/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/

// NAUTITIA
//
// jys 2-OCT-2002
//
// Binary memory explorer. 
// BinaryEntry is the base class of other dictionnary structure
// management class (DictionaryEntry, LingInfoEntry, etc.)
// BinaryEntry has methods to explore dictionary in memory.
// Dictionary memory strcture is described in NAU2002.r2128 JYS

#include "BinaryEntry.h"

#include "common/misc/Exceptions.h"
#include <iostream>

using namespace std;
using namespace Lima::Common::Misc;

namespace Lima {
namespace LinguisticProcessing {
namespace Dictionary {


BinaryEntry::BinaryEntry() :
        _dummyString()
{
}

BinaryEntry::BinaryEntry(const BinaryEntry&) :
        _dummyString()
{
    // nothing to copy
}

BinaryEntry::~BinaryEntry()
{
}

BinaryEntry& BinaryEntry::operator=(const BinaryEntry&)
{
    // nothing to copy
    return *this;
}

// gets the pointed encoded number (presented in big endian)
// advances pointer to point after the number
uint64_t BinaryEntry::getEncodedNumber(unsigned char*& ptr) const
{
//    cout<<"BinaryEntry::getEncodedNumber() "<<hex<<(int)*ptr<<" "<<(int)*(ptr+1)<<" "<<(int)*(ptr+2)<<endl;
    unsigned char firstByte = *ptr++;
    uint64_t result = firstByte;
    if (firstByte >= 0x80) {
        result = (result-0x80)*0x100 + *ptr++;
        if (firstByte >= 0xC0) {
            result = (result-0x4000)*0x100 + *ptr++;
            if (firstByte >= 0xE0) {
                result = (result-0x200000)*0x100 + *ptr++;
                if (firstByte >= 0xF0)
                    throw InvNumberException();
            }
        }
    }
    return result;
}

// gets the pointed encoded number (presented in big endian)
// pointer is not moved
uint64_t BinaryEntry::getEncodedNumberNoMove(unsigned char* ptr) const {
    unsigned char *ptr2 = ptr;
    return getEncodedNumber(ptr2);
}

// gets the next number of specified number of bytes (presented in big endian)
// advances pointer to point after the number
uint64_t BinaryEntry::getNumber(unsigned char*& ptr,
            const uint64_t count) const
 {
    if ((count > 4) || (count < 1)) throw InvNumberException();
    uint64_t result = *ptr++;
    for (uint64_t loop=1; loop<count; loop++) 
  result = result*0x100 + *ptr++;
    return result;
}

// gets the next number of specified number of bytes (presented in big endian)
// pointer is not moved
uint64_t BinaryEntry::getNumberNoMove(unsigned char* ptr,
            const uint64_t count) const
{
    unsigned char *ptr2 = ptr;
    return getNumber(ptr2, count);
}

// gets the pointed dictionary string presented in big endian as a LimaString
// advances pointer to point after the string
LimaString BinaryEntry::getString(unsigned char*& ptr) const {
    LimaString foundString;
    uint64_t count = getEncodedNumber(ptr);
    uint64_t testLittleEndian = 13;        // if platform in little endian, reverse
    if (testLittleEndian == (*(unsigned char*)&testLittleEndian))
        foundString = setStringReverseEndian((unsigned short*)ptr, count/2);
    else
        foundString = setStringSameEndian((unsigned short*)ptr, count/2);
    ptr = ptr+count;
    return foundString;
}

// gets the pointed dictionary string as a LimaString
// pointer is not moved
LimaString BinaryEntry::getStringNoMove(unsigned char* ptr) const
{
    unsigned char *ptr2 = ptr;
    return getString(ptr2);
}

// gets the pointed dictionary string presented in UTF8 as a LimaString
// advances pointer to point after the string
LimaString BinaryEntry::getUtf8String(unsigned char*& ptr) const {
    LimaString foundString;
    uint64_t maxSize = getEncodedNumber(ptr);
    unsigned char *ptrEnd = ptr + maxSize;
    LimaString utf16String; utf16String.resize(maxSize);
    int j=0;
    while (ptr < ptrEnd) {
        LimaChar first = *ptr++ & 0xFF;
        if (first < 0x80)                   // 7 bits available
            utf16String[j++] = first;      
        else if (first < 0xDF)                // 11 bits available
            utf16String[j++] = LimaChar(((first.unicode()<<6)+(*ptr++&0x3F))&0x000007FF);
        else if (first < 0xEF)                // 16 bits available
            utf16String[j++] = LimaChar(((first.unicode()<<12)+((*ptr++&0x3F)<<6)+(*ptr++&0x3F))&0x0000FFFF);
        else
            throw BoundsErrorException();
    }
    ptr = ptrEnd;
// #ifdef WIN32
//     LimaString result;
//     result.resize(j);
//     for (uint64_t i = 0; i < j; i++)
//     {
//       result[i] = LimaChar(utf16String[i]);
//     }
//     return result;
// #else
    return utf16String.left(j);
// #endif
}

// gets the pointed dictionary string as a LimaString
// pointer is not moved
LimaString BinaryEntry::getUtf8StringNoMove(unsigned char* ptr) const
{
    unsigned char *ptr2 = ptr;
    return getUtf8String(ptr2);
}

// advances pointer to the next dictionary field
unsigned char* BinaryEntry::nextField(unsigned char*& ptr) const
{
  uint64_t num = getEncodedNumber(ptr);
    ptr += num;
//    ptr = getEncodedNumber(ptr) + ptr;
    return ptr;
}

} // closing namespace Dictionary
} // closing namespace LinguisticProcessing
} // closing namespace Lima
