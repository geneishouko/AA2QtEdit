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
        void mouseMoveEvent(QMouseEvent *event) override;

    protected:
        void dragEnterEvent(QDragEnterEvent *event) override;
        void dragMoveEvent(QDragMoveEvent *event) override;
        void dropEvent(QDropEvent *event) override;

    signals:
        void droppedFiles(QStringList);

    private:
        CardListDelegate *m_delegate;
    };

}

#endif // CARDLISTVIEW_H
