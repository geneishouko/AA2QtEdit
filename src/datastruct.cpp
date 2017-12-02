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

#include "datastruct.h"

using namespace ClassEdit;

DataStruct::DataStruct()
{

}

QVector<DataBlock> DataStruct::blocks() const
{
    return m_blocks;
}

void DataStruct::registerDataBlock(const QXmlStreamAttributes &attributes)
{
    m_blocks << DataBlock(-1, attributes);
}
