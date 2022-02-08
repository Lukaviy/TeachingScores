#ifndef SUBJECTEDITDIALOG_H
#define SUBJECTEDITDIALOG_H

#include <QDialog>

#include "models/subjectsdatamodel.h"

namespace Ui {
class SubjectEditDialog;
}

class SubjectEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SubjectEditDialog(std::vector<ts::Subject>&& subjects, QWidget *parent = nullptr);
    ~SubjectEditDialog();

    std::vector<ts::Subject> getSubjects() const;

public slots:
    void moveSubjectDown();
    void moveSubjectUp();
    void removeSubject();
    void addSubject();

private:
    Ui::SubjectEditDialog *ui;

    std::unique_ptr<SubjectsDataModel> m_dataModel;
};

#endif // SUBJECTEDITDIALOG_H
