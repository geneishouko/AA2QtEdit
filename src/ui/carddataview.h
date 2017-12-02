#ifndef CARDDATAVIEW_H
#define CARDDATAVIEW_H

#include <QTreeView>

namespace ClassEdit {

    class CardDataView : public QTreeView
    {
    public:
        CardDataView(QWidget *parent = nullptr);
    };

}

#endif // CARDDATAVIEW_H
