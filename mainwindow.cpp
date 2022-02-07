#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "datamodel.h"
#include "addnewsubjectdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto data = ts::initializeWithDefaults({
                                               ts::Subject{ .id = ts::UniqueIdProvider<ts::Subject>::generateId(), .name = "Subject Huita" }
                                           },
                                           {
                                               ts::Article{ .id = ts::UniqueIdProvider<ts::Article>::generateId(), .name = "Article Huita" }
                                           }
                                           );

    m_dataModel = std::make_unique<DataModel>(ts::ComputedDataModel::compute(std::move(data)));

    ui->tableView->setModel(m_dataModel.get());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFile()
{

}

void MainWindow::addNewSubject()
{
    AddNewSubjectDialog dialog;

    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    m_dataModel->addSubject(ts::Subject{
                                .id = ts::UniqueIdProvider<ts::Subject>::generateId(),
                                .name = dialog.getSubjectName().toStdString()
                            });
}

void MainWindow::addNewArticle()
{
    m_dataModel->addArticle(ts::Article{
                                .id = ts::UniqueIdProvider<ts::Article>::generateId(),
                                .name = "New Subject"
                            });
}

