#include "mainwindow.h"
#include <QApplication>
#include "gatortext.h"
#include <QWebEngineView>

QString loadStyleSheet();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //prepares the web engine for opengl which is what the web browser uses
    a.setAttribute(Qt::AA_UseSoftwareOpenGL);
   // a.setStyleSheet(loadStyleSheet());
    GatorText gatorText;
    gatorText.show();
    return a.exec();
}

//test function
//just wanted to experiment
//usually done through the codeeditor
QString loadStyleSheet() {
    QFile stylesheet(":/style.qss");
    stylesheet.open(QFile::ReadOnly);
    QTextStream stream(&stylesheet);
    return stream.readAll();
}
