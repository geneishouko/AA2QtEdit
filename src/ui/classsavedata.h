#ifndef CLASSSAVEDATA_H
#define CLASSSAVEDATA_H

#include <QDialog>

namespace Ui {
class ClassSaveData;
}

namespace ClassEdit {

    class ClassSaveCardListModel;

    class ClassSaveData : public QDialog
    {
        Q_OBJECT

    public:
        explicit ClassSaveData(QWidget *parent = 0);
        ~ClassSaveData();

        void accept();
        void setClassData(ClassSaveCardListModel* data);

    private:
        Ui::ClassSaveData *ui;

        ClassSaveCardListModel* m_classData;
    };

}

#endif // CLASSSAVEDATA_H
