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
/************************************************************************
 *
 * @file       bowBinaryReaderWriter.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Mar 20 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * Project     Common
 * 
 * @brief      reader writer of bow in binary format
 * 
 * 
 ***********************************************************************/

#ifndef BOWBINARYREADERWRITER_H
#define BOWBINARYREADERWRITER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <iostream>
#include <vector>
#include <map>

namespace Lima {
namespace Common {
namespace BagOfWords {

class AbstractBoWDocumentHandler;
class BoWDocument;
class BoWText;
class BoWToken;
class BoWRelation;
class BoWNamedEntity;
// reader/writer are kept independent from BoW (instead of having
// read/write functions in each BoW element) because it is easier
// to handle common mappings (pointers, entity types etc).

#define BOW_VERSION "0.8"

typedef enum {
  BOWFILE_NOTYPE,
  BOWFILE_TEXT,
  BOWFILE_DOCUMENT,
  BOWFILE_DOCUMENTST,
  BOWFILE_SDOCUMENT
} BoWFileType;

// reader and writer are not symmetrical : writer simply write infos;
// reader must handle mappings 
class BoWBinaryReaderPrivate;
class LIMA_BOW_EXPORT BoWBinaryReader
{
 public:
  BoWBinaryReader(); 
  virtual ~BoWBinaryReader();
  
  void readHeader(std::istream& file);
  void readBoWText(std::istream& file,
                   BoWText& bowText);
  void readBoWDocumentBlock(std::istream& file,
                       BoWDocument& document,
                       AbstractBoWDocumentHandler& handler, 
                       bool useIterator=false);
  BoWToken* readBoWToken(std::istream& file,
                         std::vector<BoWToken*>& refMap);
  void readSimpleToken(std::istream& file,
                       BoWToken* token);

  BoWFileType getFileType() const;
  std::string getFileTypeString() const;

private:
  BoWBinaryReader(const BoWBinaryReader&);
  BoWBinaryReader& operator=(const BoWBinaryReader&);
  BoWBinaryReaderPrivate* m_d;
};

class BoWBinaryWriterPrivate;
class LIMA_BOW_EXPORT BoWBinaryWriter
{
 public:
  BoWBinaryWriter(); 
  virtual ~BoWBinaryWriter();
  
  void writeHeader(std::ostream& file, 
                   BoWFileType type) const;
  void writeBoWText(std::ostream& file,
                    const BoWText& bowText) const;
  void writeBoWDocument(std::ostream& file,
                        const BoWDocument& bowText) const;
  void writeBoWToken(std::ostream& file,
                     const BoWToken* bowToken,
                     std::map<BoWToken*,uint64_t>& refMap) const;
  void writeSimpleToken(std::ostream& file,
                        const BoWToken* token) const;

 private:
  BoWBinaryWriter(const BoWBinaryWriter&);
  BoWBinaryWriter& operator=(const BoWBinaryWriter&);
  BoWBinaryWriterPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
