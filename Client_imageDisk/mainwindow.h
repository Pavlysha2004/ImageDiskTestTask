#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <vector>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QTcpSocket>
#include <QImageReader>
#include <QBuffer>
#include "custom_qlistwidget.h"
#include "viewingimage.h"
#include "ImageInfo.h"
#include "ModesDisk.h"

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QTcpSocket *socket;
    QByteArray Data;
    quint64 NextBlockSize;
    int NumberPixmap = 0;
    bool PixmapLoad = 0;
    QString permission;

    void SendToServer(int mode, int numberItem);
    void SendToServer(int mode, QFileInfo,QPixmap, int);
    void SendToServer(int mode, QString);
    void SendToServer(int mode);

    ViewingImage* ImageShow;
    QListWidgetItem *currentItem;
    vector <ImageInfo> ListPixmap;
    vector <QListWidgetItem*> ListItem;
    int FoldersNumber;
    QStringList filters;
    void UpdateFolder();


private slots:
    void showContextMenu(const QPoint &pos);
    void ImageLoaded(QString FilePatch, QFileInfo, int);
    void viewImage();
    void downloadImage();
    void deleteImage();
    void handleImageClick(QListWidgetItem *item);
    void FileDialogLoadImage();
    void createFolder();
    void OpenFolder();
    void DeleteFolder();
    void on_BackFolder_clicked();

    void on_BackRoot_clicked();

public slots:
    void slotReadyRead();

signals:
    void imageClicked(const QPixmap &Pixmap);
    void ImageSignalLoad(QString, QFileInfo, int);
};

#endif // MAINWINDOW_H

