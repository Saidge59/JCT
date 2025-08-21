#include "overlaywidget.h"
#include <QTimer>
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>

OverlayWidget::OverlayWidget() {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(400, 300);
    setWindowOpacity(0.9);

    QIcon icon(":/appicon.ico");
    setWindowIcon(icon);

    chart = new CandleChartWidget(this);
    chart->setGeometry(0, 0, width(), height());

    client = new BinanceClient(this);
    connect(client, &BinanceClient::candlesReceived, chart, &CandleChartWidget::setCandles);
    connect(chart, &CandleChartWidget::intervalChanged, this, [this](const QString &interval) {
        client->fetchCandles(chart->getCurrentSymbol(), interval, chart->getCurrentLimit());
    });
    connect(chart, &CandleChartWidget::limitChanged, this, [this](int limit) {
        client->fetchCandles(chart->getCurrentSymbol(), chart->getCurrentInterval(), limit);
    });
    connect(chart, &CandleChartWidget::symbolChanged, this, [this](const QString &symbol) {
        client->fetchCandles(symbol, chart->getCurrentInterval(), chart->getCurrentLimit());
    });

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        client->fetchCandles(chart->getCurrentSymbol(), chart->getCurrentInterval(), chart->getCurrentLimit());
    });
    timer->start(5000);

    client->fetchCandles(chart->getCurrentSymbol(), chart->getCurrentInterval(), chart->getCurrentLimit()); // сразу при старте

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect avail = screen->availableGeometry();
    move(avail.right() - width(), avail.bottom() - height());
}

void OverlayWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = true;
        dragStart = event->globalPos() - frameGeometry().topLeft();
    }
}

void OverlayWidget::mouseMoveEvent(QMouseEvent *event) {
    if (dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPos() - dragStart;
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect availGeometry = screen->availableGeometry();

        if (newPos.x() < availGeometry.left())
            newPos.setX(availGeometry.left());
        if (newPos.y() < availGeometry.top())
            newPos.setY(availGeometry.top());
        if (newPos.x() + width() > availGeometry.right())
            newPos.setX(availGeometry.right() - width());
        if (newPos.y() + height() > availGeometry.bottom())
            newPos.setY(availGeometry.bottom() - height());

        move(newPos);
    }
}

void OverlayWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging = false;
    }
}
