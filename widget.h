#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

private:
    Ui::Widget *ui;

    short* m_pImageData3d;
    short* m_ptiefenkarte;

    int getIndex(int x, int y);
    int windowing(int HU_value, int center, int width, int &greyValue);
    void updateSliceView(QImage& image, short* imageData);
    int calculateDepthBuffer(short* inputData, int width, int height, int layers, int threshold, short* depthBuffer);

private slots:
    void load_3d();
    void updatedWindowingCenter(int center);
    void updatedWindowingWidth(int width);
    void updatedThreshold(int width);
    void updatedLayer(int layer);
    void onDepthBufferClicked();
};
#endif // WIDGET_H
