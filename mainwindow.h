#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "datamodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void newFile();
    void saveFile();
    void openFile();

    void addNewSubject();
    void addNewArticle();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<DataModel> m_dataModel;

    std::optional<QString> m_filePath;
};
#endif // MAINWINDOW_H
