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
/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/


#include "common/LimaCommon.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common;
using namespace Lima;


//****************************************************************************
void usage(int argc, char* argv[]);
void decode(const PropertyCodeManager& propcodemanager,LinguisticCode prop);
void encode(const PropertyCodeManager& propcodemanager,vector<string>& args);
void check(const PropertyCodeManager& propcodemanager,LinguisticCode p1, LinguisticCode p2);

//****************************************************************************
// GLOBAL variable -> the command line arguments
struct
{
  string codeFile; // file to parse
  string language;
  string outputFile;
  bool decode;
  bool encode;
  bool check;
  bool help;       // help mode
  string input;
  std::vector<std::string> args;
}
param={"",
       "",
       "",
       false,
       false,
       false,
       false,
       "",
       vector<string>()};

void readCommandLineArguments(uint64_t argc, char *argv[])
{
  for(uint64_t i(1); i<argc; i++)
  {
    string arg(argv[i]);
    std::string::size_type pos;
    if (arg=="-h" || arg=="--help")
      param.help=true;
    else if ( (pos = arg.find("--code=")) != std::string::npos )
    {
      param.codeFile = arg.substr(pos+7);
    }
    else if ( (pos = arg.find("--language=")) != std::string::npos )
    {
      param.language = arg.substr(pos+11);
    }
    else if ( (pos = arg.find("--output=")) != std::string::npos )
    {
      param.outputFile=arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--decode")) != std::string::npos )
    {
      param.decode=true;
    }
    else if ( (pos = arg.find("--encode")) != std::string::npos )
    {
      param.encode=true;
    }
    else if ( (pos = arg.find("--check")) != std::string::npos )
    {
      param.check=true;
    }
    else if ( (pos = arg.find("--input=")) != std::string::npos )
    {
      param.input = arg.substr(pos+8);
    }
    else if (arg[0]=='-')
    {
      cerr << "unrecognized option " <<  arg << endl;
      usage(argc,argv);
      exit(1);
    }
    else
    {
      param.args.push_back(arg);
    }
  }

}

//****************************************************************************
int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  readCommandLineArguments(argc,argv);
  if (param.help)
  {
    usage(argc,argv);
    exit(0);
  }

  std::string resourcesPath=(getenv("LIMA_RESOURCES")!=0)?string(getenv("LIMA_RESOURCES")):string("/usr/share/apps/lima/resources");
  std::string configDir=(getenv("LIMA_CONF")!=0)?string(getenv("LIMA_CONF")):string("/usr/share/config/lima");

  if (param.codeFile == "")
  {
    if (param.language == "")
    {
      cerr << "no codefile nor language specified !" << endl;
      exit(1);
    }
    param.codeFile=resourcesPath+"/LinguisticProcessings/"+param.language+"/code-"+param.language+".xml";
  }

  PropertyCodeManager propcodemanager;
  propcodemanager.readFromXmlFile(param.codeFile);

  if (param.outputFile != "")
  {
    // print debug file

    ofstream fout(param.outputFile.c_str(), std::ofstream::binary);

    fout << "#" << endl;
    fout << "# Automatically generated from " << param.codeFile << endl;
    fout << "#" << endl << endl;
    const std::map<std::string,PropertyManager>& managers=propcodemanager.getPropertyManagers();
    for (std::map<std::string,PropertyManager>::const_iterator propItr=managers.begin();
         propItr!=managers.end();
         propItr++)
    {
      const std::map<std::string,LinguisticCode>& symbol2code=propItr->second.getSymbol2Code();
      for (std::map<std::string,LinguisticCode>::const_iterator valItr=symbol2code.begin();
           valItr!=symbol2code.end();
           valItr++)
      {
        fout << propItr->first << ":" << valItr->first << ":" << valItr->second << endl;
      }
      fout << endl;
    }
  }

  if (param.decode)
  {
    // decode numeric properties
    if (param.input == "")
    {
      for (vector<string>::const_iterator argItr=param.args.begin();
           argItr!=param.args.end();
           argItr++)
      {
        LinguisticCode prop(atoi(argItr->c_str()));
        decode(propcodemanager,prop);
      }
    }
    else
    {
      ifstream fin(param.input.c_str(), std::ifstream::binary);
      string line;
      while (fin.good() && !fin.eof())
      {
        getline(fin,line);
  if (line.size()>0) {
    LinguisticCode prop(atoi(line.c_str()));
    decode(propcodemanager,prop);
        }
      }
    }
  }
  else if (param.encode)
  {
    encode(propcodemanager,param.args);
  }
  else if (param.check)
  {
    // check numeric properties

    if (param.args.size() < 2)
    {
      cout << "Needs two args to check equality !!" << endl;
      exit(0);
    }

    LinguisticCode p1(atoi(param.args[0].c_str()));
    LinguisticCode p2(atoi(param.args[1].c_str()));

    check(propcodemanager,p1,p2);

  }
}

void decode(const Lima::Common::PropertyCode::PropertyCodeManager& propcodemanager, LinguisticCode prop)
{
  const std::map<std::string,PropertyManager>& managers=propcodemanager.getPropertyManagers();
  cout << "** decode " << dec << prop << " (" << hex << prop << ")" << endl;
  for (std::map<std::string,PropertyManager>::const_iterator manItr=managers.begin();
       manItr!=managers.end();
       manItr++)
  {
    LinguisticCode value=manItr->second.getPropertyAccessor().readValue(prop);
    cout << manItr->first << " => " << manItr->second.getPropertySymbolicValue(value);
    if (manItr->second.getPropertyAccessor().empty(prop))
    {
      cout << " (EMPTY)";
    }
    cout << endl;
  }
  cout << endl;
}

void encode(const PropertyCodeManager& propcodemanager,vector<string>& args)
{
  map<string,string> propValues;
  for (vector<string>::const_iterator argItr=args.begin();
       argItr!=args.end();
       argItr++)
  {
    //uint64_t index=argItr->find("=");
    //uint64_t index=argItr->find("=");
    std::vector<std::string>::size_type index=argItr->find("=");
    if (index != string::npos)
    {
      string propName=argItr->substr(0,index);
      string valName=argItr->substr(index+1);
      const PropertyManager& propMan=propcodemanager.getPropertyManager(propName);
      LinguisticCode value=propMan.getPropertyValue(valName);
      cout << propName << " : " << valName << " => " << dec << value << " (" << hex << value << ")" << endl;
      propValues[propName]=valName;
    }
  }
  LinguisticCode coded=propcodemanager.encode(propValues);
  cout << "result = " << dec << coded << " (" << hex << coded << ")" << endl;
}

void check(const Lima::Common::PropertyCode::PropertyCodeManager& propcodemanager, LinguisticCode p1, LinguisticCode p2)
{
  const std::map<std::string,PropertyManager>& managers=propcodemanager.getPropertyManagers();

  cout << "** check " << dec << p1 << " (" << hex << p1 << ") / " << dec << p2 << " (" << hex << p2 << ")" << endl;
  for (std::map<std::string,PropertyManager>::const_iterator manItr=managers.begin();
       manItr!=managers.end();
       manItr++)
  {
    if (manItr->second.getPropertyAccessor().equal(p1,p2))
    {
      cout << "same " << manItr->first << " ( mask is " << hex << manItr->second.getMask() << " ) value = " << manItr->second.getPropertySymbolicValue(p1) << endl;
    }
    else
    {
      cout << "different " << manItr->first << " ( mask is " << hex << manItr->second.getMask() << " ) "
      << "p1 = " << manItr->second.getPropertySymbolicValue(p1) << " ( " << manItr->second.getPropertyAccessor().readValue(p1) << " ) "
      << "p2 = " << manItr->second.getPropertySymbolicValue(p2) << " ( " << manItr->second.getPropertyAccessor().readValue(p2) << " ) " << endl;
    }
  }
  cout << endl;
}

void usage(int argc, char *argv[])
{
  LIMA_UNUSED(argc);
  cout << "usage: " << argv[0] << " [config] [commands]" << endl;
  cout << "where [config] can be either :" << endl;
  cout << "  --code=<xmlPropertyFile>" << endl;
  cout << "  --language=<lang> : use file $LIMA_RESOURCES/LinguisticProcessings/<lang>/code-<lang>.xml" << endl;
  cout << "and [commands] are : " << endl;
  cout << "***** To print debug file *****" << endl;
  cout << "--output=<file> : print debug to <file>" << endl;
  cout << "***** To decode properties *****" << endl;
  cout << "--decode p1 [p2] ... : decode properties, p1 p2 are in numeric format" << endl;
  cout << "--decode --input=file : decode properties in file, une property on each line, in numeric format" << endl;
  cout << "***** To encode properties *****" << endl;
  cout << "--encode p1=v1 [p2=v2] .... : encode properties, pX=vX are name=value of properties to encode" << endl;
  cout << "***** To check equality *****" << endl;
  cout << "--check p1 p2 : check equality of each properties for the given numeric codes" << endl;
  exit(0);
}
