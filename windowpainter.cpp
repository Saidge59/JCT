#include <QInputDialog>
#include <QMouseEvent>
#include <QPainter>

#include "windowpainter.h"

WindowPainter::WindowPainter(QWidget *parent) : QWidget(parent) {
    setMinimumSize(400, 300);
    intervals << "1s" << "1m" << "3m" << "5m" << "15m" << "30m"
              << "1h" << "2h" << "4h" << "6h" << "8h" << "12h"
              << "1d" << "3d" << "1w" << "1M";
    setMouseTracking(true);
}

void WindowPainter::setConfig(Config &conf) {
    config = conf;

    currentIntervalIndex = intervals.indexOf(config.intervalDef);
    if(currentIntervalIndex < 0) {
        qDebug() << "Wrong 'intervalDef'!";
    }
    currentLimit = config.limitCandleDef;
    symbol = config.symbolDef;

    beakgroundColor = config.beakgroundColor;

    candleBullColor = config.candleBull;
    candleBearColor = config.candleBear;
    candleBullColor.setAlpha(config.candleAlpha);
    candleBearColor.setAlpha(config.candleAlpha);

    volumeBullColor = config.volumeBull;
    volumeBearColor = config.volumeBear;
}

void WindowPainter::setCandles(const QVector<CandleData> &data) {
    candles = data;
    maxPrice = candles[0].high;
    minPrice = candles[0].low;
    maxVolume = candles[0].volume;
    for (auto &c : candles) {
        if (c.high > maxPrice) maxPrice = c.high;
        if (c.low < minPrice) minPrice = c.low;
        if (c.volume > maxVolume) maxVolume = c.volume;
    }

    const CandleData &last = candles.last();
    currentPrice = last.close;

    update();
}

void WindowPainter::paintCandles(QPainter &p) {
    double candleWidth = static_cast<double>(width() - CANDLE_SPACING_X*2) / candles.size();

    for (int i = 0; i < candles.size(); i++) {
        const CandleData &c = candles[i];
        int x = static_cast<int>(i * candleWidth);

        auto scaleY = [&](double price) {
            return chartHeight - static_cast<int>((price - minPrice) / (maxPrice - minPrice) * chartHeight);
        };

        int yHigh  = scaleY(c.high);
        int yLow   = scaleY(c.low);
        int yOpen  = scaleY(c.open);
        int yClose = scaleY(c.close);

        QColor color = (c.close >= c.open) ? candleBullColor : candleBearColor;
        p.setPen(color);
        p.setBrush(color);

        int wickX = CANDLE_SPACING_X + x + static_cast<int>(candleWidth / 2);
        p.drawLine(wickX, yHigh, wickX, yLow);

        int bodyWidth = qMax(static_cast<int>(candleWidth / 2), 1);
        int bodyX = CANDLE_SPACING_X + x + static_cast<int>(candleWidth / 4);
        int top = qMin(yOpen, yClose);
        int bottom = qMax(yOpen, yClose);
        p.drawRect(bodyX, top, bodyWidth, bottom - top);

        color = (c.close >= c.open) ? volumeBullColor : volumeBearColor;
        int volHeight = static_cast<int>((c.volume / maxVolume) * VOLUME_AREA);
        QRect volRect(CANDLE_SPACING_X + x + static_cast<int>(candleWidth / 4), height() - TEXT_AREA - volHeight, bodyWidth, volHeight);
        p.fillRect(volRect, QColor(color.red(), color.green(), color.blue(), config.volumeAlpha));
    }
}

void WindowPainter::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), QColor(beakgroundColor.red(), beakgroundColor.green(), beakgroundColor.blue(), config.beakgroundAlpha));

    if (candles.isEmpty()) return;

    int w = width();
    int h = height();
    chartHeight = h - TEXT_AREA - VOLUME_AREA;

    paintCandles(p);

    const CandleData &last = candles.last();
    double percent = (last.close - last.open) / last.open * 100.0;

    if(newBet > 0 && newBet < maxPrice && newBet > minPrice) {
        drawLabeledLine(p, Qt::SolidLine, chartHeight, newBet, minPrice, maxPrice,
                        w, config.linePositionColor, config.linePositionColor,
                        QString::number(newBet, 'f', 2));
    }

    if (rect().contains(mousePos)) {
        double rel = 1.0 - (double)mousePos.y() / chartHeight;
        double hoverPrice = minPrice + rel * (maxPrice - minPrice);
        double percent = std::abs(hoverPrice - currentPrice) / currentPrice * 100.0;

        if (hoverPrice < maxPrice && hoverPrice > minPrice) {
            drawLabeledLine(p, Qt::DotLine, chartHeight, hoverPrice, minPrice, maxPrice,
                            w, config.lineUserColor, config.lineUserColor,
                            QString("%1 (%2%)").arg(QString::number(hoverPrice, 'f', 2))
                                .arg(QString::number(percent, 'f', 2)), mousePos.x());
        }
    }

    drawLabeledLine(p, Qt::DashLine, chartHeight, currentPrice, minPrice, maxPrice,
                    w, config.lineCurrentPriceColor, config.lineCurrentPriceColor,
                    QString("%1").arg(QString::number(currentPrice, 'f', 2)));

    QFont font("Arial", 14, QFont::Bold);
    p.setFont(font);
    QFontMetrics fm(font);

    QString name = QString("%1").arg(symbol);
    p.setPen(Qt::white);
    symbolRect = QRect(10, h - TEXT_AREA, fm.horizontalAdvance(name), TEXT_AREA);
    p.drawText(TEXT_SPACING, h - 8, name);

    QColor bullColor(config.priceBull);
    QColor bearColor(config.priceBear);

    QString priceText = QString(QString::number(currentPrice, 'f', 2));
    int priceTextWidth = fm.horizontalAdvance(name);
    p.setPen(percent >= 0 ? bullColor : bearColor);
    p.drawText(priceTextWidth + (TEXT_SPACING * 2), h - 8, priceText);

    if(newBet > 0) {
        percent = std::abs(newBet - currentPrice) / currentPrice * 100.0;
        p.setPen(Qt::yellow);
        QString percentText = QString("%1%").arg(QString::number(percent, 'f', 2));
        int percentTextWidth = fm.horizontalAdvance(priceText);
        p.drawText(priceTextWidth + percentTextWidth + (TEXT_SPACING * 3), h - 8, percentText);
    }

    QString intervalText = QString("%1").arg(intervals[currentIntervalIndex]);
    QString limitText = QString("%1").arg(currentLimit);

    int betTextWidth = fm.horizontalAdvance(priceText);
    int intervalTextWidth = fm.horizontalAdvance(intervalText);
    int limitTextWidth = fm.horizontalAdvance(limitText);

    betRect = QRect(priceTextWidth + (TEXT_SPACING * 2), h - TEXT_AREA, betTextWidth, TEXT_AREA);
    intervalRect = QRect(w - intervalTextWidth - TEXT_SPACING, h - TEXT_AREA, intervalTextWidth, TEXT_AREA);
    limitRect = QRect(w - intervalTextWidth - limitTextWidth - (TEXT_SPACING * 2), h - TEXT_AREA, limitTextWidth, TEXT_AREA);

    p.setPen(Qt::white);
    p.drawText(intervalRect, Qt::AlignLeft | Qt::AlignVCenter, intervalText);
    p.drawText(limitRect, Qt::AlignLeft | Qt::AlignVCenter, limitText);
}

void WindowPainter::drawLabeledLine(QPainter &p, Qt::PenStyle style, int chartHeight, double value,
                                        double minPrice, double maxPrice,
                                        int w, QColor lineColor, QColor textColor,
                                        const QString &label, int xOffset)
{
    if (value <= minPrice || value >= maxPrice)
        return;

    int yHigh = chartHeight - static_cast<int>((value - minPrice) / (maxPrice - minPrice) * chartHeight);

    QPen pen(lineColor);
    pen.setStyle(style);
    p.setPen(pen);
    p.drawLine(0, yHigh, w, yHigh);

    QFont font("Arial", 8, QFont::Bold);
    p.setFont(font);

    QFontMetrics fm(p.fontMetrics());
    int tw = fm.horizontalAdvance(label);
    int th = fm.height();

    int x = xOffset;
    if (x + tw + 10 > width()) x = width() - tw - 10;

    QRect r(x, yHigh - th - 4, tw, th + 4);
    p.fillRect(r, QColor(0,0,0,150));

    p.setPen(textColor);
    p.drawText(r, Qt::AlignVCenter, label);
}


void WindowPainter::mousePressEvent(QMouseEvent *event) {
    if (symbolRect.contains(event->pos())) {
        bool ok;
        QString newSymbol = QInputDialog::getText(this, "Change Symbol",
                                                  "Enter new symbol:",
                                                  QLineEdit::Normal, symbol, &ok);
        if (ok && !newSymbol.isEmpty()) {
            if(symbol != newSymbol.toUpper()) newBet = 0;
            symbol = newSymbol.toUpper();
            emit symbolChanged(symbol);
            update();
        }
    } else if (intervalRect.contains(event->pos())) {
        if (event->button() == Qt::LeftButton) {
            currentIntervalIndex = (currentIntervalIndex + 1) % intervals.size();
        } else if (event->button() == Qt::RightButton) {
            currentIntervalIndex = (currentIntervalIndex - 1 + intervals.size()) % intervals.size();
        }
        emit intervalChanged(intervals[currentIntervalIndex]);
        update();
    } else if (limitRect.contains(event->pos())) {
        if (event->button() == Qt::LeftButton && currentLimit < 100) {
            currentLimit += 10;
        } else if (event->button() == Qt::RightButton && currentLimit > 10) {
            currentLimit -= 10;
        }
        emit limitChanged(currentLimit);
        update();
    } else if (betRect.contains(event->pos())) {
        bool ok;
        newBet = QInputDialog::getDouble(this, "Change Bet",
                                         "Enter new bet:",
                                         newBet, 0.0, 1000000.0, 2, &ok);
    } else {
        QWidget::mousePressEvent(event);
    }
}

void WindowPainter::mouseMoveEvent(QMouseEvent *event)
{
    mousePos = event->pos();
    update();
    QWidget::mousePressEvent(event);
}

void WindowPainter::leaveEvent(QEvent *) {
    mousePos = QPoint(-1, -1);
    update();
}
