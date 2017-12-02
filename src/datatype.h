#ifndef DATATYPE_H
#define DATATYPE_H

#include <QString>

namespace ClassEdit {

    enum DataType {
        Invalid,
        Byte,
        Int16,
        Int32,
        Bool,
        Color,
        Enum,
        String,
        EncodedString,
        Struct,
        Seat,
        Array,
        Dummy,
    };

    DataType stringToDataType(const QString& string);

    inline void dataTypeToSize(DataType type, int &size) {
        if (type == DataType::Bool || type == DataType::Byte)
            size = 1;
        else if (type == DataType::Int16)
            size = 2;
        else if (type == DataType::Int32 || type == DataType::Color)
            size = 4;
    }

}

#endif // DATATYPE_H
