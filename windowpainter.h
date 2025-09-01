#ifndef WINDOWPAINTER_H
#define WINDOWPAINTER_H

#include <QWidget>

#include "candledata.h"
#include "config.h"

#define TEXT_AREA           30
#define VOLUME_AREA         50
#define TEXT_SPACING        10
#define CANDLE_SPACING_X    5
#define CANDLE_SPACING_Y    20

class WindowPainter : public QWidget {
    Q_OBJECT
public:
    explicit WindowPainter(QWidget *parent = nullptr);

    void setConfig(Config &config);
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
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *) override;

private:
    Config config;
    QColor beakgroundColor;
    QColor candleBullColor;
    QColor candleBearColor;
    QColor volumeBullColor;
    QColor volumeBearColor;
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
    QPoint mousePos;
    double maxPrice;
    double minPrice;
    double maxVolume;
    double currentPrice;
    int chartHeight;

    void paintCandles(QPainter &p);
    void drawLabeledLine(QPainter &p, Qt::PenStyle style, int chartHeight, double value,
                         double minPrice, double maxPrice,
                         int w, QColor lineColor, QColor textColor,
                         const QString &label, int xOffset = 10);
};

#endif // WINDOWPAINTER_H
