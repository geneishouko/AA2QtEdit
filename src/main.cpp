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

#include "ui/mainwindow.h"

#include "src/datareader.h"
#include "src/ui/filedialog.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include <QtDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifdef Q_OS_LINUX
    QIcon icon;
    icon.addFile(":/icon/application-icon_16x16.png");
    icon.addFile(":/icon/application-icon_32x32.png");
    icon.addFile(":/icon/application-icon_48x48.png");
    icon.addFile(":/icon/application-icon_64x64.png");
    a.setWindowIcon(icon);
#endif

    QCoreApplication::setOrganizationName("aa2g");
    QCoreApplication::setOrganizationDomain("aa2g.vg.4chan.org");
    QCoreApplication::setApplicationName("AA2QtEdit");

    ClassEdit::DataReader::getDataReader("chardata");
    ClassEdit::DataReader::getDataReader("playdata");
    ClassEdit::DataReader::getDataReader("headerdata");
    ClassEdit::DataReader::getDataReader("clothdata");

    {
        QDir dir(a.applicationDirPath());
        QStringList filter;
        filter << "*.xml";
        QFileInfoList definitions = dir.entryInfoList(filter, QDir::Files, QDir::Time);
        foreach (const QFileInfo &path, definitions) {
            qDebug() << "load definitions" << path.completeBaseName();
            QFile xml(path.absoluteFilePath());
            xml.open(QFile::ReadOnly);
            ClassEdit::DataReader::loadExternalDefinitions(&xml);
        }

        QSettings settings;
        settings.beginGroup("General");
        ClassEdit::FileDialog::setStartPath(settings.value("FileDialogStartPath", QString()).toString());
        settings.endGroup();
    }

    MainWindow w;
    w.show();

    return a.exec();
}

#ifdef WIN32
#ifdef QT_STATIC
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin)
#endif
#endif

