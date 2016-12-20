#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qcustomplot.h"

#include <QDebug>
#include <QFile>
#include <QByteArray>
#include <QVector>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    DataModified = false;

    ui->setupUi(this);
    setWindowTitle("TrfEditor");

    ui->Plot->addGraph();
    ui->Plot->graph(0)->setPen(QPen(QColor(0x40, 0x80, 0x80)));
    ui->Plot->setInteractions(QCP::iRangeZoom);

    ui->SummLabel->setText("Summ ");
    ui->TotalRecordNumberEdit->setDisabled(true);
    ui->RecordNumberEdit->setValidator(new QIntValidator(1, 1000,this));

    ui->NextRecordPushButton->setIcon(QIcon(":/images/Icons/arrow.png"));
    ui->PrevRecordPushButton->setIcon(QIcon(":/images/Icons/arrow-180.png"));
    ui->RewriteButton->setIcon(QIcon(":/images/Icons/arrow-curve-180.png"));

    ui->RewriteButton->setEnabled(false);

    TotalRecords = 0;
    CurrentRecordNumber = 0;

    ui->ScalersTextEdit->setDisabled(true);
    ui->SpectrumTextEdit->setDisabled(true);
    ui->RecordNumberEdit->setEnabled(false);

    ui->NextRecordPushButton->setEnabled(false);
    ui->PrevRecordPushButton->setEnabled(false);
    ui->actionAddRecord->setEnabled(false);
    ui->actionDeleteRecord->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionSaveToTXT->setEnabled(false);

}


void MainWindow::ShowRecord()
{
    QString TmpStr;

    TmpStr.clear();

    TotalRecords = MuSR.GetTotalRecords();

    QVector <double> Scalers = MuSR.GetScalers(CurrentRecordNumber);
    QVector <double> Spectrum = MuSR.GetSpectrum(CurrentRecordNumber);

    QVector <double> Channel;
    for (int i = 0; i < 2048;i++)  Channel.push_back(i);

    ui->ScalersTextEdit->setEnabled(false);
    ui->SpectrumTextEdit->setEnabled(false);
    ui->SummLabel->setEnabled(false);

    qDebug()<<"Номер записи"<<CurrentRecordNumber;

    ui->RecordNumberEdit->setText( QString::number(CurrentRecordNumber + 1) );
    ui->TotalRecordNumberEdit->setText( QString::number(TotalRecords) );


    ui->Plot->graph(0)->setLineStyle(QCPGraph::lsImpulse);
    ui->Plot->graph(0)->setData(Channel, Spectrum);
    ui->Plot->rescaleAxes();
    ui->Plot->replot();

    quint64 Summ = 0;
    for (int i = 0; i < Spectrum.size();i++)
    {
        Summ+= Spectrum[i];
    }
    ui->SummLabel->setText("Sum: " + QString::number(Summ));

    for (int i = 0; i < 7; i++)
    {
        TmpStr.append(( QString::number( Scalers[i]) ) + '\n');
    }
    TmpStr.chop(1);
    ui->ScalersTextEdit->setText(TmpStr);


    TmpStr.clear();
    for (int i = 0; i < 2048;i++)
    {
        TmpStr.append( (QString::number( Spectrum[i]) ) + '\n');
    }

    TmpStr.chop(1);
    ui->SpectrumTextEdit->setText(TmpStr);


    ui->ScalersTextEdit->setEnabled(true);
    ui->SpectrumTextEdit->setEnabled(true);
    ui->SummLabel->setEnabled(true);
}


// ########################## TOOLBAR ##########################

void MainWindow::on_actionNew_triggered()
{
    if (DataModified)
    {
        QMessageBox msgBox;
        msgBox.setText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);

        switch ( msgBox.exec() )
        {
        case QMessageBox::Save:
            emit on_actionSave_triggered();
            break;
        case QMessageBox::Discard:
            break;
        default:
            return;
            break;
        }
    }

    MuSR.New();
    CurrentRecordNumber = 0;

    ui->RewriteButton->setEnabled(true);
    ui->NextRecordPushButton->setEnabled(true);
    ui->PrevRecordPushButton->setEnabled(true);
    ui->actionAddRecord->setEnabled(true);
    ui->actionDeleteRecord->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->RecordNumberEdit->setEnabled(true);
    ui->actionSaveToTXT->setEnabled(true);

    DataModified = false;
    ShowRecord();
}

void MainWindow::on_actionOpen_triggered()
{
    QString FileName = QFileDialog::getOpenFileName(0, "Open Dialog", "", "*.trf");

    if( !MuSR.ReadFromFile(FileName) )
    {
        QMessageBox::critical(0,"Error", "Can not open file!",0,0);
        return;
    }

    setWindowTitle(FileName + " - TrfEditor");

    CurrentRecordNumber = 0;


    ui->RewriteButton->setEnabled(true);
    ui->NextRecordPushButton->setEnabled(true);
    ui->PrevRecordPushButton->setEnabled(true);
    ui->actionAddRecord->setEnabled(true);
    ui->actionDeleteRecord->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->RewriteButton->setEnabled(true);
    ui->RecordNumberEdit->setEnabled(true);
    ui->actionSaveToTXT->setEnabled(true);

    ShowRecord();

}

void MainWindow::on_actionSave_triggered()
{
    QString FileName = QFileDialog::getSaveFileName(this, "Save file", "", "*.trf");

    if( !MuSR.SaveToFile(FileName) )
        QMessageBox::critical(0,"Error", "Data not saved",0,0);

    DataModified = false;
}

void MainWindow::on_actionAddRecord_triggered()
{
    MuSR.InsertRecord(++CurrentRecordNumber);
    //CurrentRecordNumber++;

    DataModified = true;
    ShowRecord();
}

void MainWindow::on_actionDeleteRecord_triggered()
{
    MuSR.RemoveRecord(CurrentRecordNumber);

    if (CurrentRecordNumber == TotalRecords - 1) CurrentRecordNumber--;
    //else CurrentRecordNumber++;

    DataModified = true;
    ShowRecord();
}


// ########################## BUTTONS ##########################

void MainWindow::on_RewriteButton_clicked()
{
    QString TmpStr = ui->SpectrumTextEdit->toPlainText();
    QStringList TmpStrList = TmpStr.split('\n', QString::SkipEmptyParts);

    QVector <double> Scalers(7,0),Spectrum(2048,0);

    if (TmpStrList.size() != 2048)
    {
        QMessageBox::warning(0,"Warning", "Wrong Spectrum Data Length!",0,0);
    }

    for (int i = 0;( i < TmpStrList.size() ) && (i < 2048); i++)
    {
        Spectrum[i] = (qint16) TmpStrList[i].toInt();
    }

    MuSR.SetSpectrum(CurrentRecordNumber, Spectrum);

    //TmpStr.clear();
    //TmpStrList.clear();

    TmpStr = ui->ScalersTextEdit->toPlainText();
    TmpStrList = TmpStr.split('\n', QString::SkipEmptyParts);


    if (TmpStrList.size() != 7)
    {
        QMessageBox::warning(0,"Warning", "Wrong Scalers Data Length!",0,0);
    }

    for (int i = 0; (i < TmpStrList.size() ) && ( i < 7); i++)
    {
        Scalers[i] = (qint32) TmpStrList[i].toUInt();
    }
    MuSR.SetScaler(CurrentRecordNumber,Scalers);

    DataModified = true;
    ShowRecord();
}

void MainWindow::on_PrevRecordPushButton_clicked()
{
    if(CurrentRecordNumber <= 0) CurrentRecordNumber = TotalRecords - 1;
    else CurrentRecordNumber--;
    ShowRecord();
}

void MainWindow::on_NextRecordPushButton_clicked()
{
    CurrentRecordNumber++;
    if(CurrentRecordNumber >= TotalRecords) CurrentRecordNumber = 0;
    ShowRecord();
}

void MainWindow::on_RecordNumberEdit_editingFinished()
{
    quint16 RecNum = ui->RecordNumberEdit->text().toInt() - 1;

    if (RecNum < TotalRecords)
    {
        CurrentRecordNumber = RecNum;
        ShowRecord();
    }
    else
    {
        ui->RecordNumberEdit->setText(QString::number(CurrentRecordNumber + 1));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSaveToTXT_triggered()
{
    QString FileName = QFileDialog::getSaveFileName(this, "Save file", "", "*.txt");

    if( !MuSR.SaveToTXT(FileName,CurrentRecordNumber) )
        QMessageBox::critical(0,"Error", "Data not saved",0,0);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!DataModified)  event->accept();
    else
    {
        event->ignore();

        QMessageBox msgBox;
        msgBox.setText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);

        switch ( msgBox.exec() )
        {
        case QMessageBox::Save:
            emit on_actionSave_triggered();
            break;
        case QMessageBox::Discard:
            event->accept();
            break;
        default:
            return;
            break;
        }
    }
}
