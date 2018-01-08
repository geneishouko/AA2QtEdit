#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
class Preferences;
}

namespace ClassEdit {

    class Preferences : public QDialog
    {
        Q_OBJECT
        Q_PROPERTY(QString fileDialogStartPath READ fileDialogStartPath WRITE setFileDialogStartPath)

    public:
        explicit Preferences(QWidget *parent = 0);
        ~Preferences();

        QString fileDialogStartPath() const;

    public slots:
        void setFileDialogStartPath(const QString &path);
        void pickFileDialogStartPath();

    private:
        Ui::Preferences *ui;

        QString m_fileDialogStartPath;
    };

}

#endif // PREFERENCES_H
