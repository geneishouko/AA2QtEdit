#ifndef CARDDATAVIEW_H
#define CARDDATAVIEW_H

#include <QTreeView>

namespace ClassEdit {

    class CardDataView : public QTreeView
    {
    public:
        CardDataView(QWidget *parent = nullptr);

    public slots:
        void toggleCollapse(const QModelIndex &index);

    protected:
        void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;
    };

}

#endif // CARDDATAVIEW_H
