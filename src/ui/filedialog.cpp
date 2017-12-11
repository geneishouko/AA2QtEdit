#include "filedialog.h"

using namespace ClassEdit;

static QHash<int,QString> m_selectionHistory;

QString FileDialog::getOpenFileName(Context context, const QString &filter, const QString &caption, QWidget *parent)
{
    QString ret = QFileDialog::getOpenFileName(parent, caption, m_selectionHistory[context], filter);
    if (!ret.isEmpty()) {
        QFileInfo info(ret);
        m_selectionHistory[context] = info.dir().path();
    }
    return ret;
}
