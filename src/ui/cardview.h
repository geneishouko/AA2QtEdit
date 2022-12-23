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
        ~CardView() override;

    public slots:
        void characterDataItemSetCheckedState(QListWidgetItem *item, bool checked);
        void characterDataItemClicked(QListWidgetItem *item);
        void characterDataItemsSelectSet(bool select);
        void characterDataItemsSelect();
        void characterDataItemsUnselect();
        void characterDataEnableImportButton();
        void modelItemSelected(const QModelIndex &index);
        void modelItemUpdated(const QModelIndex &index);
        void invalidateProxyModels();
        void importCard();
        void exportCard();
        void exportPortrait();
        void exportThumbnail();
        void importCloth();
        void exportCloth();
        void copyCloth();
        void pasteCloth();
        void replaceFacePNG();
        void replaceRosterPNG();
        void dictionaryEntryChanged(int index);
        void dictionaryEntryWidgetChanged();

    protected:
        void registerDictionaryEntryWidget(QWidget *widget);
        void updateDictionaryEntryWidgets();
        void updateDictionaryEntryWidget(QWidget *widget, const QVariant &value);

    private:
        Ui::CardView *ui;
        QPointer<CardFile> m_card;
        QSortFilterProxyModel *m_cardDataSortFilterModel;
        QSortFilterProxyModel *m_cardPlayDataSortFilterModel;
        QSet<QString> m_dictionaryEntryLocker;
        QHash<QString, QWidget*> m_dictionaryEntryWidgets;
        QVariantMap m_clothClipboard;
    };

}

#endif // CARDVIEW_H
