#ifndef BINANCECLIENT_H
#define BINANCECLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include "candledata.h"

class BinanceClient : public QObject {
    Q_OBJECT
public:
    explicit BinanceClient(QObject *parent = nullptr);

    void fetchCandles(const QString &symbol, const QString &interval, int limit);

    int countSignificantDecimals(const QString &numStr);

signals:
    void candlesReceived(const QVector<CandleData> &candles);
    void candlesDecimals(int decimals);

private:
    QNetworkAccessManager *manager;
    int decimals;
};

#endif // BINANCECLIENT_H
