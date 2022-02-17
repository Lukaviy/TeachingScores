#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "datamodel.h"
#include "addnewsubjectdialog.h"

#include "formats/jsonformat.h"
#include "formats/csvformat.h"

#include <QFileDialog>
#include <QMessageBox>
#include "view/itemdelegate.h"
#include "dialogs/subjecteditdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto filePath = m_settings.value("filePath");
    if (!filePath.isNull()) {
        openFile(filePath.toString());
    } else {
        emit modelReady(false);
        emit C_nu_changed(std::nullopt);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::C_nu_changed(std::optional<float> C_nu)
{
    emit C_nu_textChanged(C_nu ? QString::number(C_nu.value(), 'f', 2) : "N/A");
}

void MainWindow::newFile()
{
    ui->tableView->setModel(nullptr);
    m_dataModel.reset();

    emit modelReady(false);
}

void MainWindow::saveFile()
{
    if (!m_filePath) {
        auto filePath = QFileDialog::getSaveFileName(this, "Save File", m_filePath.value_or(QString()), "Json (*.json)");

        if (filePath.isEmpty()) {
            return;
        }

        m_filePath = filePath;
        m_settings.setValue("filePath", filePath);
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
    auto filePath = QFileDialog::getOpenFileName(this, "Open File", m_filePath.value_or(QString()), "Json (*.json)");

    if (filePath.isEmpty()) {
        return;
    }

    openFile(filePath);
}

void MainWindow::addNewSubject()
{
    AddNewSubjectDialog dialog;

    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    auto subjectNames = dialog.getSubjectNames();

    if (subjectNames.empty()) {
        return;
    }

    if (!m_dataModel) {
        std::vector<ts::Subject> newSubjects;
        newSubjects.reserve(subjectNames.size());

        auto idGenerator = ts::IdGenerator<ts::Subject>{};

        for (auto& subjectName : subjectNames) {
            newSubjects.push_back(ts::Subject{ .id = idGenerator.newId(), .name = std::move(subjectName).toStdString() });
        }

        auto data = ts::VerifiedData::initializeWithDefaults(std::move(newSubjects), {}).value();

        setNewModel(std::make_unique<DataModel>(ts::ComputedDataModel::compute(std::move(data))));
    } else {
        for (auto& subjectName : subjectNames) {
            m_dataModel->addSubject(std::move(subjectName).toStdString());
        }
    }
}

void MainWindow::addNewArticle()
{
    if (!m_dataModel) {
        throw std::exception("Model is not ready");
    }
    m_dataModel->addArticle("New Article");
}

void MainWindow::sort()
{
    m_dataModel->sort();
}

void MainWindow::editSubjects()
{
    auto oldSubjects = m_dataModel ? std::vector{ m_dataModel->getSubjects() } : std::vector<ts::Subject>{};

    SubjectEditDialog dialog(std::move(oldSubjects));

    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    auto subjects = dialog.getSubjects();

    if (subjects.empty()) {
        QMessageBox::critical(this, "Error", "There must be atleast one subject");
        return;
    }

    m_dataModel->setSubjects(std::move(subjects));
}

void MainWindow::removeArticle()
{
    if (!m_dataModel) {
        return;
    }

    const auto currentIndex = ui->tableView->currentIndex();

    if (!currentIndex.isValid()) {
        return;
    }

    m_dataModel->removeArticle(currentIndex.row());
}

void MainWindow::exportData()
{
    auto filePath = QFileDialog::getSaveFileName(this, "Save File", QString(), "Csv (*.csv)");

    if (filePath.isEmpty()) {
        return;
    }

    QFile saveFile(filePath);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Save file"), "Can't access file");

        return;
    }

    saveFile.write(ts::formats::CsvFormat().exportData(m_dataModel->getData()));
}

void MainWindow::onCellClicked(QModelIndex index)
{
    if (!m_dataModel) {
        return;
    }

    m_dataModel->toggleAppearance(index);
}

void MainWindow::onCustomContextMenuRequested(QPoint point)
{
    const auto index = ui->tableView->indexAt(point);

    m_dataModel->setData(index, true, Qt::EditRole);
}

void MainWindow::setNewModel(std::unique_ptr<DataModel> model)
{
    ui->tableView->setModel(nullptr);
    m_dataModel = std::move(model);
    ui->tableView->setModel(m_dataModel.get());

    connect(m_dataModel.get(), &DataModel::C_nu_changed, this, &MainWindow::C_nu_changed);

    emit modelReady(true);
    C_nu_changed(m_dataModel->getC_nu());
}

void MainWindow::openFile(const QString& filePath)
{
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

    setNewModel(std::make_unique<DataModel>(ts::ComputedDataModel::compute(std::move(data).value())));

    m_settings.setValue("filePath", filePath);
    m_filePath = filePath;
}

