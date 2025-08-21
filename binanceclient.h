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

signals:
    void candlesReceived(const QVector<CandleData> &candles);

private:
    QNetworkAccessManager *manager;
};

#endif // BINANCECLIENT_H
