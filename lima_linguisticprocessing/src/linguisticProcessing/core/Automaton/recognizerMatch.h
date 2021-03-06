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
 * @file       recognizerMatch.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Oct 13 2004
 * copyright   Copyright (C) 2004 by CEA LIST
 * Project     Automaton
 *
 * @brief      this class contains the result of the application of a rule on a text
 *
 *
 ***********************************************************************/

#ifndef RECOGNIZERMATCH_H
#define RECOGNIZERMATCH_H

#include "AutomatonExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "entityProperties.h"
#include "common/misc/fsaStringsPool.h"
#include <vector>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT MatchElement  {
 public:
  MatchElement() {}
  MatchElement(const LinguisticGraphVertex& v, const bool isKept):
    m_elem(std::make_pair(v,isKept)) {}
  ~MatchElement() {}
  bool operator==(const MatchElement& m) {
    return (m_elem.first==m.m_elem.first && m_elem.second==m.m_elem.second);
  }
  const LinguisticGraphVertex& getVertex() const { return m_elem.first; }
  LinguisticGraphVertex& getVertex() { return m_elem.first; }
  bool isKept() const { return m_elem.second; }

  std::pair<LinguisticGraphVertex,bool> m_elem;
};

class RecognizerMatch;
LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream&, const RecognizerMatch&);

class LIMA_AUTOMATON_EXPORT RecognizerMatch :
  public std::vector<MatchElement>,
  public EntityProperties
{
 public:
  RecognizerMatch(const LinguisticAnalysisStructure::AnalysisGraph* graph);
  RecognizerMatch(const LinguisticAnalysisStructure::AnalysisGraph* graph,
                  const LinguisticGraphVertex& vertex,
                  const bool isKept);
  ~RecognizerMatch();

  // comparison operator
  bool operator == (const RecognizerMatch&);

  bool isContiguous() const;
  uint64_t positionBegin() const;
  uint64_t positionEnd() const;
  uint64_t length() const;

  uint64_t numberOfElements() const; // number of *kept* elements

  const LinguisticAnalysisStructure::AnalysisGraph* getGraph() const { return m_graph; }

  LinguisticGraphVertex getEnd() const { return back().getVertex(); }
  LinguisticGraphVertex getBegin() const { return front().getVertex(); }

  bool BeginIs(const LinguisticGraphVertex& pi) const;
  bool EndIs(const LinguisticGraphVertex& pi) const;

  LinguisticAnalysisStructure::Token*
    getToken(RecognizerMatch::iterator) const;
  LinguisticAnalysisStructure::Token*
    getToken(RecognizerMatch::const_iterator) const;

  LinguisticAnalysisStructure::MorphoSyntacticData*
    getData(RecognizerMatch::iterator) const;
  LinguisticAnalysisStructure::MorphoSyntacticData*
    getData(RecognizerMatch::const_iterator) const;

  LinguisticAnalysisStructure::Token*
    getHeadToken() const;
  LinguisticAnalysisStructure::MorphoSyntacticData*
    getHeadData() const;

  void reinit();
  void addBackVertex(const LinguisticGraphVertex&,bool isKept=true);
  void addFrontVertex(const LinguisticGraphVertex&,bool isKept=true);
  void popBackVertex();
  void popFrontVertex();
  void addBack(const RecognizerMatch& l);
  void addFront(const RecognizerMatch& l);
  void removeUnkeptAtExtremity();

  LimaString getString() const;
  LimaString concatString() const { return getString(); } // just an alias

  LimaString getNormalizedString(const FsaStringsPool& sp) const;
  
  // overlap based on positions
  bool isOverlapping(const RecognizerMatch& otherMatch) const;

  friend LIMA_AUTOMATON_EXPORT std::ostream& operator << (std::ostream&, const RecognizerMatch&);
  friend LIMA_AUTOMATON_EXPORT QDebug& operator << (QDebug&, const RecognizerMatch&);
  
 private:
  const LinguisticAnalysisStructure::AnalysisGraph* m_graph;
};

//**********************************************************************
// inline functions
//**********************************************************************

inline bool RecognizerMatch::BeginIs(const LinguisticGraphVertex& pi) const {
  return (front().getVertex()==pi);
}
inline bool RecognizerMatch::EndIs(const LinguisticGraphVertex& pi) const {
  return (back().getVertex()==pi);
}

inline LinguisticAnalysisStructure::Token* RecognizerMatch::
getToken(RecognizerMatch::iterator i) const {
  return get(vertex_token,*(m_graph->getGraph()),(*i).getVertex());
}

inline LinguisticAnalysisStructure::Token* RecognizerMatch::
getToken(RecognizerMatch::const_iterator i) const {
  return get(vertex_token,*(m_graph->getGraph()),(*i).getVertex());
}

inline LinguisticAnalysisStructure::MorphoSyntacticData* RecognizerMatch::
getData(RecognizerMatch::iterator i) const {
  return get(vertex_data,*(m_graph->getGraph()),(*i).getVertex());
}

inline LinguisticAnalysisStructure::MorphoSyntacticData* RecognizerMatch::
getData(RecognizerMatch::const_iterator i) const {
  return get(vertex_data,*(m_graph->getGraph()),(*i).getVertex());
}

inline LinguisticAnalysisStructure::Token* RecognizerMatch::
getHeadToken() const {
  if (m_head==0) { return 0; }
  return get(vertex_token,*(m_graph->getGraph()),m_head);
}
inline LinguisticAnalysisStructure::MorphoSyntacticData* RecognizerMatch::
getHeadData() const {
  if (m_head==0) { return 0; }
  return get(vertex_data,*(m_graph->getGraph()),m_head);
}

} // end namespace
} // end namespace
} // end namespace

#endif
