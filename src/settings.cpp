#include "settings.h"

using namespace ClassEdit;

static Settings *s_settings = nullptr;

Settings::Settings()
{
}

ClassEdit::Settings *ClassEdit::Settings::getSettings()
{
    if (!s_settings)
        s_settings = new Settings;
    return s_settings;
}

QString ClassEdit::Settings::getFileDialogStartPath()
{
    beginGroup("General");
    QString ret = value("FileDialogStartPath", QString()).toString();
    endGroup();
    return ret;
}

void ClassEdit::Settings::setFileDialogStartPath(const QString &path)
{
    beginGroup("General");
    setValue("FileDialogStartPath", path);
    endGroup();
}
