#ifndef CARDDATADELEGATE_H
#define CARDDATADELEGATE_H

#include <QStyledItemDelegate>

namespace ClassEdit {

    class CardDataDelegate : public QStyledItemDelegate
    {
    public:
        CardDataDelegate(QObject *parent = nullptr);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

}

#endif // CARDDATADELEGATE_H
