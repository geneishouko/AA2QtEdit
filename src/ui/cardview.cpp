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

#include <QLineEdit>

using namespace ClassEdit;

CardView::CardView(QWidget *parent) :
    QTabWidget(parent),
    ui(new Ui::CardView),
    m_card(nullptr)
{
    ui->setupUi(this);
}

CardView::~CardView()
{
    delete ui;
}

void CardView::cardDestroyed()
{
    m_card = nullptr;
}

void CardView::modelItemActivated(const QModelIndex &index)
{
    CardFile* card = index.data(CardFileRole).value<CardFile*>();
    ui->editDataView->setModel(card->getEditDataModel());
}
