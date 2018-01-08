#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QFileDialog>

namespace ClassEdit {

    namespace FileDialog
    {
        enum Context {
            CardFolder,
            ClassSave,
            ImportCloth,
            ExtractCard,
            ReplaceCard,
            ReplacePNG,
            StartPath,
            ContextCount
        };

        QString getExistingDirectory(Context context, const QString &caption = QString(), QWidget *parent = nullptr);
        QString getOpenFileName(Context context, const QString &filter, const QString &caption, QWidget *parent = nullptr);
        QString getSaveFileName(Context context, const QString &filter, const QString &caption, QWidget *parent = nullptr);
        void setStartPath(const QString &path);
    }

}

#endif // FILEDIALOG_H
