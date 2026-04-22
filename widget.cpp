#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Aufgabe 1
    connect(ui->pushButton_pixel, SIGNAL(clicked()), this, SLOT(horizontalRed()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::MalePixel(){
    QImage image(512, 512, QImage::Format_RGB32);
    image.fill(qRgb(0,0,0));
    image.setPixel(50,100,qRgb(255,0,0));

    qDebug() << "Image is null: " << image.isNull();

    // Bild auf Benutzeroberflaeche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

void Widget::horizontalRed(){
    QImage image(512, 512, QImage::Format_RGB32);
    image.fill(qRgb(0,0,0));
    image.setPixel(50,100,qRgb(255,0,0));
    image.
    qDebug() << "Image is null: " << image.isNull();

    // Bild auf Benutzeroberflaeche anzeigen
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}