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
/**
  *
  * @file        SyntagmaticMatrix.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr)
  * @date        Created on  : Mon Aug 04 2003
  *              Copyright   : (c) 2003 by CEA
  * @version     $Id$
  *
  */

#include "SyntagmaticMatrix.h"
#include "XmlSyntagmaticMatrixFileHandler.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include <QtXml/QXmlSimpleReader>

#include <string>
#include <algorithm>

using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

SimpleFactory<AbstractResource,SyntagmDefStruct> syntagmDefStructFactory(SYNTAGMDEFSTRUCT_CLASSID);


SyntagmDefStruct::SyntagmDefStruct() :
    m_language(),
    m_nominalMatrix(),
    m_verbalMatrix(),
    m_nomChainBeg(),
    m_nomChainEnd(),
    m_verbChainBeg(),
    m_verbChainEnd()
{
}

SyntagmDefStruct::~SyntagmDefStruct() {}


void SyntagmDefStruct::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SALOGINIT;
  LDEBUG << "Creating a SyntagmDefStruct (loads file)";
  m_language=manager->getInitializationParameters().language;
  m_macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  m_nominalMatrix.language(m_language);
  m_verbalMatrix.language(m_language);
  
  try {
    std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
    std::string matricesFileName=resourcePath + "/" + unitConfiguration.getParamsValueAtKey("file");
    loadFromFile(matricesFileName);
  
  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'file' in SyntagmDefinitionStructure group for language " << (int) m_language << " !";
    throw InvalidConfiguration();
  }

//  display();
}


void SyntagmDefStruct::deleteMatrices()
{
  m_nominalMatrix.deleteMatrix();
  m_verbalMatrix.deleteMatrix();
}


void SyntagmDefStruct::loadFromFile(const std::string& fileName)
{
  SALOGINIT;
  LDEBUG << "Loading matrices from " << fileName;
  //
  //  Create a SAX parser object. Then, according to what we were told on
  //  the command line, set it to validate or not.
  //
  QXmlSimpleReader parser;
//   parser.setValidationScheme(SAXParser::Val_Auto);
//   parser.setDoNamespaces(false);
//   parser.setDoSchema(false);
//   parser.setValidationSchemaFullChecking(false);
  
  
  //
  //  Create the handler object and install it as the document and error
  //  handler for the parser-> Then parse the file and catch any exceptions
  //  that propogate out
  //
  XMLSyntagmaticMatrixFileHandler handler(*this,m_language);
  parser.setContentHandler(&handler);
  parser.setErrorHandler(&handler);
  QFile file(fileName.c_str());
  if (!file.open(QFile::ReadOnly))
  {
    XMLCFGLOGINIT;
    LERROR << "Error opening " << fileName.c_str();
    throw XMLException(std::string("SyntagmDefStruct::loadFromFile Unable to open ") + fileName);
  }
  if (!parser.parse( QXmlInputSource(&file)))
  {
    LERROR << "Error parsing " << fileName.c_str();
    throw XMLException(std::string("SyntagmDefStruct::loadFromFile Unable to parse ") + fileName + " : " + parser.errorHandler()->errorString().toUtf8().constData());
  }
}

void SyntagmaticMatrix::deleteMatrix()
{
  std::map< TokenFilter, SyntagmaticMatrixRow, tfless >::iterator it, it_end;

  it = m_filters.begin(); it_end = m_filters.end();
  for (; it != it_end; it++)
  {
    (*it).second.deleteRow();
  }
}

void SyntagmaticMatrix::display() const
{
  std::cout << "Displaying a matrix" << std::endl;
  for (std::map< TokenFilter, SyntagmaticMatrixRow, tfless >::const_iterator it(m_filters.begin()); it != m_filters.end(); it++)
  {
    std::cout << ((*it).first) << " : ";
    for (std::set< TokenFilter, tfless >::const_iterator it2((*it).second.m_filters.begin()); it2 != (*it).second.m_filters.end(); it2++)
    {
      std::cout << (*it2) << ", ";
    }
    std::cout << std::endl << std::endl;
  }
}

void SyntagmDefStruct::display() const
{
  m_nominalMatrix.display();
  std::cout << std::endl << std::endl;
  m_verbalMatrix.display();
  std::cout << std::endl << std::endl;
  std::cout << m_nomChainBeg << std::endl << std::endl;
  std::cout << m_nomChainEnd << std::endl << std::endl;
  std::cout << m_verbChainBeg << std::endl << std::endl;
  std::cout << m_verbChainEnd << std::endl << std::endl;
}

bool SyntagmDefStruct::canChainBeginBy(const MorphoSyntacticData* filter, ChainsType type) const
{
//   SALOGINIT;
//   LDEBUG << "SyntagmDefStruct::canChainBeginBy(" << filter << "," << type << ")";
  if (type == NOMINAL)
    return canNominalChainBeginBy(filter);
  else if (type == VERBAL)
    return canVerbalChainBeginBy(filter);
  else return false;
}

bool SyntagmDefStruct::canChainEndBy(const MorphoSyntacticData* filter, ChainsType type) const
{
  if (type == NOMINAL)
    return canNominalChainEndBy(filter);
  else if (type == VERBAL)
    return canVerbalChainEndBy(filter);
  else return false;
}

bool SyntagmDefStruct::belongsToMatrix(const MorphoSyntacticData* src, const MorphoSyntacticData* dest, ChainsType type) const
{
  if (type == NOMINAL)
    return belongsToNominalMatrix(src, dest);
  else if (type == VERBAL)
    return belongsToVerbalMatrix(src, dest);
  else return false;
}

bool SyntagmDefStruct::canNominalChainBeginBy(const MorphoSyntacticData* filter) const
{
  TokenFilter f(filter,m_microAccessor);
  return (m_nomChainBeg.find(f) != m_nomChainBeg.m_filters.end());
}

bool SyntagmDefStruct::canNominalChainEndBy(const MorphoSyntacticData* filter) const
{
  TokenFilter f(filter,m_microAccessor);
  return (m_nomChainEnd.find(f) != m_nomChainEnd.m_filters.end());
}

bool SyntagmDefStruct::canVerbalChainBeginBy(const MorphoSyntacticData* filter) const
{
  TokenFilter f(filter,m_microAccessor);
  return (m_verbChainBeg.find(f) != m_verbChainBeg.m_filters.end());
}

bool SyntagmDefStruct::canVerbalChainEndBy(const MorphoSyntacticData* filter) const
{
  TokenFilter f(filter,m_microAccessor);
  return (m_verbChainEnd.find(f) != m_verbChainEnd.m_filters.end());
}
/*
bool SyntagmDefStruct::belongsToNominalMatrix(const MorphoSyntacticData* src, const MorphoSyntacticData* dest) const
{
  SALOGINIT;
  std::map< TokenFilter, SyntagmaticMatrixRow, tfless >::const_iterator it, it_end;
  it = m_nominalMatrix.begin();
  it_end = m_nominalMatrix.end(); 
  for (; it != it_end; it++)
  {
    if ( (*it).first == src )
    { 
      LDEBUG << "Found a row for " << src << " : " << ((*it).first);
      const SyntagmaticMatrixRow& row = (*it).second;
      SyntagmaticMatrixRow::iterator itRow = row.find(dest);
      LDEBUG << "Searching " << dest << " in " << row;
      if (itRow != row.end()) 
      {
        LDEBUG << "Cell OK found: " << (*itRow); 
        return true;
      }
    }
  }
  LDEBUG << "No row found";
  return false;
}
*/
bool SyntagmDefStruct::belongsToNominalMatrix(const MorphoSyntacticData* src, const MorphoSyntacticData* dest) const
{
  TokenFilter srcTf(src,m_microAccessor);
  TokenFilter destTf(dest,m_microAccessor);
  std::pair<std::map< TokenFilter, SyntagmaticMatrixRow, tfless >::const_iterator,std::map< TokenFilter, SyntagmaticMatrixRow, tfless >::const_iterator> ends =
      std::make_pair(m_nominalMatrix.m_filters.begin(),m_nominalMatrix.m_filters.end());
  for (; ends.first != ends.second; (ends.first)++)
  {
    if ( tf_dwless()(ends.first->first,srcTf) )
    {
      const SyntagmaticMatrixRow& row = (*(ends.first)).second;
      std::set< TokenFilter, tfless >::const_iterator itRow = row.find(destTf);
      if (itRow != row.m_filters.end()) return true;
    }
  }
  return false;
}

bool SyntagmDefStruct::belongsToVerbalMatrix(const MorphoSyntacticData* src, const MorphoSyntacticData* dest) const
{
/*
  std::map< TokenFilter, SyntagmaticMatrixRow, tfless >::const_iterator  it = m_verbalMatrix.find(src);
  if ( it == m_verbalMatrix.end() ) return false;
  const SyntagmaticMatrixRow& row = (*it).second;
  SyntagmaticMatrixRow::iterator itRow = row.find(dest);
  return ( itRow != row.end() );
*/
  TokenFilter srcTf(src,m_microAccessor);
  TokenFilter destTf(dest,m_microAccessor);
  std::pair<std::map< TokenFilter, SyntagmaticMatrixRow, tfless >::const_iterator,std::map< TokenFilter, SyntagmaticMatrixRow, tfless >::const_iterator> ends =
      std::make_pair(m_verbalMatrix.m_filters.begin(),m_verbalMatrix.m_filters.end());
// @todo this equal_range does not work. See why.
//      std::equal_range(m_verbalMatrix.begin(),m_verbalMatrix.end(), 
//          TokenFilter(src), tf_dwless());
  for (; ends.first != ends.second; (ends.first)++)
  {
    if ( tf_dwless()(ends.first->first , srcTf) )
    {
      const SyntagmaticMatrixRow& row = (*(ends.first)).second;
      std::set< TokenFilter, tfless >::const_iterator itRow = row.find(destTf);
      if (itRow != row.m_filters.end()) return true;
    }
  }
  return false;
}

/** Used to search a DicoWord inside syntagmatic structure. y is assured to 
* be constructed from a MorphoSyntacticData */
bool tf_dwless::operator()(const TokenFilter& x, const TokenFilter& y) const
{
//    std::cout << "In tf_dwless(" << x << ", " << y << ") : ";
    bool attribFound = false;
    if (x.attributes().size() > 0)
    {
        std::set< StringsPoolIndex >::const_iterator itx, ity, itx_end, ity_end;
        boost::tie(itx, itx_end) = x.attributesIterators();
        boost::tie(ity, ity_end) = y.attributesIterators();
/*        for (;ity!=ity_end;ity++) 
        {
          if (x.attributes().find( *ity ) != itx_end)
          {
            attribFound = true;
            break;
          }
        }*/
        while ((itx != itx_end) && (ity != ity_end))
        {
          if (*itx == *ity) {
            attribFound = true;
            break;
          }
          if (*itx < *ity) {
            itx++;
          } else {
            ity++;
          }
        }
    } else {
      attribFound = true;
    }
    if (!attribFound) {
      return false;
    }
    bool categFound = false;
    std::set< LinguisticCode >::const_iterator itx, ity, itx_end, ity_end;
    boost::tie(itx, itx_end) = x.categoriesIterators();
    boost::tie(ity, ity_end) = y.categoriesIterators();
/*    for (;ity!=ity_end;ity++) {
      if (x.categories().find( *ity ) != itx_end) {
        categFound = true;
        break;
      }
    }*/
    while ((itx != itx_end) && (ity != ity_end))
    {
      if (*itx == *ity) {
        categFound = true;
        break;
      }
      if (*itx < *ity) {
        itx++;
      } else {
        ity++;
      }
    }
//    std::cout << std::boolalpha << ( categFound && attribFound ) << std::endl;
    return ( categFound );
}

/** Used to search a DicoWord inside syntagmatic structure. y is assured to 
* be constructed from a MorphoSyntacticData */
bool tf_dwless::operator()(const TokenFilter& x, const std::pair<TokenFilter,SyntagmaticMatrixRow>& p) const
{
    bool attribFound = false;
    const TokenFilter& y = p.first;
//    std::cout << "In tf_dwless(" << x << ", " << y << ") : " ;
    if (x.attributes().size() > 0)
    {
        std::set< StringsPoolIndex >::const_iterator itx, ity, itx_end, ity_end;
        boost::tie(itx, itx_end) = x.attributesIterators();
        boost::tie(ity, ity_end) = y.attributesIterators();
/*        for (;ity!=ity_end;ity++) 
        {
          if (x.attributes().find( *ity ) != itx_end)
          {
            attribFound = true;
            break;
          }
        }*/
        while ((itx != itx_end) && (ity != ity_end))
        {
          if (*itx == *ity) {
            attribFound = true;
            break;
          }
          if (*itx < *ity) {
            itx++;
          } else {
            ity++;
          }
        }
    } else {
      attribFound = true;
    }
    if (!attribFound) {
      return false;
    }
    bool categFound = false;
    std::set< LinguisticCode >::const_iterator itx, ity, itx_end, ity_end;
    boost::tie(itx, itx_end) = x.categoriesIterators();
    boost::tie(ity, ity_end) = y.categoriesIterators();
/*    for (;ity!=ity_end;ity++) {
      if (x.categories().find( *ity ) != itx_end) {
        categFound = true;
        break;
      }
    }*/
    while ((itx != itx_end) && (ity != ity_end))
    {
      if (*itx == *ity) {
        categFound = true;
        break;
      }
      if (*itx < *ity) {
        itx++;
      } else {
        ity++;
      }
    }
//    std::cout << std::boolalpha << ( categFound && attribFound ) << std::endl;
    return ( categFound );
}

/** Used to search a DicoWord inside syntagmatic structure. y is assured to 
* be constructed from a MorphoSyntacticData */
bool tf_dwless::operator()(const std::pair<TokenFilter,SyntagmaticMatrixRow>& p, const TokenFilter& y) const
{
    bool attribFound = false;
    const TokenFilter& x = p.first;
    if (x.attributes().size() > 0)
    {
        std::set< StringsPoolIndex >::const_iterator itx, ity, itx_end, ity_end;
        boost::tie(itx, itx_end) = x.attributesIterators();
        boost::tie(ity, ity_end) = y.attributesIterators();
/*        for (;ity!=ity_end;ity++) 
        {
          if (x.attributes().find( *ity ) != itx_end)
          {
            attribFound = true;
            break;
          }
        }*/
        while ((itx != itx_end) && (ity != ity_end))
        {
          if (*itx == *ity) {
            attribFound = true;
            break;
          }
          if (*itx < *ity) {
            itx++;
          } else {
            ity++;
          }
        }
    } else {
      attribFound = true;
    }
    if (!attribFound) {
      return false;
    }
    bool categFound = false;
    std::set< LinguisticCode >::const_iterator itx, ity, itx_end, ity_end;
    boost::tie(itx, itx_end) = x.categoriesIterators();
    boost::tie(ity, ity_end) = y.categoriesIterators();
/*    for (;ity!=ity_end;ity++) {
      if (x.categories().find( *ity ) != itx_end) {
        categFound = true;
        break;
      }
    }*/
    while ((itx != itx_end) && (ity != ity_end))
    {
      if (*itx == *ity) {
        categFound = true;
        break;
      }
      if (*itx < *ity) {
        itx++;
      } else {
        ity++;
      }
    }
//    std::cout << std::boolalpha << ( categFound && attribFound ) << std::endl;
    return ( categFound );
}

std::set< TokenFilter, tfless >::const_iterator SyntagmaticMatrixRow::find(const TokenFilter& f) const
{
  std::set< TokenFilter, tfless >::const_iterator it = m_filters.begin();
  std::set< TokenFilter, tfless >::const_iterator it_end = m_filters.end();
  for (; it != it_end; it++)
  {
    if ( tf_dwless()(*it, f))
      return it;
  }
  return it_end;
}

} // namespace SyntacticAnalysis
} // namespace LinguisticProcessing
} // namespace Lima
