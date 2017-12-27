#ifndef COLORITEMEDITOR_H
#define COLORITEMEDITOR_H

#include <QColorDialog>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QWidget>

namespace ClassEdit {

    class ColorItemEditor : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY(QColor color READ color WRITE setColor USER true)

    public:
        explicit ColorItemEditor(QWidget *parent = nullptr);

        QColor color() const;
        void setColor(QColor color);
        void setFocus() const;

    signals:

    public slots:
        void textChanged(QString text);
        void pickColor();

    private:
        QColor m_color;
        QLineEdit *m_colorLineEdit;
        QRegularExpressionValidator *m_validator;
    };

}

#endif // COLORITEMEDITOR_H
