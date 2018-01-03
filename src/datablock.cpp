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

#include "datablock.h"

using namespace ClassEdit;

DataBlock::DataBlock() :
    m_isValid(true),
    m_type(Invalid),
    m_address(-1),
    m_minValue(0),
    m_maxValue(0),
    m_warnValue(0),
    m_copyAddress(-1),
    m_dataSize(-1),
    m_readOnly(false),
    m_arrayInnerType(Invalid),
    m_arrayHeaderType(Invalid),
    m_arraySize(-1)
{
}

DataBlock::DataBlock(int currentOffset, QXmlStreamAttributes attributes) : DataBlock()
{
    setAttributes(currentOffset, attributes);
}

void DataBlock::setAttributes(int currentOffset, QXmlStreamAttributes attributes)
{
    bool ok = true;
    foreach (const QXmlStreamAttribute& attr, attributes) {
        const QStringRef &key = attr.name();
        QStringRef sval;

        if (key == "key") {
            m_key = attr.value().toString();
        }

        else if (key == "type") {
            m_type = stringToDataType(attr.value().toString());
            if (m_type == DataType::Bool || m_type == DataType::Byte)
                m_dataSize = 1;
            else if (m_type == DataType::Int16)
                m_dataSize = 2;
            else if (m_type == DataType::Int32 || m_type == DataType::Color)
                m_dataSize = 4;
        }

        else if (key == "address") {
            sval = attr.value();
            if (sval == "auto") {
                m_address = currentOffset;
            }
            else {
                m_address = sval.toInt(&ok, 16);
            }
        }

        else if (key == "displayname") {
            m_displayName = attr.value().toString();
        }

        else if (key == "datasize") {
            m_dataSize = attr.value().toInt(&ok);
        }

        else if (key == "metakey") {
            m_metaKey = attr.value().toString();
        }

        else if (key == "headtype") {
            m_arrayHeaderType = stringToDataType(attr.value().toString());
        }

        else if (key == "innertype") {
            m_arrayInnerType = stringToDataType(attr.value().toString());
        }

        else if (key == "arraysize") {
            m_arraySize = attr.value().toInt();
        }

        else if (key == "copyaddress")
            m_copyAddress = attr.value().toInt(&ok, 16);
    }

    if (!ok)
        m_isValid = false;
}

QString DataBlock::key() const
{
    return m_key;
}

int DataBlock::offset() const
{
    return m_address;
}

int DataBlock::nextOffset() const
{
    if (m_type == DataType::Array || m_type == DataType::Struct) {
        if (m_children.isEmpty()) {
            int size;
            dataTypeToSize(m_arrayHeaderType, size);
            return m_address + size;
        }
        else
            return m_children.last().nextOffset();
    }
    if (m_address < 0)
        return m_address;
    return m_address + m_dataSize;
}

int DataBlock::dataSize() const
{
    return m_dataSize;
}

DataType DataBlock::type() const
{
    return m_type;
}

QString DataBlock::typeName() const
{
    switch (m_type) {
        case Invalid:
            return QObject::tr("Invalid");
        case Byte:
            return QObject::tr("Byte");
        case Int16:
            return QObject::tr("Int16");
        case Int32:
            return QObject::tr("Int32");
        case Bool:
            return QObject::tr("Bool");
        case Color:
            return QObject::tr("Color");
        case Enum:
            return QObject::tr("Enum");
        case String:
        case EncodedString:
            return QObject::tr("String");
        case Struct:
            return QObject::tr("Struct");
        case Array:
            return QObject::tr("Array");
        case Dummy:
            return QObject::tr("Dummy");
    }
    return QString();
}

QString DataBlock::metaKey() const
{
    return m_metaKey;
}
