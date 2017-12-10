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
    void loadSaveFile();
    void loadDirectory();
    void destroyCurrentModel();
    void quit();
    void setSortKeyRole();
    void setSortOrder();

private:
    Ui::MainWindow *ui;
    QAbstractItemModel *m_cardListModel;
    QSortFilterProxyModel *m_sortFilterModel;
};

#endif // MAINWINDOW_H
