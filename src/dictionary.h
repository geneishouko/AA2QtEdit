#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QHash>
#include <QObject>
#include <QVariant>

namespace ClassEdit {

    class Dictionary : public QObject, public QHash<QString, QVariant>
    {
        Q_OBJECT
    public:
        explicit Dictionary(QObject *parent = nullptr);

    signals:

    public slots:
    };

}

#endif // DICTIONARY_H
