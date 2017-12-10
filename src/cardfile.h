#ifndef CARDFILE_H
#define CARDFILE_H

#include <QBuffer>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QSet>
#include <QString>
#include <QVariant>

namespace ClassEdit {

    class DataReader;
    class CardDataModel;
    class Dictionary;
    class PngImage;

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

        void init(QIODevice *file, qint64 startOffset, qint64 endOffset);
        int loadPlayData(QIODevice *file, int offset);
        CardDataModel* getEditDataModel() const;

        QString fileName() const;
        QVector<QString> getEditDataKeys() const;
        QVariant getAddress(const QString &key) const;
        QVariant getType(const QString &key) const;
        QVariant getValue(const QString &key) const;
        QPixmap getFace() const;
        QPixmap getRoster() const;
        QString getFullName() const;
        QDateTime getModifiedTime() const;
        void setModifiedTime(const QDateTime &date);
        void setValue(const QString &key, const QVariant &value);
        bool hasPendingChanges() const;
        void commitChanges();

    public slots:
        void saveChanges();

    signals:
        void changed(int index);

    private:
        QString m_filePath;
        QString m_fileName;
        QDateTime m_modifiedTime;
        QBuffer m_editDataIO;
        DataReader* m_editDataReader;
        DataReader* m_aauDataReader;
        DataReader* m_playDataReader;
        QByteArray m_aauData;
        qint32 m_aauDataVersion;
        QByteArray m_editData;
        Dictionary* m_editDataDictionary;
        CardDataModel* m_editDataModel;


        QString m_fullName;
        PngImage* m_face;
        PngImage* m_roster;

        int m_modelIndex;
        QSet<QString> m_dirtyKeyValues;
    };

}

#endif // CARDFILE_H
