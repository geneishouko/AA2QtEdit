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

    QString dataTypeToString(DataType type);

    inline QMetaType::Type variantType(DataType dt) {
        switch (dt) {
        case Bool:
            return QMetaType::Bool;
        case Byte:
        case Int16:
        case Int32:
        case Enum:
            return QMetaType::Int;
        case Color:
            return QMetaType::QColor;
        case String:
        case EncodedString:
            return QMetaType::QString;
        case Struct:
        case Array:
        case Dummy:
        case Invalid:
            break;
        }
        return QMetaType::UnknownType;
    }

    inline bool operator==(DataType dt, QMetaType::Type vt) {
        return vt == variantType(dt);
    }

    inline bool operator!=(DataType dt, QMetaType::Type vt) {
        return !(dt == vt);
    }
}

#endif // DATATYPE_H
