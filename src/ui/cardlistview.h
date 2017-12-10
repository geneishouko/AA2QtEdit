#ifndef CARDLISTVIEW_H
#define CARDLISTVIEW_H

#include <QListView>

namespace ClassEdit {

    class CardListDelegate;

    class CardListView : public QListView
    {
    public:
        CardListView(QWidget *parent = nullptr);
        void mouseMoveEvent(QMouseEvent *event);

    private:
        CardListDelegate *m_delegate;
    };

}

#endif // CARDLISTVIEW_H
