#ifndef CONFIG_H
#define CONFIG_H
#include <QString>

class Config
{
public:
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

    QString priceBull;
    QString priceBear;

    Config();
    void LoadSettings(QString fname);
    int precToByte(int val) { return val * 255 / 100; }
};

#endif // CONFIG_H
