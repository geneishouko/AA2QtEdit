#include "cardlistmodel.h"

#include "cardfile.h"

using namespace ClassEdit;

CardListModel::CardListModel(QObject *parent) :
    QAbstractListModel (parent)
{
}

CardListModel::~CardListModel()
{
    for (auto it = m_cardList.begin(); it != m_cardList.end(); it++)
    {
        delete *it;
    }
    m_cardList.clear();
}

void CardListModel::addCard(CardFile *card)
{
    card->setParent(this);
    card->setModelIndex(static_cast<int>(m_cardList.size()));
    connect(card, &CardFile::changed, this, &CardListModel::cardChanged);
    m_cardList << card;
    emit notify(QString(tr("Loaded %1 cards").arg(m_cardList.size())), 5000);
}

void CardListModel::cardChanged(int cardIndex)
{
    m_modifiedCardList << m_cardList[cardIndex];
    emit cardsChanged(static_cast<int>(m_modifiedCardList.size()));
    const QModelIndex &modelIndex = index(cardIndex);
    emit dataChanged(modelIndex, modelIndex);
}

void CardListModel::cardSaved(int index)
{
    m_modifiedCardList.remove(m_cardList[index]);
    emit cardsChanged(static_cast<int>(m_modifiedCardList.size()));
}

void CardListModel::commitChanges()
{
    foreach(CardFile *card, m_modifiedCardList) {
        card->commitChanges();
        const QModelIndex &modelIndex = index(card->modelIndex());
        emit dataChanged(modelIndex, modelIndex);
    }
}

void CardListModel::failedSave()
{
    emit notify(tr("Failed to save file"), 5000);
}

QVariant CardListModel::data(const QModelIndex &index, int role) const
{
    CardFile *card = m_cardList[index.row()];
    switch (role) {
        case Qt::DisplayRole:
            return card->fullName();
        case Qt::DecorationRole:
            return card->thumbnailPixmap();
        case CardFileRole:
            return QVariant::fromValue<CardFile*>(card);
        case CardModifiedTimeRole:
            return card->modifiedTime();
        case CardSeatRole:
            return card->seat();
        case CardHasModifications:
            return m_modifiedCardList.contains(card);
    }
    return QVariant();
}

CardList CardListModel::modifiedCardList() const
{
    CardList cards;
    for(QSet<CardFile*>::ConstIterator it = m_modifiedCardList.begin(); it != m_modifiedCardList.end(); it++) {
        cards << *it;
    }
    return cards;
}

int CardListModel::rowCount(const QModelIndex &) const
{
    return static_cast<int>(m_cardList.size());
}

int CardListModel::columnCount(const QModelIndex &) const
{
    return 1;
}
