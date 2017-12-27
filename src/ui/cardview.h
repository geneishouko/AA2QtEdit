#ifndef CARDVIEW_H
#define CARDVIEW_H

#include <QListWidgetItem>
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
        void characterDataItemSetCheckedState(QListWidgetItem *item, bool checked);
        void characterDataItemClicked(QListWidgetItem *item);
        void characterDataItemsSelectSet(bool select);
        void characterDataItemsSelect();
        void characterDataItemsUnselect();
        void characterDataEnableImportButton();
        void modelItemSelected(const QModelIndex &index);
        void modelItemUpdated(const QModelIndex &index);
        void exportCard();
        void exportPortrait();
        void exportThumbnail();
        void importCloth();
        void importCard();
        void replaceFacePNG();
        void replaceRosterPNG();

    signals:
        void setEditDataValue(const QString &key, const QVariant &value);

    protected:
        bool eventFilter(QObject *watched, QEvent *event);

    private:
        Ui::CardView *ui;
        QPointer<CardFile> m_card;
        QSortFilterProxyModel *m_cardDataSortFilterModel;
        int m_setText;
    };

}

#endif // CARDVIEW_H
