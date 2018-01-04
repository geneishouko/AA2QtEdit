#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace ClassEdit {
    class CardFile;
    typedef QList<CardFile*> CardList;

    extern const QString ClothSlotUniformKey;
    extern const QString ClothSlotSportKey;
    extern const QString ClothSlotSwimsuitKey;
    extern const QString ClothSlotClubKey;
    extern const QString PortraitPngKey;
    extern const QString ThumbnailPngKey;
    extern const qint32 EditDataLength;
}

#endif // CONSTANTS_H
