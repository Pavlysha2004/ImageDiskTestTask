#include "viewingimage.h"
#include "ui_viewingimage.h"
#include "QTimer"

ViewingImage::ViewingImage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ViewingImage)
    , scene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ViewingImage::~ViewingImage()
{
    delete scene;
    delete ui;
}

void ViewingImage::ShowImage(const QPixmap &Pixmap)
{
    if (Items)
    {
        scene->removeItem(Items);
        delete Items;
        Items = nullptr;
    }
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(Pixmap);
    scene->addItem(pixmapItem);
    Items = pixmapItem;
    QTimer::singleShot(0, this, [=]{
        ui->graphicsView->fitInView(Items, Qt::KeepAspectRatio);
        ui->graphicsView->centerOn(pixmapItem);
        ui->graphicsView->update();
    });
    this->show();
}

void ViewingImage::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (Items)
    {
        ui->graphicsView->fitInView(Items, Qt::KeepAspectRatio);
    }
}
