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

#include "integerCost.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

IntegerCost::IntegerCost() :
m_cost(0)
{}

IntegerCost::IntegerCost(uint64_t cost) :
m_cost(cost)
{}

IntegerCost::~IntegerCost() {}

bool IntegerCost::operator<(const IntegerCost& fc) const
{
  return m_cost<fc.m_cost;
}

bool IntegerCost::operator==(const IntegerCost& fc) const
{
  return (m_cost==fc.m_cost);
}

std::ostream& operator<<(std::ostream& out,const IntegerCost& cost)
{
  out << "intcost{" << cost.m_cost << "}";
  return out;
}


IntegerCost IntegerCost::operator+(const IntegerCost& fc) const
{
  return IntegerCost(m_cost+fc.m_cost);
}

IntegerCost& IntegerCost::operator+=(const IntegerCost& fc)
{
  m_cost+=fc.m_cost;
  return *this;
}



IntegerCostFunction::IntegerCostFunction()
{}

IntegerCostFunction::IntegerCostFunction(
  const IntegerCostFunction& cf)
{}

IntegerCostFunction::~IntegerCostFunction() {}

IntegerCost IntegerCostFunction::operator()(Lima::LinguisticCode cat1, Lima::LinguisticCode cat2, Lima::LinguisticCode cat3) const
{
  return IntegerCost(0);
}

} // PosTagger
} // LinguisticProcessing
}// Lima
