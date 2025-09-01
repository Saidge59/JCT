#include <QCoreApplication>
#include <QSettings>
#include <QFile>

#include "config.h"

Config::Config() {}

void Config::LoadSettings(QString fname)
{
    QString currentDir = QCoreApplication::applicationDirPath();
    fname = currentDir + "/" + fname;

    QString errLoadSettings;
    QSettings settings(fname, QSettings::IniFormat);
    if (!QFile::exists(fname)) {
        qDebug() << "Cannot load " << fname;
        return;
    }

    symbolDef = settings.value("SymbolDef").toString();
    intervalDef = settings.value("IntervalDef").toString();
    limitCandleDef = settings.value("LimitCandleDef").toInt();

    beakgroundColor = settings.value("BeakgroundColor").toString();
    beakgroundAlpha = precToByte(settings.value("BeakgroundAlpha").toInt());

    candleBear = settings.value("CandleBear").toString();
    candleBull = settings.value("CandleBull").toString();
    candleAlpha = precToByte(settings.value("CandleAlpha").toInt());

    volumeBear = settings.value("VolumeBear").toString();
    volumeBull = settings.value("VolumeBull").toString();
    volumeAlpha = precToByte(settings.value("VolumeAlpha").toInt());

    lineCurrentPriceColor = settings.value("LineCurrentPriceColor").toString();
    lineUserColor = settings.value("LineUserColor").toString();
    linePositionColor = settings.value("LinePositionColor").toString();

    priceBull = settings.value("PriceBull").toString();
    priceBear = settings.value("PriceBear").toString();
}

