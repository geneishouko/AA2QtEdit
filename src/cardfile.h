#ifndef CARDFILE_H
#define CARDFILE_H

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

    class ClothData;
    class DataReader;
    class CardDataModel;
    class Dictionary;

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

        QString fileName() const;
        QString filePath() const;
        CardDataModel *getEditDataModel() const;
        QVector<QString> getEditDataKeys() const;
        QVariant getEditDataAddress(const QString &key) const;
        QVariant getEditDataType(const QString &key) const;
        QVariant getEditDataValue(const QString &key) const;
        int getGender() const;
        QPixmap getFace();
        QPixmap getRoster();
        QString fullName() const;
        QDateTime modifiedTime() const;
        void replaceCard(const QString &file);
        int seat() const;
        void setClothes(int slot, ClothData *cloth);
        void setModelIndex(int index);
        void setModifiedTime(const QDateTime &date);
        void setSeat(int seat);
        void setFace(const QByteArray &file);
        void setFace(QIODevice *file);
        void setRoster(const QByteArray &file);
        void setRoster(QIODevice *file);

        void updateQuickInfoGetters();
        bool hasPendingChanges() const;
        void commitChanges();
        void save();
        void saveToFile(const QString &file);
        void writeToDevice(QIODevice *device, bool writePlayData = false, qint64 *editOffset = nullptr, qint64 *aaudOffset = nullptr);

    public slots:
        void setEditDataValue(const QString &key, const QVariant &value);

    signals:
        void changed(int index);

    private:
        QString m_filePath;
        QString m_fileName;
        QDateTime m_modifiedTime;
        QByteArray m_editData;
        QByteArray m_playData;
        QBuffer m_editDataIO;
        QByteArray m_aauData;
        qint32 m_aauDataVersion;
        DataReader *m_editDataReader;
        DataReader *m_aauDataReader;
        DataReader *m_playDataReader;
        CardDataModel *m_editDataModel;
        QAbstractItemModel *m_parentModel;

        QString m_fullName;
        int m_gender;
        int m_seat;
        QPixmap m_face;
        QPixmap m_roster;

        int m_modelIndex;
        Dictionary *m_editDataDictionary;
        QSet<QString> m_dirtyKeyValues;
        bool m_isValid;
    };

}

#endif // CARDFILE_H
