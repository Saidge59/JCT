#include "candlechartwidget.h"
#include <QInputDialog>
#include <QMouseEvent>
#include <QPainter>

CandleChartWidget::CandleChartWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(400, 300);
    intervals << "1s" << "1m" << "3m" << "5m" << "15m" << "30m"
              << "1h" << "2h" << "4h" << "6h" << "8h" << "12h"
              << "1d" << "3d" << "1w" << "1M";
    currentIntervalIndex = intervals.indexOf("15m");
    currentLimit = 100;
    symbol = "SOLUSDT";
}

void CandleChartWidget::setCandles(const QVector<CandleData> &data) {
    candles = data;
    update();
}

void CandleChartWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0, 128)); // фон

    if (candles.isEmpty()) return;

    int w = width();
    int h = height();
    int textArea = 30;
    int volumeArea = 50;
    int chartHeight = h - textArea - volumeArea;
    int textSpacing = 10;

    double maxPrice = candles[0].high;
    double minPrice = candles[0].low;
    double maxVolume = candles[0].volume;
    for (auto &c : candles) {
        if (c.high > maxPrice) maxPrice = c.high;
        if (c.low < minPrice) minPrice = c.low;
        if (c.volume > maxVolume) maxVolume = c.volume;
    }

    double candleWidth = static_cast<double>(w) / candles.size();

    int lastCloseY = 0;

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

        QColor color = (c.close >= c.open) ? Qt::green : Qt::red;
        p.setPen(color);
        p.setBrush(color);

        int wickX = x + static_cast<int>(candleWidth / 2);
        p.drawLine(wickX, yHigh, wickX, yLow);

        int bodyWidth = qMax(static_cast<int>(candleWidth / 2), 1);
        int bodyX = x + static_cast<int>(candleWidth / 4);
        int top = qMin(yOpen, yClose);
        int bottom = qMax(yOpen, yClose);
        p.drawRect(bodyX, top, bodyWidth, bottom - top);

        int volHeight = static_cast<int>((c.volume / maxVolume) * volumeArea);
        QRect volRect(x + static_cast<int>(candleWidth / 4), h - textArea - volHeight, bodyWidth, volHeight);
        p.fillRect(volRect, QColor(color.red(), color.green(), color.blue(), 128));

        lastCloseY = yClose;
    }

    if(newBet > 0 && newBet < maxPrice && newBet > minPrice) {
        QPen pen(Qt::yellow);
        p.setPen(pen);
        int yHigh  = chartHeight - static_cast<int>((newBet - minPrice) / (maxPrice - minPrice) * chartHeight);
        p.drawLine(0, yHigh, w, yHigh);
        QFont font("Arial", 8, QFont::Bold);
        p.setFont(font);
        QString priceText = QString(QString::number(newBet, 'f', 2));
        p.drawText(10, yHigh - 5, priceText);
    }

    QFont font("Arial", 14, QFont::Bold);
    p.setFont(font);
    QFontMetrics fm(font);

    const CandleData &last = candles.last();
    double currentPrice = last.close;
    double percent = (last.close - last.open) / last.open * 100.0;

    QString name = QString("%1").arg(symbol);
    p.setPen(Qt::white);
    symbolRect = QRect(10, h - textArea, fm.horizontalAdvance(name), textArea);
    p.drawText(textSpacing, h - 8, name);

    QString priceText = QString(QString::number(currentPrice, 'f', 2));
    int priceTextWidth = fm.horizontalAdvance(name);
    p.setPen(percent >= 0 ? Qt::green : Qt::red);
    p.drawText(priceTextWidth + (textSpacing * 2), h - 8, priceText);

    QPen pen(Qt::darkGray);
    pen.setStyle(Qt::DashLine);
    p.setPen(pen);
    p.drawLine(0, lastCloseY, w, lastCloseY);
    QFont oldFont = font;
    QFont font2("Arial", 8, QFont::Bold);
    p.setFont(font2);
    p.drawText(10, lastCloseY - 5, priceText);
    p.setFont(font);

    if(newBet > 0) {
        percent = std::abs(newBet - currentPrice) / currentPrice * 100.0;
        p.setPen(Qt::yellow);
        QString percentText = QString("%1%").arg(QString::number(percent, 'f', 2));
        int percentTextWidth = fm.horizontalAdvance(priceText);
        p.drawText(priceTextWidth + percentTextWidth + (textSpacing * 3), h - 8, percentText);
    }

    QString intervalText = QString("%1").arg(intervals[currentIntervalIndex]);
    QString limitText = QString("%1").arg(currentLimit);

    int betTextWidth = fm.horizontalAdvance(priceText);
    int intervalTextWidth = fm.horizontalAdvance(intervalText);
    int limitTextWidth = fm.horizontalAdvance(limitText);

    betRect = QRect(priceTextWidth + (textSpacing * 2), h - textArea, betTextWidth, textArea);
    intervalRect = QRect(w - intervalTextWidth - textSpacing, h - textArea, intervalTextWidth, textArea);
    limitRect = QRect(w - intervalTextWidth - limitTextWidth - (textSpacing * 2), h - textArea, limitTextWidth, textArea);

    p.setPen(Qt::white);
    p.drawText(intervalRect, Qt::AlignLeft | Qt::AlignVCenter, intervalText);
    p.drawText(limitRect, Qt::AlignLeft | Qt::AlignVCenter, limitText);
}

void CandleChartWidget::mousePressEvent(QMouseEvent *event) {
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
