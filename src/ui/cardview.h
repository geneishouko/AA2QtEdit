#ifndef CARDVIEW_H
#define CARDVIEW_H

#include <QTabWidget>

namespace Ui {
class CardView;
}

namespace ClassEdit {

    class CardFile;

    class CardView : public QTabWidget
    {
        Q_OBJECT

    public:
        explicit CardView(QWidget *parent = nullptr);
        ~CardView();

        void setCard(CardFile *card);

    public slots:
        void cardDestroyed();
        void modelItemActivated(const QModelIndex &index);

    private:
        Ui::CardView *ui;
        CardFile *m_card;
    };

}

#endif // CARDVIEW_H
