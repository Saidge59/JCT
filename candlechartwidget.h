#ifndef CANDLECHARTWIDGET_H
#define CANDLECHARTWIDGET_H

#include <QWidget>
#include "candledata.h"

class CandleChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit CandleChartWidget(QWidget *parent = nullptr);

    void setCandles(const QVector<CandleData> &data);

    QString getCurrentInterval() const { return intervals[currentIntervalIndex]; }
    int getCurrentLimit() const { return currentLimit; }
    QString getCurrentSymbol() const { return symbol; }

signals:
    void intervalChanged(const QString &interval);
    void limitChanged(int limit);
    void symbolChanged(const QString &symbol);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<CandleData> candles;
    QStringList intervals;
    int currentIntervalIndex;
    int currentLimit;
    QString symbol;
    QRect symbolRect;
    QRect intervalRect;
    QRect limitRect;
    QRect betRect;
    double newBet = 0;
};

#endif // CANDLECHARTWIDGET_H
