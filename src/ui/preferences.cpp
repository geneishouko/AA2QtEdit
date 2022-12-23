#include "preferences.h"
#include "ui_preferences.h"

#include "../settings.h"
#include "filedialog.h"

using namespace ClassEdit;

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    connect(ui->fileDialogStartPathText, &QLineEdit::textChanged, this, &Preferences::setFileDialogStartPath);
    connect(ui->fileDialogStartPathButton, &QPushButton::clicked, this, &Preferences::pickFileDialogStartPath);
    QString startPath = settings()->getFileDialogStartPath();
    ui->fileDialogStartPathText->setText(startPath);
    setFileDialogStartPath(startPath);
}

Preferences::~Preferences()
{
    delete ui;
}

QString Preferences::fileDialogStartPath() const
{
    return m_fileDialogStartPath;
}

void Preferences::setFileDialogStartPath(const QString &path)
{
    m_fileDialogStartPath = path;
}

void Preferences::pickFileDialogStartPath()
{
    QString path = FileDialog::getExistingDirectory(FileDialog::StartPath, tr("Select Folder"), this);
    if (!path.isEmpty()) {
        ui->fileDialogStartPathText->setText(path);
        setFileDialogStartPath(path);
    }
}
