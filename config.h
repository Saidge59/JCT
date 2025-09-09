#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QStringList>

class Config
{
public:
    int screenWidth;
    int screenHeight;

    QString symbolDef;
    QString intervalDef;
    int limitCandleDef;

    QString beakgroundColor;
    int beakgroundAlpha;

    QString candleBear;
    QString candleBull;
    int candleAlpha;

    QString volumeBear;
    QString volumeBull;
    int volumeAlpha;

    QString lineCurrentPriceColor;
    QString lineUserColor;
    QString linePositionColor;

    QString priceBullColor;
    QString priceBearColor;

    QString priceMaxColor;
    QString priceMinColor;

    QStringList viewSymbols;

    Config();
    void LoadSettings(QString fname);
    int precToByte(int val) { return val * 255 / 100; }
};

#endif // CONFIG_H
