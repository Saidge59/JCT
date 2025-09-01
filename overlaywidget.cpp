#include <QTimer>
#include <QApplication>
#include <QScreen>
#include <QMouseEvent>

#include "overlaywidget.h"
#include "config.h"

#define CONFIG_FILE "config.ini"

OverlayWidget::OverlayWidget() {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    resize(400, 300);
    setWindowOpacity(0.9);

    QIcon icon(":/appicon.ico");
    setWindowIcon(icon);

    Config config;
    config.LoadSettings(CONFIG_FILE);

    winPainter = new WindowPainter(this);
    winPainter->setGeometry(0, 0, width(), height());
    winPainter->setConfig(config);

    client = new BinanceClient(this);
    connect(client, &BinanceClient::candlesReceived, winPainter, &WindowPainter::setCandles);
    connect(winPainter, &WindowPainter::intervalChanged, this, [this](const QString &interval) {
        client->fetchCandles(winPainter->getCurrentSymbol(), interval, winPainter->getCurrentLimit());
    });
    connect(winPainter, &WindowPainter::limitChanged, this, [this](int limit) {
        client->fetchCandles(winPainter->getCurrentSymbol(), winPainter->getCurrentInterval(), limit);
    });
    connect(winPainter, &WindowPainter::symbolChanged, this, [this](const QString &symbol) {
        client->fetchCandles(symbol, winPainter->getCurrentInterval(), winPainter->getCurrentLimit());
    });

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        client->fetchCandles(winPainter->getCurrentSymbol(), winPainter->getCurrentInterval(), winPainter->getCurrentLimit());
    });
    timer->start(5000);

    client->fetchCandles(winPainter->getCurrentSymbol(), winPainter->getCurrentInterval(), winPainter->getCurrentLimit());

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
