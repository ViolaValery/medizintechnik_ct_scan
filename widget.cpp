#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QElapsedTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Placeholder image
    QImage placeholder(512, 512, QImage::Format_RGB32);
    placeholder.fill(qRgb(30, 30, 30));

    QPainter painter(&placeholder);
    painter.setPen(QPen(QColor(180, 180, 180)));
    painter.setFont(QFont("Courier New", 36, QFont::Bold));
    painter.drawText(placeholder.rect(), Qt::AlignCenter, "NO IMAGE");
    painter.end();

    ui->label_image->setPixmap(QPixmap::fromImage(placeholder));

    // Aufgabe 2.1
    connect(ui->pushButton_pixel, SIGNAL(clicked()), this, SLOT(load_image()));

    // Aufgabe 2.2
    m_pIMageData = new short[512*512];
    connect(ui->pushButton_load12bit, SIGNAL(clicked()), this, SLOT(load_12bitimage()));

    // Aufgabe 3.1
    connect(ui->horizontalSlider_center, SIGNAL(valueChanged(int)), this, SLOT(updatedWindowingCenter(int)));
    connect(ui->horizontalSlider_width, SIGNAL(valueChanged(int)), this, SLOT(updatedWindowingWidth(int)));
}

Widget::~Widget()
{
    delete ui;
    delete[] m_pIMageData;
}

void Widget::load_image(){
    QString imagePath = QFileDialog::getOpenFileName(this, "Please provide the image", "./",
                                                     "Raw Image Files (*.raw)");
    QFile dataFile(imagePath);

    char imageData[512*512];

    // Open the file with openMode ReadOnly and compare
    if (!dataFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Error", "Data could not be read.");
        return;
    }

    // Load data into a temporary 8-bit buffer to ensure that it is displayed correctly
    unsigned char tempData[512 * 512];

    if (!(dataFile.size() == dataFile.read((char*)tempData, 512*512*sizeof(unsigned char))))
    {
        QMessageBox::critical(this, "Error", "Raw data not equal to read data.");
        return;
    }

    //dataFile.read(imageData, 512*512);
    dataFile.close();

    // Copy 8-bit values (0-255) into short array
    for(int i = 0; i < 512 * 512; i++){
        m_pIMageData[i] = (short)tempData[i];
    }

    updateSliceView();
}

void Widget::load_12bitimage(){
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

    updateSliceView();
}

int Widget::windowing(int HU_value, int center, int width, int &igreyValue){
    double low_end_window = (double)center-(double)width/2;
    double high_end_window = (double)center+(double)width/2;

    if(HU_value < low_end_window){
        igreyValue = 0.0;
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
    updateSliceView();
}

void Widget::updatedWindowingWidth(int value)
{
    ui->label_width->setText("Width value: " + QString::number(value));
    updateSliceView();
}

void Widget::updateSliceView(){

    QElapsedTimer timer;
    timer.start();

    QImage image(512, 512, QImage::Format_RGB32);
    image.fill(qRgb(0,0,0));

    int center = ui->horizontalSlider_center->value();
    int width = ui->horizontalSlider_width->value();

    int igreyValue;
    int index;
    // Read and set greyscale value of index at position x, y in image
    //y*512+x
    for(int j = 0; j<511; j++){
        index = j*512;
        for(int i = 0; i<511; i++){
            windowing(m_pIMageData[index+i], center, width, igreyValue);
            image.setPixel(i,j,qRgb(igreyValue, igreyValue, igreyValue));
        }
    }

    // Show image on user interface
    ui->label_image->setPixmap(QPixmap::fromImage(image));

    qDebug() << "Elapsed time: " << timer.nsecsElapsed();
}