#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QElapsedTimer>
#include <QDirIterator>
#include <cmath>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //---------------------------------------------------------------------
    // ------------------------- Style ------------------------------------
    this->setAttribute(Qt::WA_StyledBackground, true);

    // Placeholder image
    QImage placeholder(512, 512, QImage::Format_RGB32);
    placeholder.fill(qRgb(13, 11, 21));

    QPainter painter(&placeholder);
    painter.setPen(QPen(QColor(100, 60, 140)));
    painter.setFont(QFont("Courier New", 36, QFont::Bold));
    painter.drawText(placeholder.rect(), Qt::AlignCenter, "NO IMAGE");
    painter.end();

    ui->label_image->setPixmap(QPixmap::fromImage(placeholder));
    // ------------------------- Style ------------------------------------
    //---------------------------------------------------------------------

    m_pImageData3d = new short[512*512*130];
    m_pDepthMap = new short[512*512];
    m_pDepthMap_diffuse = new short[512*512];

    connect(ui->horizontalSlider_center, SIGNAL(valueChanged(int)), this, SLOT(updatedWindowingCenter(int)));
    connect(ui->horizontalSlider_width, SIGNAL(valueChanged(int)), this, SLOT(updatedWindowingWidth(int)));
    connect(ui->pushButton_3d, SIGNAL(clicked()), this, SLOT(load_3d()));
    connect(ui->verticalSlider_layers, SIGNAL(valueChanged(int)), this, SLOT(updatedLayer(int)));
    connect(ui->horizontalSlider_threshold, SIGNAL(valueChanged(int)), this, SLOT(updatedThreshold(int)));
    connect(ui->pushButton_render3d, SIGNAL(clicked()), this, SLOT(onDepthBufferClicked()));
}

Widget::~Widget()
{
    delete ui;
    delete[] m_pImageData3d;
}

int Widget::windowing(int HU_value, int center, int width, int &igreyValue){
    double low_end_window = (double)center-(double)width/2;
    double high_end_window = (double)center+(double)width/2;

    if(HU_value < low_end_window){
        igreyValue = 0.0;
    }else if(HU_value > ui->horizontalSlider_threshold->value()){
        return 50; // not pretty
    }else if(HU_value > high_end_window){
        igreyValue = 255.0;
    }else{
        // calculate the grey value
        // -1024 bis +3071 auf 0 bis 255
        igreyValue = ((HU_value - low_end_window) / (double)width * 255.0);
    }
    return 0;
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------- Aufgabe 3.1 ----------------------------------------------
//---------------------------------------------------------------------------------------------------------

void Widget::updatedWindowingCenter(int value)
{
    ui->label_center->setText("Center value: " + QString::number(value));
    QImage image(512, 512, QImage::Format_RGB32);
    updateSliceView(image, m_pImageData3d);
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

void Widget::updatedWindowingWidth(int value)
{
    ui->label_width->setText("Width value: " + QString::number(value));
    QImage image(512, 512, QImage::Format_RGB32);
    updateSliceView(image, m_pImageData3d);
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

void Widget::updatedThreshold(int value)
{
    ui->label_threshold->setText("Threshold: " + QString::number(value));
    QImage image(512, 512, QImage::Format_RGB32);
    updateSliceView(image, m_pImageData3d);
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

void Widget::updatedLayer(int value)
{
    QImage image(512, 512, QImage::Format_RGB32);
    updateSliceView(image, m_pImageData3d);
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

void Widget::updateSliceView(QImage &image, short* imageData){

    //QImage image(512, 512, QImage::Format_RGB32);
    image.fill(qRgb(0,0,0));

    int center = ui->horizontalSlider_center->value();
    int width = ui->horizontalSlider_width->value();

    int igreyValue;
    int index;
    int layer = ui->verticalSlider_layers->value(); // 3d images
    if(layer >= 130){
        qDebug() << "Error: Layer slider was set too high (>130).";
        return;
    }

    // Read and set greyscale value of index at position x, y in image
    for(int j = 0; j<image.height(); j++){
        // For every layer add number of pixels to index
        // index = j*512, number of pixels for every layer = 512*512*layer
        index = layer*image.width()*image.height() + j*image.width();
        for(int i = 0; i<image.width(); i++){
            int r = windowing(imageData[index+i], center, width, igreyValue);
            if(r == 50){
                image.setPixel(i,j,qRgb(255.0, 0.0, 0.0));
            }else{
                image.setPixel(i,j,qRgb(igreyValue, igreyValue, igreyValue));
            }
        }
    }
}

void Widget::load_3d(){
    QString imagePath = QFileDialog::getOpenFileName(this, "Please provide the 12 bit image", "./",
                                                     "Raw Image Files (*.raw)");
    QFile dataFile(imagePath);

    // Open the file with openMode ReadOnly and compare
    if (!dataFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Error", "Data could not be read.");
        return;
    }

    if (!(dataFile.size() == dataFile.read((char*)m_pImageData3d, 512*512*130*sizeof(short))))
    {
        QMessageBox::critical(this, "Error", "Raw data not equal to read data.");
        return;
    }

    //dataFile.read(imageData, 512*512);
    dataFile.close();

    QImage image(512, 512, QImage::Format_RGB32);
    updateSliceView(image, m_pImageData3d);
    ui->label_image->setPixmap(QPixmap::fromImage(image));
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------- Aufgabe 4 ----------------------------------------------
//---------------------------------------------------------------------------------------------------------

void Widget::onDepthBufferClicked(){

    int threshold = ui->horizontalSlider_threshold->value();

    calculateDepthBuffer(m_pImageData3d, 512, 512, 130, threshold, m_pDepthMap);
    renderDepthBuffer(m_pDepthMap, 512, 512, m_pDepthMap_diffuse);

    QImage image(512, 512, QImage::Format_RGB32);
    for(int j = 0; j < 512; j++){
        for(int i = 0; i < 512; i++){
            int grey = (int)((m_pDepthMap_diffuse[j * 512 + i]));
            grey = qBound(0, grey, 255);
            image.setPixel(i, j, qRgb(grey, grey, grey));
        }
    }

    ui->label_depthBuffer->setPixmap(QPixmap::fromImage(image));
}

int Widget::calculateDepthBuffer(short* inputData, int width, int height, int layers, int threshold, short* depthBuffer){
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            depthBuffer[j * width + i] = 0;

            for(int k = 0; k < layers; k++){
                if(inputData[k * width * height + j * width + i] >= threshold){
                    depthBuffer[j * width + i] = k;
                    break;
                }
            }
        }
    }
    return 0;
}

int Widget::renderDepthBuffer(const short* depthBuffer, int width, int height, short* shadedBuffer){
    for (int j = 1; j < height-1; j++) {
        for (int i = 1; i < width-1; i++) {
            shadedBuffer[j * width + i] = 0;

            int left_neighbour = depthBuffer[j*width + i-1];
            int right_neighbour = depthBuffer[j*width + i+1];
            int upper_neighbour = depthBuffer[(j-1)*width + i];
            int lower_neighbour = depthBuffer[(j+1)*width + i];
            int Tx = left_neighbour - right_neighbour;
            int Ty = upper_neighbour - lower_neighbour;

            int divisor = (2*Tx)*(2*Tx) + (2*Ty)*(2*Ty) + 16;
            if(divisor <= 0) shadedBuffer[j*width + i] = 0;
            else shadedBuffer[j*width + i] = 255*(4/sqrt(divisor));
        }
    }
    return 0;
}