#include "cardlistmodel.h"

#include "cardfile.h"

using namespace ClassEdit;

CardListModel::CardListModel(QObject *parent) :
    QAbstractListModel (parent)
{
}

CardListModel::~CardListModel()
{
}

void CardListModel::addCard(CardFile *card)
{
    card->setParent(this);
    card->setModelIndex(m_cardList.size());
    connect(card, &CardFile::changed, this, &CardListModel::cardChanged);
    connect(card, &CardFile::saved, this, &CardListModel::cardSaved);
    connect(card, &CardFile::saveRequest, this, &CardListModel::submit);
    m_cardList << card;
}

void CardListModel::cardChanged(int cardIndex)
{
    m_modifiedCardList << m_cardList[cardIndex];
    emit cardsChanged(m_modifiedCardList.size());
    const QModelIndex &modelIndex = index(cardIndex);
    emit dataChanged(modelIndex, modelIndex);
}

void CardListModel::cardSaved(int index)
{
    m_modifiedCardList.remove(m_cardList[index]);
    emit cardsChanged(m_modifiedCardList.size());
}

void CardListModel::commitChanges()
{
    foreach(CardFile *card, m_modifiedCardList) {
        card->commitChanges();
        card->updateQuickInfoGetters();
        const QModelIndex &modelIndex = index(card->modelIndex());
        emit dataChanged(modelIndex, modelIndex);
    }
    m_modifiedCardList.clear();
    emit cardsChanged(m_modifiedCardList.size());
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
    return m_cardList.size();
}

int CardListModel::columnCount(const QModelIndex &) const
{
    return 1;
}
