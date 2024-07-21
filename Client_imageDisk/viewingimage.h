#ifndef VIEWINGIMAGE_H
#define VIEWINGIMAGE_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QCloseEvent>
#include <QGraphicsPixmapItem>

namespace Ui {
class ViewingImage;
}

class ViewingImage : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewingImage(QWidget *parent = nullptr);
    ~ViewingImage();

protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    Ui::ViewingImage *ui;
    QGraphicsScene *scene;
    QGraphicsItem* Items;

public slots:
    void ShowImage(const QPixmap &Pixmap);
};

#endif // VIEWINGIMAGE_H
