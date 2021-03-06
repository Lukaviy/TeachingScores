#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "datamodel.h"
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void C_nu_changed(std::optional<float>);

public slots:
    void newFile();
    void saveFile();
    void saveFileAs();
    void openFile();

    void addNewSubject();
    void addNewArticle();

    void toggleSubjects();

    void sort();

    void editSubjects();

    void removeArticle();

    void exportData();

    void onCellClicked(QModelIndex index);
    void onCustomContextMenuRequested(QPoint point);

signals:
    void modelReady(bool);
    void C_nu_textChanged(QString);

private:
    void setNewModel(std::unique_ptr<DataModel> model);

    bool openFile(const QString& filePath);

    Ui::MainWindow *ui;

    std::unique_ptr<DataModel> m_dataModel;

    std::optional<QString> m_filePath;

    QSettings m_settings{"Tsoi Productions", "Teaching Scores"};
};
#endif // MAINWINDOW_H
