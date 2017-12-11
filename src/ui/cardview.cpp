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
#include "filedialog.h"

#include <QHeaderView>
#include <QLineEdit>

using namespace ClassEdit;

CardView::CardView(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::CardView),
    m_card(nullptr),
    m_cardDataSortFilterModel(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);
    m_cardDataSortFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_cardDataSortFilterModel->setFilterKeyColumn(2);
    ui->editDataView->setModel(m_cardDataSortFilterModel);
    connect(ui->textMakerKeyFilter, &QLineEdit::textChanged, m_cardDataSortFilterModel, &QSortFilterProxyModel::setFilterFixedString);
}

CardView::~CardView()
{
    delete ui;
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
}

void CardView::modelItemUpdated(const QModelIndex &index)
{
    CardFile *card = index.data(CardFileRole).value<CardFile*>();
    if (m_card == card) {
        ui->cardFace->setPixmap(card->getFace());
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
