#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

namespace ClassEdit {

    class Settings : public QSettings
    {
    private:
        Settings();

    public:
        static Settings *getSettings();

        QString getFileDialogStartPath();
        void setFileDialogStartPath(const QString &path);
    };

    inline Settings *settings() {
        return Settings::getSettings();
    }
}

#endif // SETTINGS_H
