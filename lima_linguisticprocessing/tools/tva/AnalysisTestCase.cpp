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
#include "tools/tva/AnalysisTestCase.h"

#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"

using namespace std;
using namespace Lima::Common::TGV;
using namespace Lima::LinguisticProcessing;

namespace Lima
{

namespace AnalysisValidation
{

AnalysisTestCaseProcessor::AnalysisTestCaseProcessor(
  const std::string& workingDirectory,
  LinguisticProcessing::AbstractLinguisticProcessingClient* client,
  const std::map<std::string, AbstractAnalysisHandler*> &handlers) :
  TestCaseProcessor(workingDirectory),
  m_lpclient(client),
  m_handlers(handlers) {}
    
TestCaseError AnalysisTestCaseProcessor::processTestCase(const Lima::Common::TGV::TestCase& testCase)
{
  // write text in file
  const std::string& text = testCase.getParam( "text" );
  std::string filename(m_workingDirectory+"/test"+testCase.id+".txt");
  {
    ofstream fout(filename.c_str(), std::ofstream::binary);
    fout << text;
    fout.flush();
    fout.close();
  }
  LimaString contentText = Common::Misc::utf8stdstring2limastring(text);
  const std::string& language = testCase.getParam("language");

  // For each pipeline process test
  MultiValCallParams::const_iterator pos = testCase.multiValCallParams.find("pipelines");
  if( pos != testCase.multiValCallParams.end() ) {
    const list<string>& pipeList = (*pos).second;
    for (list<string>::const_iterator pipItr=pipeList.begin();
         pipItr!=pipeList.end();  pipItr++)
    {
  
      string filenameWithPipeLine=filename+"."+*pipItr;

      // Analyse text 
      map<string,string> metaData;
      metaData["Lang"]=language;
      metaData["FileName"]=filenameWithPipeLine;
      metaData["DocumentName"]=testCase.id;
      m_lpclient->analyze(contentText,metaData, *pipItr, m_handlers);
      BowTextHandler* bowHandler = static_cast<BowTextHandler*>(m_handlers["bowTextHandler"]);
      // dump results
      Common::BagOfWords::BoWText text=bowHandler->getBowText();
      string outputfile=filenameWithPipeLine+".indexed.xml";
      ofstream fout(outputfile.c_str(), std::ofstream::binary);
      fout << "<?xml version='1.0' encoding='UTF-8'?>" << endl;
      Common::BagOfWords::BoWXMLWriter writer(fout);
      writer.writeBoWText(&text, true);
      fout.close();
  
      TestCaseError error = evalTestCase( testCase, *pipItr, filename, filenameWithPipeLine );
      if (error())
        return error;
    }
  }
  return TestCaseError();
}


}

}
