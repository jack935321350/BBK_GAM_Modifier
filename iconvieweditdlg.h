#ifndef ICONVIEWEDITDLG_H
#define ICONVIEWEDITDLG_H

#include <QDialog>
#include <QPushButton>
#include <QSettings>
#include <QFileDialog>
#include <QImage>

#include "iconviewedit.h"

namespace Ui {
class IconViewEditDlg;
}

class IconViewEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit IconViewEditDlg(QWidget *parent = nullptr);
    ~IconViewEditDlg();
    void UpdateView(uint8_t *data, uint32_t len);
    void WriteBack(uint8_t *data, uint32_t len);

private:
    Ui::IconViewEditDlg *ui;

private slots:
    void on_brushTypeChanged(IconViewEdit::BrushType brushType);
    void on_iconIndex_valueChanged(int index);
    void on_pushButton_clicked();
    void on_dlgBtnBox_clicked(QAbstractButton *button);

};

#endif // ICONVIEWEDITDLG_H
