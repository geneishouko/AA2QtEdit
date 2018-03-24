#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSortFilterProxyModel>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void cardsChanged(int count);
    void loadSaveFile();
    void loadDirectory();
    void editClassSave();
    void destroyCurrentModel();
    void preferences();
    void quit();
    void readSettings();
    void setSortKeyRole();
    void setSortOrder();
    void writeSettings();

private:
    Ui::MainWindow *ui;
    QAbstractItemModel *m_cardListModel;
    QSortFilterProxyModel *m_sortFilterModel;
};

#endif // MAINWINDOW_H
