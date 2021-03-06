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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "SpecificEntitiesConstraints.h"
#include "SpecificEntitiesMicros.h"

// #include <assert.h>

using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{

namespace LinguisticProcessing
{

namespace SpecificEntities
{

// factories for constraint functions defined in this file
ConstraintFunctionFactory<isASpecificEntity>
isASpecificEntityFactory(isASpecificEntityId);

ConstraintFunctionFactory<isInSameSpecificEntity>
  isInSameSpecificEntityFactory(isInSameSpecificEntityId);

ConstraintFunctionFactory<CreateSpecificEntity>
  CreateSpecificEntityFactory(CreateSpecificEntityId);



isASpecificEntity::
isASpecificEntity(MediaId language,
                  const LimaString& complement):
ConstraintFunction(language,complement),
m_type()
{
  if (! complement.isEmpty()) {
    m_type=Common::MediaticData::MediaticData::single().getEntityType(complement);
  }
}

bool isASpecificEntity::operator()(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
                                   const LinguisticGraphVertex& v,
                                   AnalysisContent& analysis) const
{
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData == 0)
  {
    return false;
  }
  bool annotFound = false;
  //std::set< uint64_t > matches = annotationData->matches(recoData->getGraphId(),v,"annot"); portage 32 64
  //for (std::set< uint64_t >::const_iterator it = matches.begin(); portage 32 64
  std::set< AnnotationGraphVertex > matches = annotationData->matches(recoData->getGraphId(),v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    if (annotationData->hasAnnotation(*it, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      annotFound = true;
      break;
    }
  }
  
  if (!annotFound)
  {
    return false;
  }

  if (m_type == Common::MediaticData::EntityType())
  {
    return true;
  }
  else
  {
    //for (std::set< uint64_t >::const_iterator it = matches.begin(); portage 32 64
    for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
         it != matches.end(); it++)
    {
      if (annotationData->annotation(*it,
                                     Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>()->getType() == m_type)
      {
        return true;
      }
    }
    return false;
  }
  return false;
}

isInSameSpecificEntity::
  isInSameSpecificEntity(MediaId language,
                         const LimaString& complement):
  ConstraintFunction(language,complement),
  m_type()
{
  if (! complement.isEmpty()) {
    m_type=Common::MediaticData::MediaticData::single().getEntityType(complement);
  }
}

/** @brief Tests if the two given vertices are in the same specific entity
  *
  * There is several cases:
  *   - va1 and va2 are SE vertices : true iff va1 == va2
  *   - va1 and va2 are standard vertices : true iff there is an outgoing edge
  *     in the annotation graph annotated with "belongstose" from each of them
  *     and toward the same vertex
  *   - va1 (va2) is a SE vertex and there is an outgoing edge in the annotation
  *     graph annotated with "belongstose" from va2 (va1) to va1 (va2).
  *
  * In all the cases, va1 and va2 are the uniq "morphannot" matches of v1 and v2
  *
  * @note This method handles only the first level of SE: if a SE is recursively
  * included in a second one, morph vertices from the first one and from the
  * the second one (not in the first one) will NOT be considered as being in the
  * same specific entity.
  * @note It is considered that a morph vertex can be directly in only one SE.
  * So, its annotation vertex will have at most one "belongstose" annotated
  * outgoing edge.
  */
bool isInSameSpecificEntity::operator()(
           const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
           const LinguisticGraphVertex& v1,
           const LinguisticGraphVertex& v2,
           AnalysisContent& analysis) const
{
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  AnnotationGraphVertex va1 = *(annotationData->matches(recoData->getGraphId(), v1, "annot").begin());
  AnnotationGraphVertex va2 = *(annotationData->matches(recoData->getGraphId(), v2, "annot").begin());

  if ( (va1 == va2) && annotationData->hasAnnotation(va1, Common::Misc::utf8stdstring2limastring("SpecificEntity")) )
  { // first case
    return true;
  }
  AnnotationGraphVertex vase = std::numeric_limits<uint64_t>::max();
  AnnotationGraphVertex va = std::numeric_limits<uint64_t>::max();
  if (annotationData->hasAnnotation(va1, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
  {
    vase = va1;
    va = va2;
  }
  else if (annotationData->hasAnnotation(va2, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
  {
    vase = va2;
    va = va1;
  }
  if (vase == std::numeric_limits<uint64_t>::max())
  { // second case
    AnnotationGraphOutEdgeIt it1, it1_end;
    AnnotationGraphVertex se1 = std::numeric_limits<uint64_t>::max();
    boost::tie(it1, it1_end) = out_edges(va1, annotationData->getGraph());
    for (; it1 != it1_end; it1++)
    {
      if ( annotationData->intAnnotation((*it1), Common::Misc::utf8stdstring2limastring("belongstose"))==1)
      {
        se1 = target(*it1, annotationData->getGraph());
        break;
      }
    }
    if (se1 == std::numeric_limits<uint64_t>::max())
    {
      return false;
    }
    AnnotationGraphVertex se2 = std::numeric_limits<uint64_t>::max();
    AnnotationGraphOutEdgeIt it2, it2_end;
    boost::tie(it2, it2_end) = out_edges(va2, annotationData->getGraph());
    for (; it2 != it2_end; it2++)
    {
      if ( annotationData->intAnnotation((*it2), Common::Misc::utf8stdstring2limastring("belongstose"))==1)
      {
        se2 = target(*it2, annotationData->getGraph());
        break;
      }
    }
    return (se1 == se2);
  }
  else
  { // third case
    bool ok; AnnotationGraphEdge e;
    boost::tie(e, ok) = edge(va,vase,annotationData->getGraph());
    return (ok && (annotationData->intAnnotation(e, Common::Misc::utf8stdstring2limastring("belongstose"))==1));
  }
}



CreateSpecificEntity::CreateSpecificEntity(MediaId language,
                       const LimaString& complement):
ConstraintFunction(language,complement),
m_language(language)
{
  SELOGINIT;
  m_sp=&(Common::MediaticData::MediaticData::changeable().stringsPool(language));

  LimaString str=complement; // copy for easier parse (modify)
  LimaString sep=Common::Misc::utf8stdstring2limastring(",");
  if (!str.isEmpty()) {
    LimaString typeName;
    int j=str.indexOf(sep);
    if (j!=-1) {
      typeName=str.left(j);
      str=str.mid(j+1);
    }
    else {
      typeName=complement;
      str.clear();
    }
    LDEBUG << "CreateSpecificEntity: getting entity type "
           <<  Common::Misc::limastring2utf8stdstring(typeName);
    m_type=Common::MediaticData::MediaticData::single().getEntityType(typeName);
  }

  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));

  const Common::PropertyCode::PropertyManager& microManager=
      static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().
getPropertyManager("MICRO");

  if (!str.isEmpty())
  {
    //uint64_t currentPos = 0; portage 32 64
    int currentPos = 0;
    while (currentPos != str.size()+1)
    {
      int sepPos = str.indexOf(sep, currentPos);

      if (sepPos == -1)
      {
        sepPos = str.size();
      }
      std::string smicro=Common::Misc::limastring2utf8stdstring(str.mid(currentPos, sepPos-currentPos));
      LinguisticCode micro = microManager.getPropertyValue(smicro);
      m_microsToKeep.insert(micro);
//       LDEBUG << "Added " << smicro << " / " << micro << " to micros to keep";
      currentPos = sepPos+1;
    }
  }
}
    
    
    
    
    
// CreateSpecificEntity::CreateSpecificEntity(MediaId language,
//                        const LimaString& complement):
// ConstraintFunction(language,complement),
// m_language(language)
// {
//   SELOGINIT;
//   m_sp=&(Common::MediaticData::MediaticData::changeable().stringsPool(language));
// 
//   std::string str=Common::Misc::limastring2utf8stdstring(complement);
//   LDEBUG << "CreateSpecificEntity constructor with complement: " <<  str;
//   if (! str.empty()) {
//     //uint64_t i=str.find("group:"); portage 32 64
//     std::string::size_type i=str.find("group:");
//     if (i!=std::string::npos) {
//       //uint64_t j=str.find(","); portage 32 64
//       std::string::size_type j=str.find(",");
//       entityGroup=std::string(str,i+6,j-i-6);
//       LDEBUG << "CreateSpecificEntity: use group " << entityGroup;
//       if (j==std::string::npos) {
//  str.clear();
//       }
//       else {
//         str=std::string(str,j+1);
//       }
//     }
//     else LDEBUG << "CreateSpecificEntity: no group specified";
//   }
//   if (!str.empty()) {
//     LimaString typeName;
//     uint64_t j=str.find(sep);
//     if (j!=std::string::npos) {
//       typeName=LimaString(str,0,j);
//       str=LimaString(str,j+1);
//     }
//     else {
//       typeName=complement;
//       str.clear();
//     }
//     LDEBUG << "CreateSpecificEntity: getting entity type " 
//            <<  Common::Misc::limastring2utf8stdstring(typeName);
//     m_type=Common::MediaticData::MediaticData::single().getEntityType(typeName);
//   }
// 
//   m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
// 
//   const Common::PropertyCode::PropertyManager& microManager=
//       static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MICRO");
// 
//   if (!str.empty())
//   {
//     //uint64_t currentPos = 0; portage 32 64
//     std::string::size_type currentPos = 0;
//     while (currentPos != str.size()+1)
//     {
//       std::string::size_type sepPos = str.find(sep, currentPos);
// 
//       if (sepPos == LimaString::npos)
//       {
//         sepPos = str.size();
//       }
//       std::string smicro=Common::Misc::limastring2utf8stdstring(LimaString(str, currentPos, sepPos-currentPos));
//       LinguisticCode micro = microManager.getPropertyValue(smicro);
//       m_microsToKeep.insert(micro);
// //       LDEBUG << "Added " << smicro << " / " << micro << " to micros to keep";
//       currentPos = sepPos+1;
//     }
//   }
// }


/** @todo Verifier la Completude du mapping entre graphes morpho, synt et d'annotation */
bool CreateSpecificEntity::operator()(Automaton::RecognizerMatch& match,
                                      AnalysisContent& analysis) const
{
   SELOGINIT;
   LDEBUG << "CreateSpecificEntity: create entity of type " << match.getType() << " on vertices " << match;
  if (match.empty()) return false;
  LinguisticGraphVertex v1 = (*(match.begin())).m_elem.first;
  LinguisticGraphVertex v2 = (*(match.rbegin())).m_elem.first;
  const LinguisticAnalysisStructure::AnalysisGraph& graph = *(match.getGraph());
  
//     LDEBUG << "CreateSpecificEntity action between " << v1 << " and " << v2
//         << " with complement " << m_complement;
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    return false;
  }
  // do not create annotation if annotation of same type exists
  if (match.size() == 1) 
  {
    //&& (isASpecificEntity(0,LimaString())(graph,v1,analysis)))
    //std::set< uint64_t > matches = annotationData->matches(graph.getGraphId(),v1,"annot"); portage 32 64
    //for (std::set< uint64_t >::const_iterator it = matches.begin(); portage 32 64
    std::set< AnnotationGraphVertex > matches = annotationData->matches(graph.getGraphId(),v1,"annot");
    for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
         it != matches.end(); it++) {
      if (annotationData->hasAnnotation(*it, Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          && annotationData->annotation(*it,
                                        Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>()->getType() == match.getType() ) {
        return false;
      }
    }
  }

/*  {
    std::set< uint32_t > annots = annotationData->matches(recoData.getGraphId(), v1, "annot");
    for ( std::set< uint32_t >::iterator it = annots.begin(); it != annots.end(); it++)
    {
      if (annotationData->hasAnnotation( *it, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    }
  }*/
  if (annotationData->dumpFunction("SpecificEntity") == 0)
  {
    annotationData->dumpFunction("SpecificEntity", new DumpSpecificEntityAnnotation());
  }

  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  std::string graphId=recoData->getGraphId();
    
//   LDEBUG << "    match is " << match;

//   LDEBUG << "    Creating annotation "<< LENDL;
  SpecificEntityAnnotation annot(match,*m_sp);
  

//   LDEBUG << "    Building new morphologic data for head "<< annot.getHead();
  // getting data
  LinguisticGraph* lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
//   LDEBUG << "There is " << out_degree(v2, *lingGraph) << " edges out of " << v2;
  VertexTokenPropertyMap tokenMap = get(vertex_token, *lingGraph);
  VertexDataPropertyMap dataMap = get(vertex_data, *lingGraph);

  const MorphoSyntacticData* dataHead = dataMap[annot.getHead()];

  // Preparer le Token et le MorphoSyntacticData pour le nouveau noeud. Construits
  // a partir des infos de l'entitee nommee
  StringsPoolIndex seFlex = annot.getString();
  StringsPoolIndex seLemma = annot.getNormalizedString();
  StringsPoolIndex seNorm = annot.getNormalizedForm();

//   LDEBUG << "    Creating LinguisticElement"<< LENDL;
//   LDEBUG << "    Creating MorphoSyntacticData"<< LENDL;
  // creer un MorphoSyntacticData
  MorphoSyntacticData* newMorphData = new MorphoSyntacticData();

  // all linguisticElements of this morphosyntacticData share common SE information
  LinguisticElement elem;
  elem.inflectedForm = seFlex; // StringsPoolIndex
  elem.lemma = seLemma; // StringsPoolIndex
  elem.normalizedForm = seNorm; // StringsPoolIndex
  elem.type = SPECIFIC_ENTITY; // MorphoSyntacticType

  if (! m_microsToKeep.empty()) { 
    // micros are given in the rules
    addMicrosToMorphoSyntacticData(newMorphData,dataHead,m_microsToKeep,elem);
  }
  else {
    LDEBUG << "CreateSpecificEntity, use micros from config file ";
    // use micros given in the config file : get the specific resource
    // (specific to modex) 
    // WARN : some hard coded stuff here in resource names
    EntityType seType=match.getType();
    std::string resourceName=
      Common::Misc::limastring2utf8stdstring(Common::MediaticData::MediaticData::single().getEntityGroupName(seType.getGroupId()))+"Micros";
    AbstractResource* res=LinguisticResources::single().getResource(m_language,resourceName);
    LDEBUG << "Entities resource name is : " << resourceName;
    if (res!=0) {
      SpecificEntitiesMicros* entityMicros=static_cast<SpecificEntitiesMicros*>(res);
      const std::set<LinguisticCode>* micros=entityMicros->getMicros(seType);
      if (logger.isDebugEnabled()) 
      {
        std::ostringstream oss;
        for (std::set<LinguisticCode>::const_iterator it=micros->begin(),it_end=micros->end();it!=it_end;it++) {
          oss << (*it) << ";";
        }
        LDEBUG << "CreateSpecificEntity, micros are " << oss.str();
      }
      addMicrosToMorphoSyntacticData(newMorphData,dataHead,*micros,elem);
    }
    else {
      // cannot find micros for this type: error
      LERROR << "CreateSpecificEntity: missing resource " << resourceName ;
    }
  }

  const FsaStringsPool& sp=*m_sp;
  Token* newToken = new Token(
      seFlex,
      sp[seFlex],
      match.positionBegin(),
      match.length());

  // always take status from first element in match
  //if (match.size() == 1)
  //{
  newToken->setStatus(tokenMap[v1]->status());
  //}  

//   LDEBUG << "    Updating morphologic graph "<< graphId;
  // creer le noeud et ses 2 arcs
  LinguisticGraphVertex newVertex;
  DependencyGraphVertex newDepVertex = 0;
  if (syntacticData != 0)
  {
    boost::tie (newVertex, newDepVertex) = syntacticData->addVertex();
  }
  else
  {
    newVertex = add_vertex(*lingGraph);
  }
  AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
  annotationData->addMatching(graphId, newVertex, "annot", agv);
  annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring(graphId), newVertex);
  tokenMap[newVertex] = newToken;
  dataMap[newVertex] = newMorphData;
  LDEBUG << "      - new vertex " << newVertex << "("<<graphId<<"), " << newDepVertex
      << "(dep), " << agv << "(annot) added";

//   LDEBUG << "    Setting annotation "<< LENDL;
  GenericAnnotation ga(annot);

  annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("SpecificEntity"), ga);
//   LDEBUG << "    Creating SE annotation edges between SE match vertices "
//       "annotation and the new annotation vertex"<< LENDL;
  Automaton::RecognizerMatch::const_iterator matchIt, matchIt_end;
  matchIt = match.begin(); matchIt_end = match.end();
  for (; matchIt != matchIt_end; matchIt++)
  {
    std::set< AnnotationGraphVertex > matches = annotationData->matches(graphId,(*matchIt).m_elem.first,"annot");
    if (matches.empty())
    {
      LERROR << "CreateSpecificEntity::operator() No annotation 'annot' for" << (*matchIt).m_elem.first;
    }
    else
    {
      AnnotationGraphVertex src = *(matches.begin());
      annotationData->annotate( src, agv, Common::Misc::utf8stdstring2limastring("belongstose"), 1);
    }
  }

  // creer les relations necessaires dans le graphe morphosyntaxique
  // 1. entre les noeuds avant v1 et le nouveau noeud
  std::vector< LinguisticGraphVertex > previous;
  LinguisticGraphInEdgeIt firstInEdgesIt, firstInEdgesIt_end;
  boost::tie(firstInEdgesIt, firstInEdgesIt_end) = in_edges(v1, *lingGraph);
  std::set< std::pair<LinguisticGraphVertex,LinguisticGraphVertex > > newEdgesToRemove;
  for (; firstInEdgesIt != firstInEdgesIt_end; firstInEdgesIt++)
  {
    LinguisticGraphVertex firstInVertex = source(*firstInEdgesIt, *lingGraph);
    previous.push_back(firstInVertex);
    if (shouldRemoveInitial(source(*firstInEdgesIt, *lingGraph),target(*firstInEdgesIt, *lingGraph), match))
    {
      LDEBUG << "        - storing edge " << source(*firstInEdgesIt, *lingGraph) <<" -> "<<target(*firstInEdgesIt, *lingGraph) << " to be removed"<< LENDL;
/*      recoData->setEdgeToBeRemoved(analysis, *firstInEdgesIt);*/
      newEdgesToRemove.insert(std::make_pair(source(*firstInEdgesIt, *lingGraph),target(*firstInEdgesIt, *lingGraph)));
    }
    else
    {
      LDEBUG << "        - do not store initial edge " << source(*firstInEdgesIt, *lingGraph) <<" -> "<<target(*firstInEdgesIt, *lingGraph) << " to be removed"<< LENDL;
    }
  }
  std::vector< LinguisticGraphVertex >::iterator pit, pit_end;
  pit = previous.begin(); pit_end = previous.end();
  for (; pit != pit_end; pit++)
  {
    /* Si X-Y doit etre supprime et que Z remplace Y , alors ne pas creer X-Z 
    autrement dit si *pit-v1 est dans recoData->m_edgesToRemove, ne pas creer l'arc */
    if (!recoData->isEdgeToBeRemoved(*pit, v1))
    {
      bool success;
      LinguisticGraphEdge e;
      boost::tie(e, success) = add_edge(*pit, newVertex, *lingGraph);
      if (success)
      {
        LDEBUG << "        - in edge " << e.m_source << " -> " << e.m_target << " added"<< LENDL;
      }
      else
      {
        LERROR << "        - in edge " << *pit << " ->" << newVertex << " NOT added"<< LENDL;
      }
    }
    else
    {
      LDEBUG << "        - edge " << *pit << " - " << newVertex << " not added because " << *pit << " - " << v1 << " has to be removed";
    }
  }
  std::set< std::pair<LinguisticGraphVertex,LinguisticGraphVertex > >::iterator newEdgesToRemoveIt = newEdgesToRemove.begin();
  for (; newEdgesToRemoveIt != newEdgesToRemove.end(); newEdgesToRemoveIt++)
  {
    recoData->setEdgeToBeRemoved(analysis, edge( newEdgesToRemoveIt->first, newEdgesToRemoveIt->second, *lingGraph).first);
  }
  LDEBUG << "      - in edges added"<< LENDL;
  
  
  // 2. entre le nouveau noeud et les noeuds qui etaient apres v2
  LDEBUG << "        there is " << out_degree(v2, *lingGraph) << " edges out of " << v2;
  LinguisticGraphOutEdgeIt secondOutEdgesIt, secondOutEdgesIt_end;
  boost::tie(secondOutEdgesIt, secondOutEdgesIt_end) = out_edges(v2, *lingGraph);
  std::vector< LinguisticGraphVertex > nexts;
  for (; secondOutEdgesIt != secondOutEdgesIt_end; secondOutEdgesIt++)
  {
    LDEBUG << "        looking at edge " << source(*secondOutEdgesIt, *lingGraph) << " -> " << target(*secondOutEdgesIt, *lingGraph);
    LinguisticGraphVertex secondOutVertex = target(*secondOutEdgesIt, *lingGraph);
    if (secondOutVertex ==  v2) continue;
    nexts.push_back(secondOutVertex);
    if (shouldRemoveFinal(source(*secondOutEdgesIt, *lingGraph),target(*secondOutEdgesIt, *lingGraph), match))
    {
      LDEBUG << "        - storing edge " << source(*secondOutEdgesIt, *lingGraph) << " -> " << target(*secondOutEdgesIt, *lingGraph) << " to be removed"<< LENDL;
      recoData->setEdgeToBeRemoved(analysis, *secondOutEdgesIt);
    }
    else
    {
      LDEBUG << "        - do not store final edge " << source(*secondOutEdgesIt, *lingGraph) << " -> " << target(*secondOutEdgesIt, *lingGraph) << " to be removed"<< LENDL;
    }
  }
  std::vector< LinguisticGraphVertex >::iterator nit, nit_end;
  nit = nexts.begin(); nit_end = nexts.end();
  for (; nit != nit_end; nit++)
  {
    bool success;
    LinguisticGraphEdge e;
    boost::tie(e, success) = add_edge(newVertex, *nit, *lingGraph);
    if (success)
    {
      LDEBUG << "        - out edge " << e.m_source << " -> " << e.m_target << " added"<< LENDL;
    }
    else
    {
      LERROR << "        - out edge " << newVertex << " ->" << *nit << " NOT added"<< LENDL;
    }
  }
  LDEBUG << "      - out edges added"<< LENDL;

  // 3. supprimer les arcs a remplacer
  recoData->removeEdges( analysis );
  
  // 4 specifier le noeud suivant a utiliser dans la recherche :
  // - nouveau noeud si l'expression reconnue etait composee de plusieurs noeuds
  // - les fils du nouveau noeud sinon (pour eviter les bouclages)
  if (annot.m_vertices.size() > 1)
  {
    recoData->setNextVertex(newVertex);
  }
  else
  {
    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    boost::tie(outItr,outItrEnd) = out_edges(newVertex,*lingGraph);
    for (;outItr!=outItrEnd;outItr++)
    {
      recoData->setNextVertex(target(*outItr, *lingGraph));
    }
  }
  RecognizerMatch::const_iterator matchItr=match.begin();
  for (; matchItr!=match.end(); matchItr++)
  {
    recoData->clearUnreachableVertices( analysis, (*matchItr).getVertex());
  }
  
  return true;
}

void CreateSpecificEntity::addMicrosToMorphoSyntacticData(LinguisticAnalysisStructure::MorphoSyntacticData* newMorphData,
                               const LinguisticAnalysisStructure::MorphoSyntacticData* oldMorphData,
                               const std::set<LinguisticCode>& micros,
                               LinguisticAnalysisStructure::LinguisticElement& elem) const
{
  // try to filter existing microcategories
  for (MorphoSyntacticData::const_iterator it=oldMorphData->begin(), 
         it_end=oldMorphData->end(); it!=it_end; it++) {
    
    if (micros.find(m_microAccessor->readValue((*it).properties)) !=
        micros.end()) {
      elem.properties=(*it).properties;
      newMorphData->push_back(elem);
    }
  }
  // if no categories kept : assign all micros to keep
  if (newMorphData->empty()) {
    for (std::set<LinguisticCode>::const_iterator it=micros.begin(),
           it_end=micros.end(); it!=it_end; it++) {
      elem.properties=*it;
      newMorphData->push_back(elem);
    }
  }
}

bool CreateSpecificEntity::shouldRemoveInitial(
                                               LinguisticGraphVertex /*src*/, 
                                               LinguisticGraphVertex /*tgt*/, 
                                               const RecognizerMatch& match) const
{
  SELOGINIT;
  if (match.size() == 1)
  {
    return true;
  }
  const LinguisticAnalysisStructure::AnalysisGraph& graph = *(match.getGraph());
  
  std::set< LinguisticGraphVertex > matchVertices;
  Automaton::RecognizerMatch::const_iterator matchIt, matchIt_end;
  
  matchIt = match.begin(); 
  matchIt_end = match.end();
  for (; matchIt != matchIt_end; matchIt++)
  {
    matchVertices.insert((*matchIt).m_elem.first);
  }
  
  matchIt = match.begin(); 
  matchIt_end = match.end()-1;
  if (boost::out_degree((*matchIt).m_elem.first,*graph.getGraph()) > 1)
  {
    LDEBUG << "removing edge (" << (*matchIt).m_elem.first << "," << (*(matchIt+1)).m_elem.first << ") because there is more than one path from the first vertex of the match.";
    boost::remove_edge((*matchIt).m_elem.first,(*(matchIt+1)).m_elem.first, *const_cast<LinguisticGraph*>(graph.getGraph()));
    return false;
  }
  matchIt++;
  for (; matchIt != matchIt_end; matchIt++)
  {
    if (boost::out_degree((*matchIt).m_elem.first,*graph.getGraph()) > 1)
    {
      LinguisticGraphOutEdgeIt outIt, outIt_end;
      boost::tie (outIt, outIt_end) = boost::out_edges((*matchIt).m_elem.first, *graph.getGraph());
      for (; outIt != outIt_end; outIt++) 
      {
        if (matchVertices.find(source(*outIt, *graph.getGraph())) != matchVertices.end())
        {
//            LDEBUG << "removing initial edge " << *outIt;
//           boost::remove_edge(*outIt, *const_cast<LinguisticGraph*>(graph.getGraph()));
          break;
        }
      }
      return false;
    }
  }
  
  return true;
}

bool CreateSpecificEntity::shouldRemoveFinal(
                                             LinguisticGraphVertex /*src*/, 
                                             LinguisticGraphVertex /*tgt*/, 
                                             const RecognizerMatch& match) const
{
  SELOGINIT;
  if (match.size() == 1)
  {
    return true;
  }
  const LinguisticAnalysisStructure::AnalysisGraph& graph = *(match.getGraph());
  
  std::set< LinguisticGraphVertex > matchVertices;
  Automaton::RecognizerMatch::const_iterator matchIt, matchIt_end;
  
  matchIt = match.begin(); 
  matchIt_end = match.end();
  for (; matchIt != matchIt_end; matchIt++)
  {
    matchVertices.insert((*matchIt).m_elem.first);
  }
  
  matchIt = match.begin()+1; 
  matchIt_end = match.end();
  for (; matchIt != matchIt_end; matchIt++)
  {
    if (boost::in_degree((*matchIt).m_elem.first,*graph.getGraph()) > 1)
    {
      LinguisticGraphInEdgeIt inIt, inIt_end;
      boost::tie (inIt, inIt_end) = boost::in_edges((*matchIt).m_elem.first, *graph.getGraph());
      for (; inIt != inIt_end; inIt++) 
      {
        if (matchVertices.find(source(*inIt, *graph.getGraph())) != matchVertices.end())
        {
          LDEBUG << "removing final edge " << source(*inIt, *graph.getGraph()) << " -> " << target(*inIt, *graph.getGraph());
          boost::remove_edge(*inIt, *const_cast<LinguisticGraph*>(graph.getGraph()));
          break;
        }
      }
      return false;
    }
  }
  
  return true;
}




} // SpecificEntities

} // LinguisticProcessing

} // Lima
