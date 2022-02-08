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

    QStringList getSubjectNames();

public slots:
    void onTextChanged();

private:
    Ui::AddNewSubjectDialog *ui;

    QString m_text;
};

#endif // ADDNEWSUBJECTDIALOG_H
