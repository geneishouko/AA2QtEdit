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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../cardfile.h"
#include "../classsavecardlistmodel.h"
#include "../filesystemcardlistmodel.h"
#include "../settings.h"
#include "filedialog.h"
#include "preferences.h"
#include "classsavedata.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QLabel>
#include <QListView>
#include <QTableView>
#include <QTreeView>

#include <QtDebug>

using namespace ClassEdit;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_cardListModel(nullptr),
    m_sortFilterModel(new QSortFilterProxyModel(this))
{
    ui->setupUi(this);
    ui->cardListView->setMouseTracking(true);
    ui->cardListView->setModel(m_sortFilterModel);
    QWidget *separator = new QWidget(this);
    separator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolBar->insertWidget(ui->actionPreferences, separator);
    QObject::connect(m_sortFilterModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), ui->cardView, SLOT(modelItemUpdated(QModelIndex)));
    QObject::connect(ui->cardListView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), ui->cardView, SLOT(modelItemSelected(QModelIndex)));
    QObject::connect(ui->cardListView, &CardListView::droppedFiles, this, &MainWindow::loadDroppedFiles);
    m_sortFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_sortFilterModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_sortFilterModel->setSortRole(CardModifiedTimeRole);
    m_sortFilterModel->sort(0, Qt::DescendingOrder);
    connect(ui->textFilter, &QLineEdit::textChanged, m_sortFilterModel, &QSortFilterProxyModel::setFilterFixedString);
    ui->sortBy->addItem("Modified Time", CardModifiedTimeRole);
    ui->sortBy->addItem("Name", Qt::DisplayRole);
    ui->sortBy->addItem("Seat", CardSeatRole);
    ui->sortOrder->addItem("Ascendant", Qt::AscendingOrder);
    ui->sortOrder->addItem("Descendant", Qt::DescendingOrder);
    ui->sortOrder->setCurrentIndex(1);
    connect(ui->sortBy, SIGNAL(currentIndexChanged(int)), this, SLOT(setSortKeyRole()));
    connect(ui->sortOrder, SIGNAL(currentIndexChanged(int)), this, SLOT(setSortOrder()));
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::preferences);
    ui->textFilter->setFocus();
    readSettings();
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

void MainWindow::cardsChanged(int count)
{
    ui->actionSave_All_Changes->setEnabled(count > 0);
}

void MainWindow::loadDirectory()
{
    QString path = FileDialog::getExistingDirectory(FileDialog::CardFolder, tr("Select Folder"), this);
    if (path.isEmpty())
        return;
    destroyCurrentModel();
    if (ui->sortBy->currentData().toInt() == CardSeatRole) {
        ui->sortBy->setCurrentIndex(0); //Modified Time
        ui->sortOrder->setCurrentIndex(1); // Descendant
    }
    FileSystemCardListModel *fs = new FileSystemCardListModel(path);
    m_sortFilterModel->setSourceModel(fs);
    m_cardListModel = fs;

    QObject::connect(fs, &FileSystemCardListModel::cardsChanged, this, &MainWindow::cardsChanged);
    QObject::connect(ui->actionSave_All_Changes, &QAction::triggered, fs, &FileSystemCardListModel::saveAll);
    QObject::connect(fs, &CardListModel::notify, statusBar(), &QStatusBar::showMessage);
}

void MainWindow::loadDroppedFiles(QStringList files)
{
    QString first = files.first();
    if (first.endsWith(".sav")) {
        loadSaveFile(first);
        return;
    }
    destroyCurrentModel();
    if (ui->sortBy->currentData().toInt() == CardSeatRole) {
        ui->sortBy->setCurrentIndex(0); //Modified Time
        ui->sortOrder->setCurrentIndex(1); // Descendant
    }
    FileSystemCardListModel *fs = new FileSystemCardListModel(files);
    m_sortFilterModel->setSourceModel(fs);
    m_cardListModel = fs;

    QObject::connect(fs, &FileSystemCardListModel::cardsChanged, this, &MainWindow::cardsChanged);
    QObject::connect(ui->actionSave_All_Changes, &QAction::triggered, fs, &FileSystemCardListModel::saveAll);
    QObject::connect(fs, &CardListModel::notify, statusBar(), &QStatusBar::showMessage);
}

void MainWindow::editClassSave()
{
    ClassSaveCardListModel *model = qobject_cast<ClassSaveCardListModel*>(m_cardListModel);
    if (model) {
        ClassSaveData *editClass = new ClassSaveData(this);
        editClass->setClassData(model);
        editClass->exec();
        editClass->deleteLater();
    }
}

void MainWindow::destroyCurrentModel()
{
    if (m_cardListModel) {
        m_sortFilterModel->setSourceModel(nullptr);
        ui->cardView->invalidateProxyModels();
        m_cardListModel->deleteLater();
        m_cardListModel = nullptr;
    }
    ui->cardView->modelItemSelected(QModelIndex());
}

void MainWindow::preferences()
{
    Preferences *p = new Preferences(this);
    if (p->exec()) {
        QString path = p->fileDialogStartPath();
        FileDialog::setStartPath(path);
    }
    p->deleteLater();
}

void MainWindow::quit()
{
    qApp->quit();
}

void MainWindow::loadSaveFile(QString path)
{
    if (path.isNull())
        path = FileDialog::getOpenFileName(FileDialog::ClassSave, "Class saves (*.sav)", "Open a class save", this);
    if (path.isEmpty())
        return;
    destroyCurrentModel();
    if (ui->sortBy->currentData().toInt() == CardModifiedTimeRole) {
        ui->sortBy->setCurrentIndex(2); // Seat
        ui->sortOrder->setCurrentIndex(0); // Ascendant
    }
    ClassSaveCardListModel *cs = new ClassSaveCardListModel();
    QObject::connect(cs, &CardListModel::notify, statusBar(), &QStatusBar::showMessage);
    cs->loadFromFile(path);
    m_sortFilterModel->setSourceModel(cs);
    m_cardListModel = cs;

    QObject::connect(cs, &ClassSaveCardListModel::cardsChanged, this, &MainWindow::cardsChanged);
    QObject::connect(ui->actionSave_All_Changes, &QAction::triggered, cs, &ClassSaveCardListModel::saveAll);
}

void MainWindow::setSortKeyRole()
{
    m_sortFilterModel->setSortRole(ui->sortBy->currentData().toInt());
}

void MainWindow::setSortOrder()
{
    m_sortFilterModel->sort(0, static_cast<Qt::SortOrder>(ui->sortOrder->currentData().toInt()));
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(960, 640)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}
