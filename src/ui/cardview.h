#ifndef CARDVIEW_H
#define CARDVIEW_H

#include <QPointer>
#include <QSortFilterProxyModel>
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

        void lineEditChanged(const QString &newText);
        void updateDataControls();

    public slots:
        void modelItemSelected(const QModelIndex &index);
        void modelItemUpdated(const QModelIndex &index);
        void importCloth();
        void extractCard();
        void replaceCard();
        void replaceFacePNG();
        void replaceRosterPNG();

    signals:
        void setEditDataValue(const QString &key, const QVariant &value);

    private:
        Ui::CardView *ui;
        QPointer<CardFile> m_card;
        QSortFilterProxyModel *m_cardDataSortFilterModel;
    };

}

#endif // CARDVIEW_H
