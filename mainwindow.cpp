#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "datamodel.h"
#include "addnewsubjectdialog.h"

#include "formats/jsonformat.h"

#include <QFileDialog>
#include <QMessageBox>
#include "view/itemdelegate.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto data = ts::VerifiedData::initializeWithDefaults({
                                               ts::Subject{ .id = ts::UniqueIdProvider<ts::Subject>::generateId(), .name = "Subject Huita" }
                                           },
                                           {
                                               ts::Article{ .id = ts::UniqueIdProvider<ts::Article>::generateId(), .name = "Article Huita" }
                                           }
                                           ).value();

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

void MainWindow::saveFile()
{
    if (!m_filePath) {
        auto filePath = QFileDialog::getSaveFileName(this, "Save File", QString(), "Json (*.json)");

        if (filePath.isEmpty()) {
            return;
        }

        m_filePath = filePath;
    }

    QFile saveFile(m_filePath.value());

    if (!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Save file"), "Can't access file");

        return;
    }

    saveFile.write(ts::formats::JsonFormat().exportData(m_dataModel->getData()));
}

void MainWindow::openFile()
{
    auto filePath = QFileDialog::getOpenFileName(this, "Open File", QString(), "Json (*.json)");

    if (filePath.isEmpty()) {
        return;
    }

    QFile openFile(filePath);

    if (!openFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Open file"), "Can't open file");

        return;
    }

    auto fileData = openFile.readAll();

    if (fileData.isEmpty()) {
        QMessageBox::critical(this, tr("Open file"), "File is empty or unexpected error occured");

        return;
    }

    auto data = ts::formats::JsonFormat().importData(fileData);

    if (!data) {
        QMessageBox::critical(this, tr("Open file"), QString::fromStdString("Can't open file, file corrupted: " + data.error()));

        return;
    }

    auto model = std::make_unique<DataModel>(ts::ComputedDataModel::compute(std::move(data).value()));

    ui->tableView->setModel(model.get());
    //ui->tableView->setItemDelegate(new ItemDelegate());

    m_dataModel = std::move(model);

    m_filePath = filePath;
}

void MainWindow::addNewSubject()
{
    AddNewSubjectDialog dialog;

    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    m_dataModel->addSubject(dialog.getSubjectName().toStdString());
}

void MainWindow::addNewArticle()
{
    m_dataModel->addArticle("New Subject");
}

