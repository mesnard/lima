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
 * @file       posTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Oct 14 2002
 * copyright   Copyright (C) 2002 by CEA LIST
 * Project     Automaton
 * 
 * @brief      representation of POS transitions : the transitions are 
 * Parts-Of-Speech
 * 
 ***********************************************************************/

#ifndef POSTRANSITION_H
#define POSTRANSITION_H

#include "AutomatonExport.h"
#include "automatonCommon.h"
#include "transitionUnit.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT PosTransition : public TransitionUnit
{
 public:
  PosTransition(); 
  PosTransition(const Tpos&,
                const Common::PropertyCode::PropertyAccessor* macroAccessor,
                const Common::PropertyCode::PropertyAccessor* microAccessor,
                bool keep=true); 
  PosTransition(const PosTransition&);
  virtual ~PosTransition();
  PosTransition& operator = (const PosTransition&);

  PosTransition* clone() const;
  PosTransition* create() const;

  std::string printValue() const;
  bool operator== (const TransitionUnit&) const;

  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const;

  TypeTransition type() const;

  Tpos pos() const;
  void setPos(Tpos);
  bool comparePos(const LinguisticCode& pos) const;

 private:
  Tpos m_pos;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  
};

/***********************************************************************/
// inline access functions
/***********************************************************************/
inline Tpos PosTransition::pos() const { return m_pos; }
inline void PosTransition::setPos(Tpos s) { m_pos = s; }
inline TypeTransition PosTransition::type() const { return T_POS; }


inline PosTransition* PosTransition::clone() const { 
  return new PosTransition(*this); }
inline PosTransition* PosTransition::create() const { 
  return new PosTransition(); }

} // namespace end
} // namespace end
} // namespace end

#endif
