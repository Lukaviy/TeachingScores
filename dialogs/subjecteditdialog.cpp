#include "subjecteditdialog.h"
#include "ui_subjecteditdialog.h"
#include "addnewsubjectdialog.h"

SubjectEditDialog::SubjectEditDialog(std::vector<ts::Subject>&& subjects, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubjectEditDialog),
    m_dataModel(std::make_unique<SubjectsDataModel>(std::move(subjects)))

{
    ui->setupUi(this);

    ui->tableView->setModel(m_dataModel.get());
}

SubjectEditDialog::~SubjectEditDialog()
{
    delete ui;
}

std::vector<ts::Subject> SubjectEditDialog::getSubjects() const
{
    return m_dataModel->getSubjects();
}

void SubjectEditDialog::moveSubjectDown()
{
    const auto currentIndex = ui->tableView->currentIndex();

    if (!currentIndex.isValid()) {
        return;
    }

    m_dataModel->moveDown(currentIndex.row());
}

void SubjectEditDialog::moveSubjectUp()
{
    const auto currentIndex = ui->tableView->currentIndex();

    if (!currentIndex.isValid()) {
        return;
    }

    m_dataModel->moveUp(currentIndex.row());
}

void SubjectEditDialog::removeSubject()
{
    const auto currentIndex = ui->tableView->currentIndex();

    if (!currentIndex.isValid()) {
        return;
    }

    m_dataModel->remove(currentIndex.row());
}

void SubjectEditDialog::addSubject()
{
    const auto currentIndex = ui->tableView->currentIndex();

    AddNewSubjectDialog dialog;

    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    auto newSubjectNames = dialog.getSubjectNames();

    if (newSubjectNames.empty()) {
        return;
    }

    std::vector<std::string> names;
    names.reserve(newSubjectNames.size());

    for (auto& name : newSubjectNames) {
        names.push_back(std::move(name).toStdString());
    }

    m_dataModel->add(currentIndex.row(), std::move(names));
}
