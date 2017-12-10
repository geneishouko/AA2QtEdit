/*
    This file is part of AA2QtEdit.

    AA2QtEdit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AA2QtEdit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AA2QtEdit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "clothdata.h"
#include "datareader.h"
#include "dictionary.h"

#include <QHash>

using namespace ClassEdit;

ClothData::ClothData(QObject *parent) :
    QObject(parent)
{
    m_dataReader = DataReader::getDataReader("clothdata");
    m_dictionary = new Dictionary(this);
}

ClothData *ClothData::fromCardData(const QString &type, CardFile *card)
{
    ClothData *data = new ClothData;
    DataReader::DataBlockList &blockList = data->m_dataReader->m_dataBlocks;
    for (DataReader::DataBlockList::const_iterator it = blockList.constBegin(); it != blockList.constEnd(); it++) {
        data->m_dictionary->insert((*it)->key(), card->getEditDataValue((*it)->key().replace("CLOTH", type)));
    }
    return data;
}

ClothData *ClothData::fromClothFile(const QString &path)
{
    ClothData *data = new ClothData;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        DataReader::DataBlockList &blockList = data->m_dataReader->m_dataBlocks;
        for (DataReader::DataBlockList::const_iterator it = blockList.constBegin(); it != blockList.constEnd(); it++) {
            data->m_dictionary->insert((*it)->key(), data->m_dataReader->read(&file, (*it)->key()));
        }
    }
    return data;
}

QVariant ClothData::getValue(const QString &key) const
{
    return m_dictionary->value(key);
}

QHash<QString,QVariant> ClothData::getValues(const QString &type) const
{
    QHash<QString,QVariant> list;
    QString key;
    DataReader::DataBlockList &blockList = m_dataReader->m_dataBlocks;
    for (DataReader::DataBlockList::const_iterator it = blockList.constBegin(); it != blockList.constEnd(); it++) {
        key = (*it)->key().replace("CLOTH", type);
        list.insert(key, m_dictionary->value((*it)->key()));
    }
    return list;
}

void ClothData::setValue(const QString &key, const QVariant &value)
{
    m_dictionary->insert(key, value);
}

QByteArray ClothData::toClothFile() const
{
    QByteArray data;
    if (m_dictionary->isEmpty())
        return data;
    QBuffer buffer(&data);
    foreach(DataBlock *db, m_dataReader->m_dataBlocks) {
        buffer.seek(db->offset());
        m_dataReader->write(&buffer, db->key(), m_dictionary->value(db->key()));
    }
    return data;
}
