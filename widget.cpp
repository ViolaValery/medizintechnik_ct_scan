#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Aufgabe 2
    connect(ui->pushButton_pixel, SIGNAL(clicked()), this, SLOT(load_image()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::load_image(){

    QImage image(512, 512, QImage::Format_RGB32);
    image.fill(qRgb(0,0,0));

    char imageData[512*512];

    QString imagePath = QFileDialog::getOpenFileName(this, "Please provide the image", "./",
                                                     "Raw Image Files (*.raw)");
    QFile dataFile(imagePath);

    // Open the file with openMode ReadOnly and compare
    if (!dataFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Error", "Data could not be read.");
        return;
    }

    if (!(dataFile.size() == dataFile.read(imageData, 512*512)))
    {
        QMessageBox::critical(this, "Error", "Raw data not equal to read data.");
        return;
    }

    //dataFile.read(imageData, 512*512);
    dataFile.close();


    // Read and set greyscale value of index at position x, y in image
    for(int i = 0; i<511; i++){
        for(int j = 0; j<511; j++){
            //int index = getIndex(i, j);
            int iGrauwert = imageData[i*512+j];
            image.setPixel(i,j,qRgb(iGrauwert, iGrauwert, iGrauwert));
        }
    }

    // Show image on user interface
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

int getIndex(int x, int y){
    return y*512+x;
}