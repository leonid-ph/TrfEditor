#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include "trf.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionAddRecord_triggered();
    void on_actionDeleteRecord_triggered();

    void on_RecordNumberEdit_editingFinished();

    void on_PrevRecordPushButton_clicked();
    void on_NextRecordPushButton_clicked();

    void on_RewriteButton_clicked();

    void on_actionSaveToTXT_triggered();

private:
    Ui::MainWindow *ui;

    bool DataModified;
    quint16 TotalRecords;
    quint16 CurrentRecordNumber;

    trf MuSR;

    void ShowRecord ();
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
