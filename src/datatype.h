#ifndef DATATYPE_H
#define DATATYPE_H

#include <QString>
#include <QVariant>

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

    DataType stringToDataType(const QString &string);

    inline void dataTypeToSize(DataType type, int &size) {
        if (type == DataType::Bool || type == DataType::Byte)
            size = 1;
        else if (type == DataType::Int16)
            size = 2;
        else if (type == DataType::Int32 || type == DataType::Color)
            size = 4;
    }

    inline bool operator==(DataType dt, QVariant::Type vt) {
        switch (dt) {
        case Bool:
            return vt == QVariant::Bool;
        case Byte:
        case Int16:
        case Int32:
        case Color:
        case Enum:
            return vt == QVariant::Int;
        case String:
        case EncodedString:
            return vt == QVariant::String;
        }
        return false;
    }

    inline bool operator!=(DataType dt, QVariant::Type vt) {
        return !(dt == vt);
    }
}

#endif // DATATYPE_H
