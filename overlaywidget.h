#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QWidget>
#include "windowpainter.h"
#include "binanceclient.h"

class OverlayWidget : public QWidget {
    Q_OBJECT
public:
    OverlayWidget();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool dragging = false;
    QPoint dragStart;
    WindowPainter *winPainter;
    BinanceClient *client;
};

#endif // OVERLAYWIDGET_H
