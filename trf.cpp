#include "trf.h"

trf::trf()
{    

}

bool trf::ReadFromFile(QString FileName)
{
    int TotalRecords;
    TrfType TrfBuffer = {};
    QFile File(FileName);

    if (!File.open(QIODevice::ReadOnly))
    {
        qDebug() << "Файл не найден";
        return false;
    }
    if (File.size()%RECORD_SIZE)
    {
        qDebug() << "Неверный тип файла";
        return false;
    }

    TotalRecords = File.size()/RECORD_SIZE;
    qDebug() << "Число записей: "<< TotalRecords;

    TrfVector.fill(TrfBuffer, TotalRecords);
    if (File.read((char*) TrfVector.data(), TotalRecords*RECORD_SIZE) < 0) return false;

    File.close();

    return true;
}

bool trf::SaveToFile(QString FileName)
{
    QFile File(FileName);

    if (!File.open(QIODevice::WriteOnly)) return false;

    if ( ( File.write((char*) TrfVector.data(), TrfVector.size()*RECORD_SIZE) ) < 0 ) return false;

    qDebug() << "Имя файла для записи: " << FileName;
    qDebug() << "Число записей: " << (float) (File.size()/RECORD_SIZE);

    File.close();

    return true;
}

bool trf::SaveToTXT (QString FileName, int position)
{
    QFile File(FileName);

    if ( ( position < 0 )|| ( position >= TrfVector.size() ) ) return false;
    if (!File.open(QIODevice::WriteOnly)) return false;

    QTextStream Stream( &File );

    Stream << "##### EsHead: #####" << endl;
    for(int i = 0; i < 21; i++)     Stream <<TrfVector[position].EsHead[i]<<endl;
    Stream << "##### Scalers: #####" << endl;
    for(int i = 0; i < 7; i++)     Stream <<TrfVector[position].Scaler[i]<<endl;
    Stream << "##### Hist: #####" << endl;
    for(int i = 0; i < 2048; i++)   Stream <<TrfVector[position].Hist[i]<<endl;

    Stream << "##### Temperature: #####" << endl;
    Stream <<"TimeRun: ";
    for(int i = 0; i < 8; i++) Stream << TrfVector[position].TempC.TimeRun[i]<<" ";

    Stream <<"TimeStop: ";
    for(int i = 0; i < 8; i++) Stream << TrfVector[position].TempC.TimeStop[i]<<" ";

    Stream <<"Com: ";
    for(int i = 0; i < 2; i++) Stream << TrfVector[position].TempC.Com[i]<<" ";

    Stream <<"RDMSS: ";
    for(int i = 0; i < 10; i++) Stream << TrfVector[position].TempC.RDMss[i]<<" ";
    Stream << endl;

    Stream << "##### Temp: #####" << endl;
    for(int i = 0; i < ( 400 - sizeof(TempType) ); i++)   Stream <<TrfVector[position].Temp[i]<<endl;

    File.close();

    return true;
}

void trf::New()
{
    TrfType TrfBuffer = {};
    TrfVector.clear();
    TrfVector.fill(TrfBuffer, 1);
}

void trf::InsertRecord (int position)
{
    if ( ( position < 0 )|| ( position > TrfVector.size() ) ) return;
    TrfType TrfBuffer = {};
    TrfVector.insert(position, TrfBuffer);
}

void trf::RemoveRecord (int position)
{
    if ( ( position < 0 )|| ( position >= TrfVector.size() ) ) return;
    TrfVector.remove(position);
}

QVector<double> trf::GetSpectrum(int position)
{
    QVector<double> Spectrum(0);

    if ( ( position < 0 )|| ( position >= TrfVector.size() ) )
    {
        Spectrum.fill(0,2048);
        return Spectrum;
    }

    for(int i = 0; i < 2048; i++)
        Spectrum.push_back(TrfVector[position].Hist[i]);

    return Spectrum;
}

QVector<double> trf::GetScalers(int position)
{
    QVector<double> Scalers(0);

    if ( ( position < 0 )|| ( position >= TrfVector.size() ) )
    {
        Scalers.fill(0,7);
        return Scalers;
    }

    for(int i = 0; i < 7; i++)
        Scalers.push_back(TrfVector[position].Scaler[i]);

    return Scalers;
}

void trf::SetSpectrum (int position, QVector<double> Spectrum)
{
    if ( ( position < 0 )|| ( position >= TrfVector.size() ) || (Spectrum.size() != 2048) )   return;

    for(int i = 0; i < 2048; i++)
        TrfVector[position].Hist[i] = Spectrum[i];
}

void trf::SetScaler (int position, QVector<double> Scaler)
{
    if ( ( position < 0 )|| ( position >= TrfVector.size() ) || (Scaler.size() != 7) )   return;

    for(int i = 0; i < 7; i++)
        TrfVector[position].Scaler[i] = Scaler[i];
}

int trf::GetTotalRecords()
{
    return TrfVector.size();
}

trf::~trf()
{

}
