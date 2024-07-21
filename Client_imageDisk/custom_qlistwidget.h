#ifndef CUSTOM_QLISTWIDGET_H
#define CUSTOM_QLISTWIDGET_H

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QWidget>
#include <QMimeData>
#include <QString>
#include <QFileDialog>
#include <QGraphicsView>
#include <QMessageBox>
#include <QListWidget>
#include <QStringList>

class Custom_QListWidget : public QListWidget
{
    Q_OBJECT
public:
    QStringList filters;
    explicit Custom_QListWidget(QWidget *parent);
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
signals:
    void ImageLoaded(QString filepath, QFileInfo, int);
};

#endif // CUSTOM_QLISTWIDGET_H
