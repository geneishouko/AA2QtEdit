#ifndef CARDLISTVIEW_H
#define CARDLISTVIEW_H

#include <QListView>

#include <QFileInfo>

namespace ClassEdit {

    class CardListDelegate;

    class CardListView : public QListView
    {
        Q_OBJECT

    public:
        CardListView(QWidget *parent = nullptr);
        void mouseMoveEvent(QMouseEvent *event);

    protected:
        void dragEnterEvent(QDragEnterEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void dropEvent(QDropEvent *event);

    signals:
        void droppedFiles(QFileInfoList);

    private:
        CardListDelegate *m_delegate;
    };

}

#endif // CARDLISTVIEW_H
