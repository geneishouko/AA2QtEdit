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

#include "datareader.h"
#include "dictionary.h"

#include <QColor>
#include <QFile>
#include <QStringBuilder>
#include <QTextCodec>

#include <QtDebug>

using namespace ClassEdit;

QHash<QString, DataReader*> DataReader::s_readers;
QHash<QString, DataEnumerable*> DataReader::s_enumerables;

DataReader::DataReader(QIODevice *xmlDefinition)
{
    loadDefinitions(xmlDefinition);
}

DataReader *DataReader::getDataReader(const QString &name)
{
    QHash<QString, DataReader*>::iterator it = s_readers.find(name);
    if (it != s_readers.end()) {
        return *it;
    }
    qDebug() << "load definitions" << name;
    QFile xml(QString(":/XML/" % name % ".xml"));
    xml.open(QFile::ReadOnly);
    DataReader *reader = new DataReader(&xml);
    s_readers.insert(name, reader);
    return reader;
}

QVariant DataReader::read(QIODevice *data, DataType type, int dataSize) const
{
    QVariant ret;

    if (type == DataType::String || type == DataType::EncodedString) {
        QByteArray result = data->read(dataSize);
        if (type == DataType::EncodedString)
            result = decodeString(result);
#ifdef Q_OS_WIN
        else
            //truncate null-terminators in Windows to remove mojibake
            result.truncate(result.indexOf('\0', 0));
#endif
        QTextStream dec(&result);
        dec.setCodec("Shift-JIS");
        ret = dec.readAll();
    }

    else if (type == DataType::Bool) {
        char c;
        data->read(&c, 1);
        ret = static_cast<bool>(c);
    }

    else if (type == DataType::Byte) {
        qint32 c = 0;
        data->read(reinterpret_cast<char*>(&c), 1);
        ret = c;
    }

    else if (type == DataType::Int16) {
        qint16 c;
        data->read(reinterpret_cast<char*>(&c), 2);
        ret = static_cast<int>(c);
    }

    else if (type == DataType::Int32) {
        qint32 c;
        data->read(reinterpret_cast<char*>(&c), 4);
        ret = static_cast<int>(c);
    }
    else if (type == DataType::Color) {
        quint32 c;
        data->read(reinterpret_cast<char*>(&c), 4);
        ret = QColor::fromRgba(c);
    }
    else if (type == DataType::Dummy) {
        ret = data->read(dataSize);
    }

    return ret;
}

QVariant DataReader::read(QIODevice *data, DataBlock *db) const
{
    DataType type = db->type();
    int size = db->dataSize();
    data->seek(db->offset());

    if (type == DataType::Enum) {
        type = s_enumerables[db->metaKey()]->type();
        size = db->dataSize();
    }

    if (type == DataType::Array || type == DataType::Struct) {
        Dictionary *dict = new Dictionary;
        QList<DataBlock*> dictBlocks;
        dictBlocks.reserve(db->m_children.size());
        for (QVector<DataBlock>::Iterator it = db->m_children.begin(); it != db->m_children.end(); it++) {
            dict->insert(it->key(), read(data, it));
            dictBlocks.append(it);
        }
        dict->setDataBlockList(dictBlocks);
        dict->buildDisplayKeyList();
        return QVariant::fromValue(dict);
    }

    return read(data, type, size);
}

void DataReader::write(QIODevice *data, const QString &key, const QVariant &value) const
{
    DataBlock *db = m_dataBlockMap[key];
    DataType type = db->type();
    if (type == DataType::Enum) {
        type = s_enumerables[db->metaKey()]->type();
    }

    int fieldSize = 0;
    if (type == DataType::String || type == DataType::EncodedString)
        fieldSize = db->dataSize();

    write(data, type, value, fieldSize);

    if (db->m_copyAddress > -1) {
        data->seek(db->m_copyAddress);
        write(data, type, value, fieldSize);
    }
}

void DataReader::write(QIODevice *data, DataType type, const QVariant &value, int fieldSize) const
{
    // Enum type must be resolved by this point
    Q_ASSERT(type != DataType::Enum);

#ifdef QT_DEBUG
    qint64 pos = data->pos();
#endif

    if (type == DataType::String || type == DataType::EncodedString) {
        QString string = value.toString();
        QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");
        QByteArray array = codec->fromUnicode(string);
        if (type == DataType::EncodedString) {
            array.append(fieldSize - array.size(), '\x00');
            array = decodeString(array);
        }
        data->write(array.data(), fieldSize);
    }

    else if (type == DataType::Bool) {
        char c = value.toBool();
        data->write(&c, 1);
    }

    else if (type == DataType::Byte) {
        int c = static_cast<unsigned char>(value.toInt());
        data->write(reinterpret_cast<char*>(&c), 1);
    }

    else if (type == DataType::Int16) {
        int c = static_cast<qint16>(value.toInt());
        data->write(reinterpret_cast<char*>(&c), 2);
    }

    else if (type == DataType::Int32) {
        int c = static_cast<qint32>(value.toInt());
        data->write(reinterpret_cast<char*>(&c), 4);
    }
    else if (type == DataType::Color) {
        QColor color = value.value<QColor>();
        quint32 c = color.rgba();
        data->write(reinterpret_cast<char*>(&c), 4);
    }
    else if (type == DataType::Dummy) {
        data->write(value.toByteArray());
    }

#ifdef QT_DEBUG
    //qDebug() << "Wrote at" << QString::number(pos, 16) << "now at" << QString::number(data->pos(), 16);
    Q_ASSERT(pos != data->pos());
#endif
}

void DataReader::writeDictionary(QIODevice *data, Dictionary *dictionary) const
{
    const DataBlockList &dataBlocks = dictionary->dataBlocks();
    int size = dataBlocks.size();
    for (int i = 0; i < size; i++) {
        DataType blockType = dataBlocks[i]->type();
        if (blockType == DataType::Array) {
            Dictionary *subDictionary = dictionary->at(i).value<Dictionary*>();
            writeDictionaryArray(data, subDictionary, dataBlocks[i]);
        }
        else if (blockType == DataType::Struct) {
            Dictionary *subDictionary = dictionary->at(i).value<Dictionary*>();
            writeDictionary(data, subDictionary);
        }
        else {
            if (blockType == DataType::Enum) {
                blockType = s_enumerables[dataBlocks[i]->metaKey()]->type();
            }
            write(data, blockType, dictionary->at(i), dataBlocks[i]->dataSize());
        }
    }
}

void DataReader::writeDictionaryArray(QIODevice *data, Dictionary *dictionary, DataBlock *db) const
{
    DataType headerType = db->m_arrayHeaderType;
    DataType innerType = db->m_arrayInnerType;
    int size = db->m_arraySize;
    if (size < 0) {
        size = dictionary->size();
        write(data, headerType, size, 0);
    }
    if (!size)
        return;
    if (innerType == DataType::Enum) {
        innerType = s_enumerables[db->metaKey()]->type();
    }
    if (innerType == DataType::Struct) {
        for (int i = 0; i < size; i++)
            writeDictionary(data, dictionary->at(i).value<Dictionary*>());
    }
    else {
        DataBlock *firstBlock = dictionary->dataBlocks().first();
        for (int i = 0; i < size; i++)
            write(data, innerType, dictionary->at(i), firstBlock->dataSize());
    }

}

DataReader::DataBlockList DataReader::finalizeDataBlocks(QIODevice *data) const
{
    DataBlockList blocks;
    for (DataBlockList::const_iterator it = m_dataBlocks.constBegin(); it != m_dataBlocks.constEnd(); it++) {
        DataBlock *finalBlock = new DataBlock(**it);
        finalizeChildrenDataBlocks(data, finalBlock);
        data->seek(finalBlock->nextOffset());
        blocks << finalBlock;
    }
    return blocks;
}

void DataReader::finalizeDataBlockOffset(DataBlock *db, qint64 offset) const
{
    db->m_address -= offset;
    for (QVector<DataBlock>::Iterator it = db->m_children.begin(); it != db->m_children.end(); it++) {
        finalizeDataBlockOffset(it, offset);
    }
}

void DataReader::finalizeChildrenDataBlocks(QIODevice *data, DataBlock *db) const
{
    if (db->offset() < 0)
        db->m_address = static_cast<int>(data->pos());
    if (db->m_type == DataType::Array) {
        data->seek(db->offset());
        int arraySize = db->m_arraySize;
        if (arraySize == -1) {
            arraySize = read(data, db->m_arrayHeaderType).toInt();
        }

        db->m_children.resize(arraySize);
        int innerSize = 0;
        DataType innerType = db->m_arrayInnerType;
        if (innerType == DataType::Enum)
            innerType = s_enumerables[db->metaKey()]->type();
        switch(innerType) {
            case DataType::Bool:
            case DataType::Byte:
                innerSize = 1;
                break;
            case DataType::Int16:
                innerSize = 2;
                break;
            case DataType::Int32:
            case DataType::Color:
                innerSize = 4;
                break;
            case DataType::Dummy:
                innerSize = db->m_dataSize;
                break;
            case DataType::Struct:
                innerSize = -1;
                break;
            case DataType::Enum:
                innerSize = db->m_dataSize != -1 ? db->m_dataSize : 1;
                break;
            case DataType::String:
            case DataType::EncodedString:
                Q_ASSERT(innerSize);
                break;
            case DataType::Array:
                // Can't have an array of arrays
            case DataType::Invalid:
                // This shoudln't exist
                Q_ASSERT(false);
        }
        int curOffset = static_cast<int>(data->pos());
        for(int i = 0; i < arraySize; i++) {
            if (db->m_arrayInnerType == DataType::Struct) {
                db->m_children[i].m_type = DataType::Struct;
                db->m_children[i].m_address = curOffset;
                DataStruct *ds = getStruct(db->metaKey());

                int blockCount = ds->blocks().size();
                QVector<DataBlock> &blockList = db->m_children[i].m_children;
                blockList.resize(blockCount);
                for (int j = 0; j < blockCount; j++) {
                    blockList[j] = ds->blocks()[j];
                    blockList[j].m_address = curOffset;
                    finalizeChildrenDataBlocks(data, &blockList[j]);
                    curOffset = blockList[j].nextOffset();
                }
            }
            else {
                db->m_children[i].m_type = db->m_arrayInnerType;
                db->m_children[i].m_dataSize = innerSize;
                db->m_children[i].m_address = curOffset + innerSize * i;
                db->m_children[i].m_metaKey = db->m_metaKey;
                if (db->m_arrayInnerType == DataType::Struct)
                    finalizeChildrenDataBlocks(data, &db->m_children[i]);
            }
        }
    }
    else if (db->m_type == DataType::Struct) {
        DataStruct *ds = getStruct(db->metaKey());
        int curOffset = db->offset();
        db->m_children.resize(ds->blocks().size());
        for (int j = 0; j < ds->blocks().size(); j++) {
            db->m_children[j] = ds->blocks()[j];
            db->m_children[j].m_address = curOffset;
            finalizeChildrenDataBlocks(data, &db->m_children[j]);
            curOffset = db->m_children[j].nextOffset();
        }
    }
    else {
        DataType type = db->m_type;
        if (type == DataType::Enum) {
            type = s_enumerables[db->metaKey()]->type();
        }
        if (type == DataType::Bool || type == DataType::Byte)
            db->m_dataSize = 1;
        else if (type == DataType::Int16)
            db->m_dataSize = 2;
        else if (type == DataType::Int32 || type == DataType::Color)
            db->m_dataSize = 4;
    }
}

QByteArray DataReader::decodeString(const QByteArray &data)
{
    QByteArray copy(data);
    for (QByteArray::iterator it = copy.begin(); it != copy.end(); it++) {
        *it = static_cast<char>(((*it) ^ 0xFF) & 0xFF);
        if (*it == 0) {
            copy.truncate(static_cast<int>(it - copy.begin()));
            break;
        }
    }
    return copy;
}

Dictionary *DataReader::buildDictionary(QIODevice *data, DataBlockList dataBlocks) const
{
    Dictionary *dictionary = new Dictionary;
    for (DataReader::DataBlockList::const_iterator it = dataBlocks.constBegin(); it != dataBlocks.constEnd(); it++) {
        QVariant value;
        if ((*it)->m_children.isEmpty()) {
            value = read(data, *it);
        }
        else {
            value = QVariant::fromValue(buildDictionary(data, (*it)->m_children, dictionary));
        }
        dictionary->insert((*it)->key(), value);
    }
    dictionary->buildDisplayKeyList();
    dictionary->setDataBlockList(dataBlocks);
    dictionary->setDataReader(this);
    return dictionary;
}

Dictionary *DataReader::buildDictionary(QIODevice *data, QVector<DataBlock> &dataBlocks, Dictionary *parent) const
{
    Dictionary *dictionary = new Dictionary;
    for (QVector<DataBlock>::Iterator it = dataBlocks.begin(); it != dataBlocks.end(); it++) {
        if (it->m_children.isEmpty())
            dictionary->insert(it->key(), read(data, it));
        else
            dictionary->insert(it->key(), QVariant::fromValue(buildDictionary(data, it->m_children, dictionary)));
    }
    dictionary->buildDisplayKeyList();
    dictionary->setParentDictionary(parent);
    return dictionary;
}

bool DataReader::loadDefinitions(QIODevice *xmlDefinition)
{
    QXmlStreamReader xml(xmlDefinition);
    xml.readNext();
    if(xml.documentVersion() == "1.0") {
        if(xml.readNextStartElement() && xml.name() == "dataset")
            parseDefinitions(xml);
    }
    else {
        xml.raiseError(QObject::tr("The file is not an XML version 1.0 file."));
    }

    return !xml.error();
}

bool DataReader::loadExternalDefinitions(QIODevice *xmlDefinition)
{
    QXmlStreamReader xml(xmlDefinition);
    xml.readNext();
    if(xml.documentVersion() == "1.0") {
        if(xml.readNextStartElement() && xml.name() == "dataset")
            parseExternalDefinitions(xml);
    }
    else {
        xml.raiseError(QObject::tr("The file is not an XML version 1.0 file."));
    }

    return !xml.error();
}

void DataReader::parseDefinitions(QXmlStreamReader &xml) {
    while(xml.readNextStartElement()) {
        if(xml.name() == "enumerables")
            parseEnumerables(xml);
        else if(xml.name() == "structs")
            parseStructs(xml);
        else if (xml.name() == "datablocks")
            parseDatablocks(xml);
        else
            xml.skipCurrentElement();
    }
}

void DataReader::parseExternalDefinitions(QXmlStreamReader &xml)
{
    while(xml.readNextStartElement()) {
        if(xml.name() == "enumerables")
            parseEnumerables(xml);
        else
            xml.skipCurrentElement();
    }
}

void DataReader::parseEnumerables(QXmlStreamReader &xml) {
    QString name, type;
    while (xml.readNextStartElement()) {
        Q_ASSERT(xml.name() == "enumerable");
        name = xml.attributes().value("key").toString();
        type = xml.attributes().value("type").toString();
        if (type.isEmpty())
            type = "byte";
        DataEnumerable *enumerable = registerEnumerable(name, stringToDataType(type));

        int enumKey;
        QString enumValue;
        while(xml.readNextStartElement()) {
            Q_ASSERT(xml.name() == "enumpair");
            enumKey = xml.attributes().value("key").toInt();
            enumValue = xml.attributes().value("value").toString();
            enumerable->registerStringValue(enumKey, enumValue);
            xml.skipCurrentElement();
        }
    }
}

void DataReader::parseStructs(QXmlStreamReader &xml) {
    QString key;
    while (xml.readNextStartElement()) {
        Q_ASSERT(xml.name() == "struct");
        key = xml.attributes().value("key").toString();
        DataStruct *ds = registerStruct(key);
        while(xml.readNextStartElement()) {
            ds->registerDataBlock(xml.attributes());
            xml.skipCurrentElement();
        }
        Q_ASSERT(!ds->blocks().isEmpty());
    }
}

void DataReader::parseDatablocks(QXmlStreamReader &xml) {
    int currentOffset = 0;
    while (xml.readNextStartElement()) {
        Q_ASSERT(xml.name() == "datablock");

        const QXmlStreamAttributes &attr = xml.attributes();
        DataBlock *block = new DataBlock(currentOffset, attr);
        currentOffset = block->nextOffset();

        m_dataBlocks.push_back(block);
        m_dataBlockMap.insert(block->key(), block);
        xml.skipCurrentElement();
    }
}

DataStruct *DataReader::getStruct(const QString &key) const
{
    DataStructMap::const_iterator it = m_structs.find(key);
    Q_ASSERT(it != m_structs.end());
    return *it;
}

DataEnumerable *DataReader::registerEnumerable(const QString &key, DataType type)
{
    DataEnumerableMap::iterator it = s_enumerables.find(key);
    if (it != s_enumerables.end()) {
        return *it;
    }
    DataEnumerable *newEnum = new DataEnumerable(type);
    s_enumerables.insert(key, newEnum);
    return newEnum;
}

DataStruct *DataReader::registerStruct(const QString &key)
{
    DataStructMap::iterator it = m_structs.find(key);
    if (it != m_structs.end()) {
        return *it;
    }
    DataStruct *newStruct = new DataStruct;
    m_structs.insert(key, newStruct);
    return newStruct;
}
