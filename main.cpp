#include <QApplication>
#include "overlaywidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    OverlayWidget w;
    w.show();
    return app.exec();
}

#include "main.moc"
