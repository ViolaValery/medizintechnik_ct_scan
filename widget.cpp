#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QElapsedTimer>
#include <QDirIterator>

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


    // Aufgabe 3.1
    connect(ui->horizontalSlider_center, SIGNAL(valueChanged(int)), this, SLOT(updatedWindowingCenter(int)));
    connect(ui->horizontalSlider_width, SIGNAL(valueChanged(int)), this, SLOT(updatedWindowingWidth(int)));

    // Aufgabe 3.2
    m_pImageData3d = new short[512*512*130];

    // Aufgabe 3.3
    connect(ui->pushButton_3d, SIGNAL(clicked()), this, SLOT(load_3d()));
    connect(ui->verticalSlider_layers, SIGNAL(valueChanged(int)), this, SLOT(updatedLayer(int)));
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

void Widget::updatedLayer(int value)
{
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
    int layer = ui->verticalSlider_layers->value(); // 3d images
    if(layer >= 130){
        qDebug() << "Error: Layer slider was set too high (>130).";
        return;
    }
    // Read and set greyscale value of index at position x, y in image
    for(int j = 0; j<512; j++){
        // For every layer add number of pixels to index
        // index = j*512, number of pixels for every layer = 512*512*layer
        index = layer*512*512 + j*512;
        for(int i = 0; i<512; i++){
            windowing(m_pImageData3d[index+i], center, width, igreyValue);
            image.setPixel(i,j,qRgb(igreyValue, igreyValue, igreyValue));
        }
    }

    // Show image on user interface
    ui->label_image->setPixmap(QPixmap::fromImage(image));

    qDebug() << "Elapsed time: " << timer.nsecsElapsed();
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

    updateSliceView();
}
