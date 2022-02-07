#ifndef ADDNEWSUBJECTDIALOG_H
#define ADDNEWSUBJECTDIALOG_H

#include <QDialog>

namespace Ui {
class AddNewSubjectDialog;
}

class AddNewSubjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewSubjectDialog(QWidget *parent = nullptr);
    ~AddNewSubjectDialog();

    QString getSubjectName();

private:
    Ui::AddNewSubjectDialog *ui;
};

#endif // ADDNEWSUBJECTDIALOG_H
