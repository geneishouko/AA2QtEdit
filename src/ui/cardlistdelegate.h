#ifndef CARDLISTDELEGATE_H
#define CARDLISTDELEGATE_H

#include <QStyledItemDelegate>

namespace ClassEdit {

    class CardListDelegate : public QStyledItemDelegate
    {
    public:
        CardListDelegate(QObject *parent = nullptr);
        
        bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        bool needsUpdate(const QPoint &mousePos);

    private:
        QRect saveButtonRect(const QRect &itemRect) const;
    };

}

#endif // CARDLISTDELEGATE_H
