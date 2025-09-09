#include "binanceclient.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>

BinanceClient::BinanceClient(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

int BinanceClient::countSignificantDecimals(const QString &numStr) {
    int dotPos = numStr.indexOf('.');
    if (dotPos == -1) return 0;
    QString fractional = numStr.mid(dotPos + 1);

    while (fractional.endsWith('0'))
        fractional.chop(1);

    return fractional.length();
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

        decimals = 0;
        QVector<CandleData> candles;
        QJsonArray arr = doc.array();
        for (auto val : arr) {
            QJsonArray c = val.toArray();
            CandleData candle;
            candle.openTime = static_cast<qint64>(c[0].toDouble());

            QString openStr  = c[1].toString();
            QString highStr  = c[2].toString();
            QString lowStr   = c[3].toString();
            QString closeStr = c[4].toString();

            candle.open  = openStr.toDouble();
            candle.high  = highStr.toDouble();
            candle.low   = lowStr.toDouble();
            candle.close = closeStr.toDouble();
            candle.volume= c[5].toString().toDouble();

            decimals = std::max({decimals,
                                 countSignificantDecimals(openStr),
                                 countSignificantDecimals(highStr),
                                 countSignificantDecimals(lowStr),
                                 countSignificantDecimals(closeStr)});

            candles.append(candle);
        }
        emit candlesDecimals(decimals);
        emit candlesReceived(candles);
    });
}
