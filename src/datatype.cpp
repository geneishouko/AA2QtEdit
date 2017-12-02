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

#include "datatype.h"

#include <QHash>

using namespace ClassEdit;

static QHash<QString, DataType> dataTypeNames;

DataType ClassEdit::stringToDataType(const QString& string) {
    if (dataTypeNames.empty()) {
        dataTypeNames["byte"] = Byte;
        dataTypeNames["int16"] = Int16;
        dataTypeNames["int32"] = Int32;
        dataTypeNames["bool"] = Bool;
        dataTypeNames["color"] = Color;
        dataTypeNames["enum"] = Enum;
        dataTypeNames["string"] = String;
        dataTypeNames["encodedstring"] = EncodedString;
        dataTypeNames["struct"] = Struct;
        dataTypeNames["seat"] = Seat;
        dataTypeNames["array"] = Array;
        dataTypeNames["dummy"] = Dummy;
    }
    DataType type = dataTypeNames[string];
    return type;
}
