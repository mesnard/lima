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
 * @file       Features.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Feb  7 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "WordFeatureExtractor.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "common/Data/strwstrtools.h"
#include <sstream>

using namespace std;
using namespace boost;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {

//***********************************************************************
// factories for feature extractors
AbstractFeatureExtractor::AbstractFeatureExtractor(MediaId language, const std::string& /*complement*/):
m_language(language)
{
}

AbstractFeatureExtractorFactory::
AbstractFeatureExtractorFactory(const std::string& factoryId):
RegistrableFactory<AbstractFeatureExtractorFactory>(factoryId)
{
}

FeatureExtractorFactory<FeaturePosition> FeaturePositionFactory(FeaturePosition_ID);
FeatureExtractorFactory<FeatureToken> FeatureTokenFactory(FeatureToken_ID);
FeatureExtractorFactory<FeatureLemma> FeatureLemmaFactory(FeatureLemma_ID);
FeatureExtractorFactory<FeatureProperty> FeaturePropertyFactory(FeatureProperty_ID);
FeatureExtractorFactory<FeatureTstatus> FeatureTstatusFactory(FeatureTstatus_ID);

//***********************************************************************
// Feature list
//***********************************************************************
WordFeatures::WordFeatures() : vector<AbstractFeatureExtractor*>(),
m_language(0)
{
}

WordFeatures::WordFeatures(MediaId language) : vector<AbstractFeatureExtractor*>(),
m_language(language)
{
}

WordFeatures::~WordFeatures()
{
  for (WordFeatures::iterator it=begin(),it_end=end(); it!=it_end; it++) {
    if (*it) {
      delete (*it);
      *it=0;
    }
  }
}

void WordFeatures::initialize(const deque<std::string>& featureNames)
{
  for (deque<std::string>::const_iterator it=featureNames.begin(),
         it_end=featureNames.end();it!=it_end;it++) 
  {
    string featureName=(*it);
    string complement;
    string::size_type i=featureName.find(":");
    if (i!=string::npos) {
      complement=string(featureName,i+1);
      featureName=string(featureName,0,i);
    }
    DUMPERLOGINIT;
    LDEBUG << "WordFeatures: initialize feature" << featureName;
    push_back(FeatureLemmaFactory.getFactory(featureName)->create(m_language,complement));
    back()->setName(featureName);
  }
}

//***********************************************************************
// Feature accessors
//***********************************************************************
FeaturePosition::FeaturePosition(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement)
{}

std::string FeaturePosition::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v) const
{
  Token* token=get(vertex_token,*(graph->getGraph()),v);
  if (token==0) {
    return "";
  }
  ostringstream oss;
  oss << token->position();
  return oss.str();
}

//***********************************************************************
FeatureToken::FeatureToken(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement)
{}

std::string FeatureToken::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v) const
{
  Token* token=get(vertex_token,*(graph->getGraph()),v);
  if (token==0) {
    return "";
  }
  return Common::Misc::limastring2utf8stdstring(token->stringForm());
}

//***********************************************************************
FeatureLemma::FeatureLemma(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement),
m_sp()
{
  m_sp=&(Common::MediaticData::MediaticData::single().stringsPool(m_language));
}

std::string FeatureLemma::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v) const
{
  MorphoSyntacticData* data=get(vertex_data,*(graph->getGraph()),v);
  if (data==0) {
    return "";
  }
  // take first
  for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
    return Common::Misc::limastring2utf8stdstring((*m_sp)[(*it).normalizedForm]);
  }
  return "";
}

//***********************************************************************
FeatureProperty::FeatureProperty(MediaId language,const std::string& complement):
AbstractFeatureExtractor(language),
m_propertyName(complement),
m_propertyAccessor(0),
m_propertyManager(0)
{
  const Common::PropertyCode::PropertyCodeManager& codeManager=
  static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager();
  m_propertyAccessor=&codeManager.getPropertyAccessor(m_propertyName);
  m_propertyManager=&codeManager.getPropertyManager(m_propertyName);
}

std::string FeatureProperty::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v) const
{
  MorphoSyntacticData* data=get(vertex_data,*(graph->getGraph()),v);
  if (data==0) {
    return "";
  }
  ostringstream oss;
  // take first / take all ?
  for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
    LinguisticCode code=m_propertyAccessor->readValue((*it).properties);
    oss << m_propertyManager->getPropertySymbolicValue(code);
    break;
  }
  return oss.str();
}

//***********************************************************************
FeatureTstatus::FeatureTstatus(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement)
{}

std::string FeatureTstatus::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v) const
{
  Token* token=get(vertex_token,*(graph->getGraph()),v);
  if (token==0) {
    return "";
  }
  return Common::Misc::limastring2utf8stdstring(token->status().defaultKey());
}



} // end namespace
} // end namespace
