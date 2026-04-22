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

    // Aufgabe 2.1
    connect(ui->pushButton_pixel, SIGNAL(clicked()), this, SLOT(load_image()));

    // Aufgabe 2.2
    m_pIMageData = new short[512*512];
    connect(ui->pushButton_load12bit, SIGNAL(clicked()), this, SLOT(load_12bitimage()));
}

Widget::~Widget()
{
    delete ui;
    delete[] m_pIMageData;
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

void Widget::load_12bitimage(){
    QImage image(512, 512, QImage::Format_RGB32);
    image.fill(qRgb(0,0,0));

    QString imagePath = QFileDialog::getOpenFileName(this, "Please provide the 12 bit image", "./",
                                                     "Raw Image Files (*.raw)");
    QFile dataFile(imagePath);

    // Open the file with openMode ReadOnly and compare
    if (!dataFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Error", "Data could not be read.");
        return;
    }

    if (!(dataFile.size() == dataFile.read((char*)m_pIMageData, 512*512*sizeof(short))))
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
            int iGrauwert = windowing(m_pIMageData[getIndex(i, j)], 0, 800);
            image.setPixel(i,j,qRgb(iGrauwert, iGrauwert, iGrauwert));
        }
    }

    // Show image on user interface
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

int Widget::getIndex(int x, int y){
    return y*512+x;
}

int Widget::windowing(int HU_value, int center, int width){
    int iGrauwert;

    double low_end_window = (double)center-(double)width/2;
    double high_end_window = (double)center+(double)width/2;

    if(HU_value < low_end_window){
        iGrauwert = 0.0;
    }else if(HU_value > high_end_window){
        iGrauwert = 255.0;
    }else{
        // calculate the grey value
        // -1024 bis +3071 auf 0 bis 255
        iGrauwert = ((HU_value - low_end_window) / (double)width * 255.0);
    }

    return iGrauwert;
}