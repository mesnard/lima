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
 * @file       readBoWFile.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Thu Oct  9 2003
 * @version    $Id: convertBow06to05.cpp 1303 2005-02-28 18:02:34Z mathieub $
 * copyright   Copyright (C) 2003 by CEA LIST
 *
 ***********************************************************************/

#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowDocumentST.h"
#include "linguisticProcessing/common/BagOfWords/bowFileHeader.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"
#include "common/Data/strwstrtools.h"
#include <fstream>

using namespace std;
using namespace Lima;
using namespace Lima::Common::BagOfWords;

//****************************************************************************
// declarations
//****************************************************************************
// help mode & usage
static const string USAGE("usage : convertBow06to05 fileIn fileOut\n");

static const string HELP("convert a binary bowdocument from version 0.6 to version 0.5");

//****************************************************************************
// GLOBAL variable -> the command line arguments
struct
{
  string inputFile;           // input file
  string outputFile;
  bool help;                  // help mode
}
param={"","",false};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  if (argc>3)
  {
    cerr << "invalid number of argument !" << endl;
    exit(-1);
  }

  string s(argv[1]);
  if (s=="-h" || s=="--help")
    param.help=true;
  param.inputFile=s;
  param.outputFile=string(argv[2]);
}

// read documents in a file, using the document buffer given as argument
// (can be BoWDocument* or BoWDocumentST*)
void readDocuments(istream& fileIn, ostream& out)
{
  while (! fileIn.eof())
  {
    // enum type with small number of values coded on one byte;
    BoWBlocType blocType;
    fileIn >> blocType;
    if (blocType==DOCUMENT_PROPERTIES_BLOC)
    {
      Lima::Common::Misc::DocumentProperties props;
      props.read(fileIn);
      props.write(out);
      fileIn >> blocType;
    }
    BoWText text;
    while (blocType==BOW_TEXT_BLOC)
    {
      text.read(fileIn);
      if (fileIn.eof()) break;
      fileIn >> blocType;
    }
    text.write(out);
  }
}

//**********************************************************************
//
// M A I N
//
//**********************************************************************
int main(int argc, char *argv[])
{
  QsLogging::initQsLog();
  if (argc<1) {    cerr << USAGE; exit(1); }
  readCommandLineArguments(argc,argv);
  if (param.help) { cerr << HELP; exit(1); }

  // read BoWFile and output documents

  ifstream fileIn(param.inputFile.c_str());
  ofstream out(param.outputFile.c_str());
  if (! fileIn)
  {
    cerr << "cannot open input file [" << param.inputFile << "]" << endl;
    exit(1);
  }
  if (! out)
  {
    cerr << "cannot open out file [" << param.outputFile << "]" << endl;
    exit(1);
  }
  BoWFileHeader header;
  try
  {
    header.read(fileIn);
  }
  catch (VersionException& e)
  {
    cerr << "Error: " << e.what() << endl;
    exit(1);
  }

  switch (header.getFileType())
  {
      case BOWFILE_DOCUMENT:
      {
        cerr << "ReadBoWFile: file contains a BoWDocument" << endl;
        try
        {
          // write 0.5 header
          Lima::Common::Misc::writeString(out,"0.5");
          // enum type with small number of values coded on one byte;
          unsigned char typeCode=static_cast<unsigned char>(BOWFILE_DOCUMENT);
          out.write((char*) &(typeCode), sizeof(unsigned char));

          readDocuments(fileIn,out);
        }
        catch (exception& e) { cerr << "Error: " << e.what() << endl; }
        fileIn.close();
        out.close();
        break;
      }
      default:
      {
        cerr << "format of file " << header.getTypeString() << " not managed"
        << endl;
        exit(1);
      }
  }
  return EXIT_SUCCESS;
}
