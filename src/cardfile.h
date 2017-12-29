#ifndef CARDFILE_H
#define CARDFILE_H

#include "dictionary.h"

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
        CardSeatRole = 0x112
    };

    class CardFile : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(CardDataModel* editDataModel READ getEditDataModel)

    public:
        CardFile();
        CardFile(const QFileInfo &file);
        CardFile(QIODevice *file, qint64 startOffset, qint64 endOffset);
        ~CardFile();

        void setParentModel(QAbstractItemModel *model);
        void init(QIODevice *file, qint64 startOffset, qint64 endOffset);
        int loadPlayData(QIODevice *file, int offset);
        bool isValid() const;

        QByteArray aauData() const;
        int aauDataVersion() const;
        QByteArray editData() const;
        QByteArray portraitData() const;
        QByteArray thumbnailData() const;
        QString fileName() const;
        QString filePath() const;
        CardDataModel *getEditDataModel() const;
        CardDataModel *getPlayDataModel() const;
        Dictionary *editDictionary();
        int getGender() const;
        QPixmap portraitPixmap();
        QPixmap getRoster();
        inline int getModelIndex() const {
            return m_modelIndex;
        }
        QString fullName() const;
        QDateTime modifiedTime() const;
        inline void resetPortraitPixmap() {
            m_face = QPixmap();
        }
        inline void resetThumbnailPixmap() {
            m_roster = QPixmap();
        }
        int seat() const;
        void setAAUnlimitedData(const QByteArray &data, int version);
        void setClothes(const QString &slot, ClothData *cloth);
        void setEditData(const QByteArray &data);
        void setModelIndex(int index);
        void setModifiedTime(const QDateTime &date);
        void setSeat(int seat);
        void setFace(const QByteArray &file);
        void setFace(QIODevice *file);
        void setRoster(const QByteArray &file);
        void setRoster(QIODevice *file);

        void buildEditDictionary();
        void updateQuickInfoGetters();
        bool hasPendingChanges() const;
        void commitChanges();
        void save();
        void saveToFile(const QString &file);
        void writeToDevice(QIODevice *device, bool writePlayData = false, qint64 *editOffset = nullptr, qint64 *aaudOffset = nullptr);

    public slots:
        //void setEditDataValue(const QString &key, const QVariant &value);
        void dictionaryChanged();

    signals:
        void changed(int index);
        void saved(int index);

    private:
        QString m_filePath;
        QString m_fileName;
        QDateTime m_modifiedTime;
        QByteArray m_editData;
        QByteArray m_playData;
        QBuffer m_editDataIO;
        QBuffer m_playDataIO;
        QByteArray m_aauData;
        qint32 m_aauDataVersion;
        DataReader *m_editDataReader;
        DataReader *m_aauDataReader;
        DataReader *m_playDataReader;
        CardDataModel *m_editDataModel;
        CardDataModel *m_playDataModel;
        QAbstractItemModel *m_parentModel;

        QString m_fullName;
        int m_gender;
        int m_seat;
        QPixmap m_face;
        QPixmap m_roster;

        int m_modelIndex;
        Dictionary *m_editDataDictionary;
        Dictionary *m_playDataDictionary;
        bool m_isValid;
    };

}

#endif // CARDFILE_H
