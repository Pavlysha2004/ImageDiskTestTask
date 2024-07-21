#ifndef SERVERDISK_H
#define SERVERDISK_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QGraphicsPixmapItem>
#include <QListWidgetItem>
#include <QDir>
#include <vector>
#include <QDataStream>
#include <QByteArray>
#include <QString>
#include <QCoreApplication>
#include <QBuffer>
#include "ImageInfo.h"
#include "ModesDisk.h"
using namespace std;

class ServerDisk : public QTcpServer
{
    Q_OBJECT
public:
    ServerDisk();
    ~ServerDisk();
    QTcpSocket* socket;

private:
    QString FolderName = "RootFolder", FolderRootWay;
    QDir *RootDir;
    QStringList filters;
    QString permission;
    int FoldersNumber = 0;
    int NumberPixmap = 0;
    bool ReadySendPixmap = 0, ReadySendItems = 0;
    int quantityMess;
    vector <ImageInfo> ListPixmap;
    vector <QListWidgetItem*> ListItems;
    quint64 NextBlockSize;

    QByteArray Data;
    void SendToClient(int mode);

    void UpdateFolder();
    void deleteImage(int);
    void createFolder(QString);
    void OpenFolder(int);
    void DeleteFolder(int);
    void removeRecurs(const QString &path);
    void BackFolder();
    void BackRootFolder();
    void AddImage(QByteArray, QString, QPixmap, int);

public slots:
    void incomingConnection(qintptr socketDescriptor) override;
    void slotReadyRead();

};

#endif // SERVERDISK_H
