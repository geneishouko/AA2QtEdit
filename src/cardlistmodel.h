#ifndef CARDLISTMODEL_H
#define CARDLISTMODEL_H

#include <QAbstractListModel>

#include "constants.h"

#include <QSet>

namespace ClassEdit {

    class CardListModel : public QAbstractListModel
    {
        Q_OBJECT

    public:
        CardListModel(QObject *parent = nullptr);
        ~CardListModel();

        CardList &cardList() {
            return m_cardList;
        }
        virtual int columnCount(const QModelIndex &) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        CardList modifiedCardList() const;
        virtual int rowCount(const QModelIndex &) const;

    public slots:
        void addCard(CardFile *card);
        void cardChanged(int index);
        void cardSaved(int index);
        void commitChanges();
        virtual bool save() = 0;
        virtual void saveAll() = 0;

    signals:
        void cardsChanged(int count);
        void notify(const QString &message, int timeout);

    private:
        CardList m_cardList;
        QSet<CardFile*> m_modifiedCardList;
    };

}

#endif // CARDLISTMODEL_H
