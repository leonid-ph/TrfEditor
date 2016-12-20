#ifndef TRF_H
#define TRF_H

#include <QDebug>
#include <QFile>
#include <QVector>
#include <QString>

#pragma pack(push,1) // Устанавливаем размер выравнивания в 1 байт

struct TempType
{
    quint8 TimeRun[8];
    quint8 TimeStop[8];
    quint8 Com[2];
    qint32 RDMss[10];
};// Размер типа TempType - 58 байт

struct TrfType
{
    qint16      EsHead[21]; // Header for Es buffer
    qint32      Scaler[7];  // MSS
    qint16      Hist[2048]; // Spectrum
    TempType    TempC;      // Temperature mode
    quint8      Temp[ 400 - sizeof(TempType) ];
};// Размер типа TrfType - 4566 байт

#pragma pack(pop) // Возвращаем старые настройки выравнивания

#define RECORD_SIZE     sizeof(TrfType)


class trf
{
    QVector <TrfType> TrfVector;

public:
    trf();

    void InsertRecord (int position);
    void RemoveRecord (int position);

    QVector<double> GetSpectrum(int position);
    QVector<double> GetScalers(int position);

    void SetSpectrum (int position, QVector<double> Spectrum);
    void SetScaler (int position, QVector<double> Scaler);

    bool ReadFromFile(QString FileName);
    bool SaveToFile(QString FileName);
    void New ();

    bool SaveToTXT (QString FileName, int position);

    int GetTotalRecords ();

    ~trf();
};

#endif // TRF_H
