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
        ~CardListModel() override;

        CardList &cardList() {
            return m_cardList;
        }
        virtual int columnCount(const QModelIndex &) const override;
        virtual QVariant data(const QModelIndex &index, int role) const override;
        CardList modifiedCardList() const;
        virtual int rowCount(const QModelIndex &) const override;

    public slots:
        void addCard(CardFile *card);
        void cardChanged(int index);
        void cardSaved(int index);
        void commitChanges();
        void failedSave();
        virtual bool save() = 0;
        virtual bool save(const QModelIndex &) = 0;
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
