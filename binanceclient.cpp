#include "binanceclient.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>

BinanceClient::BinanceClient(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

void BinanceClient::fetchCandles(const QString &symbol, const QString &interval, int limit) {
    QString url = QString("https://api.binance.com/api/v3/klines?symbol=%1&interval=%2&limit=%3")
    .arg(symbol)
        .arg(interval)
        .arg(limit);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Error:" << reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray response = reply->readAll();
        reply->deleteLater();

        QJsonDocument doc = QJsonDocument::fromJson(response);
        if (!doc.isArray()) return;

        QVector<CandleData> candles;
        QJsonArray arr = doc.array();
        for (auto val : arr) {
            QJsonArray c = val.toArray();
            CandleData candle;
            candle.openTime = static_cast<qint64>(c[0].toDouble());
            candle.open  = c[1].toString().toDouble();
            candle.high  = c[2].toString().toDouble();
            candle.low   = c[3].toString().toDouble();
            candle.close = c[4].toString().toDouble();
            candle.volume= c[5].toString().toDouble();
            candles.append(candle);
        }
        emit candlesReceived(candles);
    });
}
