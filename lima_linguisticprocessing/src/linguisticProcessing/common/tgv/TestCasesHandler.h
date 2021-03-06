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
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_TGV_TESTCASESHANDLER_H
#define LIMA_TGV_TESTCASESHANDLER_H

#include "TestCase.h"
#include "TestCaseProcessor.hpp"

#include <QtXml/QXmlDefaultHandler>

namespace Lima
{
namespace Common
{
namespace TGV
{
class LIMA_TGV_EXPORT TestCasesHandler : public QXmlDefaultHandler
{

public:
  TestCasesHandler(TestCaseProcessor& processor);
  virtual ~ TestCasesHandler();

  // -----------------------------------------------------------------------
  //  Implementations of the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  bool endElement(const QString & namespaceURI, const QString & name, const QString & qName);
  
  bool characters(const QString& chars);
  
  bool startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes);
  
  bool startDocument();
  
  
  
  struct TestReport {
    uint64_t success;
    uint64_t failed;
    uint64_t conditional;
    uint64_t nbtests;
    TestReport() : success(0),failed(0),conditional(0),nbtests(0) {};
  };

  std::map<std::string,TestReport> m_reportByType;
  
private:
  std::string attributeValue(const QString& attr, const QXmlAttributes& attrs) const;
  
  // Liste des parametres possibles et obligatoires dans un TestCase
  // pour lp: (text, language, pipeline?...)
  // std::map<std::string,bool> m_simpleValParamTestCaseKeys;
  // std::map<std::string,bool> m_multiValParamTestCaseKeys;
  
  TestCaseProcessor& m_processor;
  TestCase currentTestCase;

  std::string getName(const QString& localName,
                      const QString& qName);
  
  bool m_inText;
  bool m_inExpl;
  bool m_inParam;
  bool m_inList;
  bool m_inMap;
  std::string m_listKey;
  std::string m_mapKey;
};


} // TGV
} // Common
} // Lima


#endif
