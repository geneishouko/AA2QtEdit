#ifndef CARDFILE_H
#define CARDFILE_H

#include "dictionary.h"

#include "constants.h"

#include <QAbstractItemModel>
#include <QBuffer>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QSet>
#include <QString>
#include <QVariant>

namespace ClassEdit {

    class CardDataModel;
    class ClothData;
    class DataReader;

    enum CardDataRoles {
        CardFileRole = 0x100,
        CardNameRole = 0x110,
        CardModifiedTimeRole = 0x111,
        CardSeatRole = 0x112,
        CardHasModifications = 0x113,
    };

    class CardFile : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(CardDataModel* editDataModel READ editDataModel)

    public:
        CardFile();
        CardFile(const QFileInfo &file);
        CardFile(QIODevice *file, qint64 startOffset, qint64 endOffset);
        ~CardFile();

        inline const QByteArray aauData() const {
            return m_editDataDictionary->value(AAUDataKey).toByteArray();
        }
        inline int aauDataVersion() const {
            return m_aauDataVersion;
        }
        void commitChanges();
        inline const QByteArray &editData() const {
            return m_editData;
        }
        inline CardDataModel *editDataModel() const {
            return m_editDataModel;
        }
        inline Dictionary *editDictionary() {
            return m_editDataDictionary;
        }
        inline const QString &fileName() const {
            return m_fileName;
        }
        inline const QString &filePath() const {
            return m_filePath;
        }
        inline const QString &fullName() const {
            return m_fullName;
        }
        inline int gender() const {
            return m_gender;
        }
        void init(QIODevice *file, qint64 startOffset, qint64 endOffset);
        inline bool isValid() const {
            return m_isValid;
        }
        int loadPlayData(QIODevice *file, int offset);
        inline int modelIndex() const {
            return m_modelIndex;
        }
        inline const QDateTime &modifiedTime() const {
            return m_modifiedTime;
        }
        inline CardDataModel *playDataModel() const {
            return m_playDataModel;
        }
        inline QByteArray portraitData() const {
            return m_editDataDictionary->value(PortraitPngKey).toByteArray();
        }
        QPixmap portraitPixmap();
        //inline void resetPortraitPixmap() {
        //    m_portrait = QPixmap();
        //}
        inline void resetThumbnailPixmap() {
            m_thumbnail = QPixmap();
        }
        bool saveToFile(const QString &file);
        inline int seat() const {
            return m_seat;
        }
        inline void setAAUnlimitedData(const QByteArray &data, int version) {
            m_editDataDictionary->set(AAUDataKey, data);
            //m_aauData = data;
            m_aauDataVersion = version;
        }
        void setClothes(const QString &slot, ClothData *cloth);
        inline void setEditData(const QByteArray &data) {
            m_editData = data;
        }
        inline void setModelIndex(int index) {
            m_modelIndex = index;
        }
        void setPortrait(const QByteArray &file);
        void setPortrait(QIODevice *file);
        inline void setSeat(int seat) {
            m_seat = seat;
        }
        void setThumbnail(const QByteArray &file);
        void setThumbnail(QIODevice *file);
        inline QByteArray thumbnailData() const {
            return m_editDataDictionary->value(ThumbnailPngKey).toByteArray();
        }
        QPixmap thumbnailPixmap();
        void updateQuickInfoGetters();
        void writeToDevice(QIODevice *device, bool writePlayData = false, qint64 *editOffset = nullptr, qint64 *aaudOffset = nullptr) const;

    public slots:
        void dictionaryChanged();

    signals:
        void changed(int index);

    private:
        QString m_filePath;
        QString m_fileName;
        QDateTime m_modifiedTime;
        QByteArray m_editData;
        QByteArray m_playData;
        QBuffer m_editDataIO;
        QBuffer m_playDataIO;
        //QByteArray m_aauData;
        qint32 m_aauDataVersion;
        DataReader *m_editDataReader;
        DataReader *m_playDataReader;
        CardDataModel *m_editDataModel;
        CardDataModel *m_playDataModel;

        QString m_fullName;
        int m_gender;
        int m_seat;
        //QPixmap m_portrait;
        QPixmap m_thumbnail;

        int m_modelIndex;
        Dictionary *m_editDataDictionary;
        Dictionary *m_playDataDictionary;
        bool m_isValid;
    };

}

#endif // CARDFILE_H
