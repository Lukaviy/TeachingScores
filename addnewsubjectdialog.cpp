#include "addnewsubjectdialog.h"
#include "ui_addnewsubjectdialog.h"

AddNewSubjectDialog::AddNewSubjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewSubjectDialog)
{
    ui->setupUi(this);
}

AddNewSubjectDialog::~AddNewSubjectDialog()
{
    delete ui;
}

QString AddNewSubjectDialog::getSubjectName()
{
   return ui->subjectNameLineEdit->text();
}
