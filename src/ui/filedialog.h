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
            ReplaceCard,
            ReplacePNG
        };

        QString getOpenFileName(Context context, const QString &filter, const QString &caption, QWidget *parent = nullptr);
    };

}

#endif // FILEDIALOG_H
