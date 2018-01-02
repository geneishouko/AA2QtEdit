#include "coloritemeditor.h"

#include <QApplication>
#include <QFrame>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

using namespace ClassEdit;

ColorItemEditor::ColorItemEditor(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    QPushButton *pickButton = new QPushButton(tr("..."));
    pickButton->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QSize minSize = pickButton->minimumSize();
    minSize.setWidth(24);
    pickButton->setMinimumSize(minSize);

    QLineEdit *lineEdit = new QLineEdit;
    QFontMetrics fontMetrics = lineEdit->fontMetrics();
    QSize lineSize = fontMetrics.size(Qt::TextSingleLine, "#0000000000");
    lineEdit->setMinimumSize(lineSize);
    lineEdit->setMaximumSize(lineSize);
    lineEdit->setFrame(false);
    layout->addWidget(lineEdit, 0);
    layout->addWidget(pickButton, 0);
    layout->addStretch(1);
    layout->setMargin(1);
    setLayout(layout);
    setFocusProxy(lineEdit);
    setFocusPolicy(Qt::StrongFocus);

    QRegularExpression rx("#([a-fA-F0-9]{2})?[a-fA-F0-9]{6}");
    m_validator = new QRegularExpressionValidator(rx, this);

    m_colorLineEdit = lineEdit;

    connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(pickButton, SIGNAL(clicked()), this, SLOT(pickColor()));
}

QColor ColorItemEditor::color() const
{
    return m_color;
}

void ColorItemEditor::setColor(QColor color)
{
    m_color = color;
    if (color.alpha() == 255)
        m_colorLineEdit->setText(color.name(QColor::HexRgb));
    else
        m_colorLineEdit->setText(color.name(QColor::HexArgb));
}

void ColorItemEditor::setFocus() const
{
    m_colorLineEdit->setFocus();
}

void ColorItemEditor::textChanged(QString text)
{
    int pos = 0;
    if (m_validator->validate(text, pos) == QValidator::Acceptable) {
        m_color = QColor(text);
    }
}

void ColorItemEditor::pickColor()
{
    QColor newColor = QColorDialog::getColor(m_color, this, tr("Pick a color"), QColorDialog::ShowAlphaChannel);
    if (newColor.isValid())
        setColor(newColor);
}
