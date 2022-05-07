#include "addnewsubjectdialog.h"
#include "ui_addnewsubjectdialog.h"

AddNewSubjectDialog::AddNewSubjectDialog(std::string&& entityName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewSubjectDialog)
{
    ui->setupUi(this);

    setWindowTitle("Add " + QString::fromStdString(std::move(entityName)));
}

AddNewSubjectDialog::~AddNewSubjectDialog()
{
    delete ui;
}

QStringList AddNewSubjectDialog::getSubjectNames() {

    QStringList res;

    auto names = m_text.split("\n");

    for (auto& name : names) {
        name = name.simplified();

        if (!name.isEmpty()) {
            res.append(name);
        }
    }

    return res;
}

void AddNewSubjectDialog::onTextChanged()
{
    m_text = ui->plainTextEdit->toPlainText();
}
