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

#include "filedialog.h"

#include "../settings.h"

using namespace ClassEdit;

static QVector<QString> s_selectionHistory;

QString FileDialog::getOpenFileName(Context context, const QString &filter, const QString &caption, QWidget *parent)
{
    QString ret = QFileDialog::getOpenFileName(parent, caption, s_selectionHistory[context], filter);
    if (!ret.isEmpty()) {
        QFileInfo info(ret);
        s_selectionHistory[context] = info.dir().path();
    }
    return ret;
}

QString FileDialog::getSaveFileName(FileDialog::Context context, const QString &filter, const QString &caption, QWidget *parent)
{
    QString ret = QFileDialog::getSaveFileName(parent, caption, s_selectionHistory[context], filter);
    if (!ret.isEmpty()) {
        QFileInfo info(ret);
        s_selectionHistory[context] = info.dir().path();
    }
    return ret;
}

void FileDialog::setStartPath(const QString &path)
{
    s_selectionHistory = QVector<QString>(ContextCount, path);
    settings()->setFileDialogStartPath(path);
}

QString FileDialog::getExistingDirectory(FileDialog::Context context, const QString &caption, QWidget *parent)
{
    QString ret = QFileDialog::getExistingDirectory(parent, caption, s_selectionHistory[context]);
    if (!ret.isEmpty()) {
        QFileInfo info(ret);
        s_selectionHistory[context] = info.dir().path();
    }
    return ret;
}
