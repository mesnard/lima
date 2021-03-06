/*
 *    Copyright 2002-2013 CEA LIST
 * 
 *    This file is part of LIMA.
 * 
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 * 
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
#ifndef LIMA_PELF_RESOURCEEDITORTABLEMODEL_H
#define LIMA_PELF_RESOURCEEDITORTABLEMODEL_H

#include <QtCore/QtDebug>
#include <QtCore/QFile>

#include "ResourceReaderTableModel.h"
#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class ResourceEditorTableModel : public ResourceReaderTableModel
{

Q_OBJECT

public:

    ResourceEditorTableModel ();
    void init (QString rp, QString ic);
    void saveData ();
    void install ();
    void addEntry ();
    void deleteEntries (QModelIndexList indexList);

public slots:

    void addEntry (AbstractResourceEntry* de);

private:

    QString editedResourcePath;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEEDITORTABLEMODEL_H
