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
 * @file       EntityType.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Jan 22 2007
 * copyright   Copyright (C) 2007-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "EntityType.h"
#include "common/LimaCommon.h"

namespace Lima {
namespace Common {
namespace MediaticData {

class EntityTypePrivate
{
  friend class EntityType;
  friend std::ostream& operator << (std::ostream&, const EntityType&);
  friend QDebug& operator << (QDebug&, const EntityType&);
  
  EntityTypePrivate();
  EntityTypePrivate(const EntityTypePrivate& etp);
  EntityTypePrivate& operator=(const EntityTypePrivate& etp);
  EntityTypePrivate(EntityTypeId id, EntityGroupId groupId);
  ~EntityTypePrivate();

  EntityTypeId m_id;
  EntityGroupId m_groupId;
};

EntityTypePrivate::EntityTypePrivate():
m_id(0),
m_groupId(0)
{
}

EntityTypePrivate::EntityTypePrivate(const EntityTypePrivate& etp):
m_id(etp.m_id),
m_groupId(etp.m_groupId)
{
}

EntityTypePrivate& EntityTypePrivate::operator=(const EntityTypePrivate& etp)
{
  m_id = etp.m_id;
  m_groupId = etp.m_groupId;
  return *this;
}

EntityTypePrivate::EntityTypePrivate(EntityTypeId id, EntityGroupId groupId):
m_id(id),
m_groupId(groupId)
{
}

EntityTypePrivate::~EntityTypePrivate()
{
}

//***********************************************************************
// constructors and destructors
EntityType::EntityType():
m_d(new EntityTypePrivate())
{
}

EntityType::EntityType(const EntityType& et):
m_d(new EntityTypePrivate(*et.m_d))
{
}

EntityType& EntityType::operator=(const EntityType& et)
{
  *m_d = *et.m_d;
  return *this;
}

EntityType::EntityType(EntityTypeId id, EntityGroupId groupId):
m_d(new EntityTypePrivate(id, groupId))
{
}

EntityType::~EntityType() 
{
  delete m_d;
}

bool EntityType::operator==(const EntityType& other) const 
{
  return (m_d->m_groupId==other.m_d->m_groupId && m_d->m_id==other.m_d->m_id);
}

bool EntityType::operator!=(const EntityType& other) const 
{
  return !(operator==(other));
}

bool EntityType::operator<(const EntityType& other) const
{
  if (m_d->m_groupId < other.m_d->m_groupId) return true;
  if (m_d->m_groupId == other.m_d->m_groupId)
    if (m_d->m_id < other.m_d->m_id) return true;
    return false;
}

bool EntityType::isNull() const { return (m_d->m_id==0 && m_d->m_groupId==0); }

EntityTypeId EntityType::getTypeId() const { return m_d->m_id; }
EntityGroupId EntityType::getGroupId() const { return m_d->m_groupId; }

void EntityType::setTypeId(EntityTypeId id) { m_d->m_id=id; }
void EntityType::setGroupId(EntityGroupId groupId) { m_d->m_groupId=groupId; }

std::ostream& operator << (std::ostream& os, const EntityType& type) {
  return os << type.m_d->m_groupId << "." << type.m_d->m_id;
}

QDebug& operator << (QDebug& os, const EntityType& type) {
  return os << type.m_d->m_groupId << "." << type.m_d->m_id;
}

} // end namespace
} // end namespace
} // end namespace
