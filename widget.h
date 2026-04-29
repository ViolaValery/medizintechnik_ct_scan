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

    short* m_pIMageData;

    int getIndex(int x, int y);
    int windowing(int HU_value, int center, int width, int &greyValue);
    void updateSliceView();

private slots:
    void load_image();
    void load_12bitimage();
    void updatedWindowingCenter(int center);
    void updatedWindowingWidth(int width);
};
#endif // WIDGET_H
