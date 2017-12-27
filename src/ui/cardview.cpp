/*
    This file is part of AA2QtEdit.

    AA2QtEdit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AA2QtEdit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with AA2QtEdit.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cardview.h"
#include "ui_cardview.h"

#include "../carddatamodel.h"
#include "../cardfile.h"
#include "../clothdata.h"
#include "filedialog.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSaveFile>
#include <QSet>

using namespace ClassEdit;

/* Keep in sync with CardView.ui's QListWidget items */
enum CharacterDataOptions {
    CharacterDataPortrait,
    CharacterDataThumbnail,
    CharacterDataProfile,
    CharacterDataBody,
    CharacterDataTraits,
    CharacterDataPreferences,
    CharacterDataPregnancy,
    CharacterDataCompatibility,
    CharacterDataSuitUniform,
    CharacterDataSuitSports,
    CharacterDataSuitSwimsuit,
    CharacterDataSuitClub,
    CharacterDataAAUnlimited,
    CharacterDataOptionCount
};

CardView::CardView(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::CardView),
    m_card(nullptr),
    m_cardDataSortFilterModel(new QSortFilterProxyModel(this)),
    m_setText(0)
{
    ui->setupUi(this);
    m_cardDataSortFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_cardDataSortFilterModel->setFilterKeyColumn(2);
    ui->editDataView->setModel(m_cardDataSortFilterModel);
    connect(ui->textMakerKeyFilter, &QLineEdit::textChanged, m_cardDataSortFilterModel, &QSortFilterProxyModel::setFilterFixedString);

    connect(ui->PROFILE_FAMILY_NAME, &QLineEdit::textEdited, this, &CardView::lineEditChanged);
    connect(ui->PROFILE_FIRST_NAME, &QLineEdit::textEdited, this, &CardView::lineEditChanged);
    ui->PROFILE_PROFILE->installEventFilter(this);

    connect(ui->characterDataSelectionList, &QListWidget::itemClicked, this, &CardView::characterDataItemClicked);
    connect(ui->selectAllImportsButton, &QPushButton::clicked, this, &CardView::characterDataItemsSelect);
    connect(ui->selectNoneImportsButton, &QPushButton::clicked, this, &CardView::characterDataItemsUnselect);

    for (int i = 0; i < CharacterDataOptionCount; i++) {
        QListWidgetItem *item = ui->characterDataSelectionList->item(i);
        item->setData(Qt::UserRole, QVariant(i));
    }
}

CardView::~CardView()
{
    delete ui;
}

void CardView::lineEditChanged(const QString &newText)
{
    if (m_card) {
        m_card->setEditDataValue(sender()->objectName(), newText);
    }
}

void CardView::updateDataControls()
{
    ui->PROFILE_FAMILY_NAME->setText(m_card->getEditDataValue(ui->PROFILE_FAMILY_NAME->objectName()).toString());
    ui->PROFILE_FIRST_NAME->setText(m_card->getEditDataValue(ui->PROFILE_FIRST_NAME->objectName()).toString());
    if (!m_setText)
        ui->PROFILE_PROFILE->setPlainText(m_card->getEditDataValue(ui->PROFILE_PROFILE->objectName()).toString());
}

void CardView::characterDataItemSetCheckedState(QListWidgetItem *item, bool checked)
{
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
}

void CardView::characterDataItemClicked(QListWidgetItem *item)
{
    bool checked = item->data(Qt::CheckStateRole).toInt() == Qt::Checked;
    characterDataItemSetCheckedState(item, !checked);
    characterDataEnableImportButton();
}

void CardView::characterDataItemsSelectSet(bool select)
{
    int count = ui->characterDataSelectionList->count();
    for (int i = 0; i < count; i++) {
        QListWidgetItem *item = ui->characterDataSelectionList->item(i);
        characterDataItemSetCheckedState(item, select);
    }
    characterDataEnableImportButton();
}

void CardView::characterDataItemsSelect()
{
    characterDataItemsSelectSet(true);
}

void CardView::characterDataItemsUnselect()
{
    characterDataItemsSelectSet(false);
}

void CardView::characterDataEnableImportButton()
{
    int count = ui->characterDataSelectionList->count();
    for (int i = 0; i < count; i++) {
        QListWidgetItem *item = ui->characterDataSelectionList->item(i);
        if (item->checkState() == Qt::Checked) {
            ui->importCardButton->setEnabled(true);
            return;
        }
    }
    ui->importCardButton->setEnabled(false);
}

void CardView::modelItemSelected(const QModelIndex &index)
{
    if (!index.isValid()) {
        m_cardDataSortFilterModel->setSourceModel(nullptr);
        m_card = nullptr;
        return;
    }
    CardFile *card = index.data(CardFileRole).value<CardFile*>();
    ui->cardFace->setPixmap(card->getFace());
    m_cardDataSortFilterModel->setSourceModel(card->getEditDataModel());
    QHeaderView *header = ui->editDataView->header();
    if (header) {
        header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(3, QHeaderView::Stretch);
    }
    m_card = card;
    updateDataControls();
}

void CardView::modelItemUpdated(const QModelIndex &index)
{
    CardFile *card = index.data(CardFileRole).value<CardFile*>();
    if (m_card == card) {
        ui->cardFace->setPixmap(card->getFace());
        updateDataControls();
    }
}

void CardView::importCloth()
{
    if (m_card) {
        bool clothSlot[4];
        clothSlot[0] = ui->checkClothSlot1->isChecked();
        clothSlot[1] = ui->checkClothSlot2->isChecked();
        clothSlot[2] = ui->checkClothSlot3->isChecked();
        clothSlot[3] = ui->checkClothSlot4->isChecked();
        ClothData *cloth = ClothData::fromClothFile(FileDialog::getOpenFileName(FileDialog::ImportCloth, "Cloth (*.cloth)", "", this));
        for (int i = 0; i < 4; i++) {
            if (clothSlot[i])
                m_card->setClothes(i, cloth);
        }
        delete cloth;
    }
}

void CardView::exportCard()
{
    if (m_card) {
        QString file = FileDialog::getSaveFileName(FileDialog::ExtractCard, "PNG Images (*.png)", "Select a card", this);
        if (!file.isEmpty())
            m_card->saveToFile(file);
    }
}

void CardView::exportPortrait()
{
    if (m_card) {
        QString file = FileDialog::getSaveFileName(FileDialog::ExtractCard, "PNG Images (*.png)", "Select a card", this);
        if (!file.isEmpty()) {
            QSaveFile save(file);
            save.open(QSaveFile::WriteOnly);
            save.write(m_card->getEditDataValue("FACE_PNG_DATA").toByteArray());
            save.commit();
        }
    }
}

void CardView::exportThumbnail()
{
    if (m_card) {
        QString file = FileDialog::getSaveFileName(FileDialog::ExtractCard, "PNG Images (*.png)", "Select a card", this);
        if (!file.isEmpty()) {
            QSaveFile save(file);
            save.open(QSaveFile::WriteOnly);
            save.write(m_card->getEditDataValue("ROSTER_PNG_DATA").toByteArray());
            save.commit();
        }
    }
}

void CardView::importCard()
{
    if (m_card) {
        QString file =FileDialog::getOpenFileName(FileDialog::ReplaceCard, "PNG Images (*.png)", "Select a card", this);
        if (file.isEmpty())
            return;
        CardFile card(file);
        if (!card.isValid()) {
            QMessageBox::critical(this, tr("Invalid card"), tr("Invalid card"));
            return;
        }
        int count = ui->characterDataSelectionList->count();
        QSet<int> selection;
        for (int i = 0; i < count; i++) {
            QListWidgetItem *item = ui->characterDataSelectionList->item(i);
            if (item->checkState() == Qt::Checked)
                selection << item->data(Qt::UserRole).toInt();
        }
        if (selection.contains(CharacterDataProfile))
            m_card->replaceEditValues(card.getEditDictionary("PROFILE"));
        if (selection.contains(CharacterDataBody))
            m_card->replaceEditValues(card.getEditDictionary("BODY"));
        if (selection.contains(CharacterDataTraits))
            m_card->replaceEditValues(card.getEditDictionary("TRAIT"));
        if (selection.contains(CharacterDataPreferences))
            m_card->replaceEditValues(card.getEditDictionary("PREFERENCE"));
        if (selection.contains(CharacterDataPregnancy))
            m_card->replaceEditValues(card.getEditDictionary("PREGNANCY"));
        if (selection.contains(CharacterDataCompatibility))
            m_card->replaceEditValues(card.getEditDictionary("COMPATIBILITY"));
        if (selection.contains(CharacterDataSuitUniform))
            m_card->replaceEditValues(card.getEditDictionary("UNIFORM"));
        if (selection.contains(CharacterDataSuitSports))
            m_card->replaceEditValues(card.getEditDictionary("SPORT"));
        if (selection.contains(CharacterDataSuitSwimsuit))
            m_card->replaceEditValues(card.getEditDictionary("SWIMSUIT"));
        if (selection.contains(CharacterDataSuitClub))
            m_card->replaceEditValues(card.getEditDictionary("CLUB"));
        if (selection.contains(CharacterDataPortrait)) {
            m_card->resetPortraitPixmap();
            m_card->replaceEditValues(card.getEditDictionary("FACE_PNG_DATA"));
        }
        if (selection.contains(CharacterDataThumbnail)) {
            m_card->resetThumbnailPixmap();
            m_card->replaceEditValues(card.getEditDictionary("ROSTER_PNG_DATA"));
        }
        if (selection.contains(CharacterDataAAUnlimited))
            m_card->setAAUnlimitedData(card.aauData(), card.aauDataVersion());
        m_card->updateQuickInfoGetters();
    }
}

void CardView::replaceFacePNG()
{
    if (m_card) {
        QFile file(FileDialog::getOpenFileName(FileDialog::ReplacePNG, "PNG Images (*.png)", "Select a PNG image", this));
        if (file.open(QFile::ReadOnly))
            m_card->setFace(&file);
    }
}

void CardView::replaceRosterPNG()
{
    if (m_card) {
        QFile file(FileDialog::getOpenFileName(FileDialog::ReplacePNG, "PNG Images (*.png)", "Select a PNG image", this));
        if (file.open(QFile::ReadOnly))
            m_card->setRoster(&file);
    }
}

bool CardView::eventFilter(QObject *watched, QEvent *event)
{
    if (!m_card)
        return false;
    if (event->type() == QEvent::KeyPress) {
        m_setText++;
    }
    else if (event->type() == QEvent::KeyRelease) {
        m_card->setEditDataValue(watched->objectName(), qobject_cast<QPlainTextEdit*>(watched)->toPlainText());
        m_setText--;
    }
    return false;
}
