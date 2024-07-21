#include "serverdisk.h"

/**
 * @brief конструктор класса
 */
ServerDisk::ServerDisk()
{
    this->listen(QHostAddress::Any, 2323);
    QFile file(":/conf/Conf/Config");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int i = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (i == 0)
            {
                permission = line;
            }
            else
            {
                filters << line;
            }
            i++;
        }
        file.close();
    }
    RootDir = new QDir(QCoreApplication::applicationDirPath());
    if (!RootDir->exists(FolderName))
        RootDir->mkdir(FolderName);
    RootDir->cd(FolderName);
    FolderRootWay = RootDir->path();
}

/**
 * @brief диструктор класса
 */
ServerDisk::~ServerDisk()
{
    delete RootDir;
    for (int i = 0; i < static_cast<int>(ListItems.size()); i++)
    {
        delete ListItems[i];
    }
}

/*обновление списка изображений и списка vector <QListWidgetItem*> ListItems
 * чтобы они соответствовали данным из текущей папки. ListPixmap хранит все изображения
 * из текущей папки, ListItems хранит все объекты в папке в виде иконок и их названий*/
/**
 * @brief Обновление списка изображаний
 */
void ServerDisk::UpdateFolder()
{
    for (int i = 0; i < static_cast<int>(ListItems.size()); i++)
    {
        delete ListItems[i];
    }
    ListItems.clear();
    ListPixmap.clear();
    FoldersNumber = 0;

    QStringList folderList = RootDir->entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QPixmap TPixmapFolder(":/iconServer/Icon/folderIcon.png");
    for (int i = 0; i < folderList.size(); i++) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(TPixmapFolder.scaled(100, 100, Qt::KeepAspectRatio)), folderList[i]);
        item->setData(Qt::UserRole, "Folder");
        ListItems.push_back(item);
        FoldersNumber++;
    }

    RootDir->setNameFilters(filters);
    QStringList imageFiles = RootDir->entryList(QDir::Files);

    for (int i = 0; i < imageFiles.size(); i++) {
        QString filePath = RootDir->filePath(imageFiles[i]);
        QPixmap TPixmapImage(filePath);
        if (!TPixmapImage.isNull()) {
            QFileInfo fileInfo(imageFiles[i]);
            QString baseName = fileInfo.fileName();
            QListWidgetItem *item = new QListWidgetItem(QIcon(TPixmapImage.scaled(100, 100, Qt::KeepAspectRatio)), baseName);
            item->setData(Qt::UserRole, "Image");
            ListItems.push_back(item);
            ImageInfo TImage(fileInfo.fileName(),TPixmapImage);
            ListPixmap.push_back(TImage);
        }
    }
    RootDir->setNameFilters(QStringList());
}

/**
 * @brief Удаление изображения по индексу, который был прислан от клиента
 * @param row индекс в списке айтемов
 */
void ServerDisk::deleteImage(int row)
{
    QString filePath = RootDir->filePath(ListPixmap[row - FoldersNumber].NameFile);
    QFile::remove(filePath);
    ListPixmap.erase(ListPixmap.begin() + row);
    UpdateFolder();
}

/**
 * @brief Создание новой папки
 * @param NameFolder Название папки
 */
void ServerDisk::createFolder(QString NameFolder)
{
    RootDir->mkdir(NameFolder);
    UpdateFolder();
}

/**
 * @brief Открытие папки по индексу
 * @param row индекс в списке айтемов
 */
void ServerDisk::OpenFolder(int row)
{
    RootDir->cd(ListItems[row]->text());
    UpdateFolder();
}

/**
 * @brief Удаление папки
 * @param row индекс в списке айтемов
 */
void ServerDisk::DeleteFolder(int row)
{
    QString FilePatch = RootDir->filePath(ListItems[row]->text());
    removeRecurs(FilePatch);
    UpdateFolder();
}

/**
 * @brief Метод для удаления папки, которая рекурсивно удаляет всё содержимое папки
 * @param path Адрес папки
 */
void ServerDisk::removeRecurs(const QString &path)
{
    QDir dir(path);

    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList items = dir.entryInfoList();

    for (int i =0; i < items.size(); i++) {
        if (items[i].isDir()) {
            removeRecurs(items[i].absoluteFilePath());
        }
        else {
            if (!QFile::remove(items[i].absoluteFilePath())) {
                return;
            }
        }
    }

    dir.rmdir(path);
}

/**
 * @brief Возвращение назад в папку родителя
 */
void ServerDisk::BackFolder()
{
    if (RootDir->path() != FolderRootWay)
    {
        RootDir->cdUp();
        UpdateFolder();
    }
}

/**
 * @brief Возвращение в корневую папку
 */
void ServerDisk::BackRootFolder()
{
    RootDir->setPath(FolderRootWay);
    UpdateFolder();
}

/**
 * @brief Загрузка изображения от клиента
 *@param fileformat формат файла
 *@param filename название файла
 *@param pixmap карта изображения
 *@param size количество загружаемых картинок
 */
void ServerDisk::AddImage(QByteArray fileformat, QString filename, QPixmap pixmap, int size)
{
    if (!pixmap.isNull()) {
        QString filePath = RootDir->filePath(filename);
        if (!QFileInfo::exists(filePath))
        {
            pixmap.save(filePath, fileformat.constData());
            quantityMess++;
            if (quantityMess == size)
            {
                quantityMess = 0;
                UpdateFolder();
            }
        }
    }
}

/**
 * @brief Метод для обработки подключения клиента
 * @param socketDescriptor Дескриптор сокета
 */
void ServerDisk::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &ServerDisk::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    UpdateFolder();
    SendToClient(ModeDisk::SendConfig);
}

/*Более подробно подобный слот описан в
 *Client_ImageDisk. Пояснение к послежнему пункту ModeDisk::ReadyGetNewMess:
 *После того, как клиент считал данные, если нужно, он отправляет сообщение о том,
 *что готов принять новое сообщение, и там есть флаг ReadySendPixmap который говорит
 *о том, что идёт загрузка изображений клиенту. В этот момент через флаг ReadySendItems
 *отключается возможность отправлять список QListWidgetItem клиенту, чтобы информация отправлялась
 *как надо. Когда все изображения отправятся, флаг ReadySendPixmap примет false и отправка изображений
 *прекратиться.
*/

/**
 * @brief Прочтение данных полученных от клиента.
 */
void ServerDisk::slotReadyRead()
{
    socket = qobject_cast<QTcpSocket *>(sender());
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);
    if(in.status() == QDataStream::Ok)
    {
        int mode;
        QString str;
        QByteArray fileFormat;
        int row;
        QPixmap pixmap;
        for(;;)
        {
            if(NextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2)
                    break;

                in >> NextBlockSize;
            }
            if (socket->bytesAvailable() < NextBlockSize)
                break;

            in >> mode;

            switch (mode) {
            case ModeDisk::ImageLoadToServer:
            {
                int size;
                in >> fileFormat >> str >> pixmap >> size;
                AddImage(fileFormat, str, pixmap, size);
                if (quantityMess == 0)
                    ReadySendItems = 1;
                else
                    ReadySendItems = 0;
                break;
            }
            case ModeDisk::DeleteImage:
            {
                in >> row;
                deleteImage(row);
                ReadySendItems = 1;
                break;
            }
            case ModeDisk::DeleteFolder:
            {
                in >> row;
                DeleteFolder(row);
                ReadySendItems = 1;
                break;
            }
            case ModeDisk::createFolder:
            {
                in >> str;
                createFolder(str);
                ReadySendItems = 1;
                break;
            }
            case ModeDisk::OpenFolder:
            {
                in >> row;
                OpenFolder(row);
                ReadySendItems = 1;
                break;
            }
            case ModeDisk::BackFolder:
            {
                BackFolder();
                ReadySendItems = 1;
                break;
            }
            case ModeDisk::BackRoot:
            {
                BackRootFolder();
                ReadySendItems = 1;
                break;
            }
            case ModeDisk::ReadyGetNewMess:
            {
                int DoMode;
                in >> DoMode;
                if(DoMode == ModeDisk::SendPixmap)
                {
                    if (ReadySendPixmap)
                    {
                        SendToClient(ModeDisk::SendPixmap);
                    }
                    ReadySendItems = 0;
                }
                else if (DoMode == ModeDisk::SendConfig)
                {
                    ReadySendItems = 1;
                }
                break;
            }
            default:
                break;
            }
            if (ReadySendItems)
            {
                SendToClient(ModeDisk::SendItems);
            }
            NextBlockSize = 0;
            break;
        }
    }
}

 /* Стоит обратить внимание, что после SendItems устанавливается
 * флаг, для того, чтобы произошла после отправки QListWidgetItem отправка клиенту списка
 * изображений
*/

/**
 * @brief Отправка сообщения клиенту.
 * @param mode Режим работы
 */
void ServerDisk::SendToClient(int mode)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    switch (mode) {
    case ModeDisk::SendItems:
    {
        out << quint64(0) << mode;
        out << FoldersNumber;
        out << static_cast<int>(ListItems.size());

        for (int i = 0; i < static_cast<int>(ListItems.size()); i++) {
            out << ListItems[i]->text();
            out << ListItems[i]->icon();
            out << ListItems[i]->data(Qt::UserRole).toString();
        }
        ReadySendPixmap = 1;
        ReadySendItems  = 0;
        break;
    }
    case ModeDisk::SendPixmap:
    {
        out << quint64(0) << mode;
        out << static_cast<int>(ListPixmap.size());
        int stop = NumberPixmap;
        for (NumberPixmap; NumberPixmap < static_cast<int>(ListPixmap.size()) && NumberPixmap < stop + 5 ; NumberPixmap++) {
            out << ListPixmap[NumberPixmap].NameFile;
            out << ListPixmap[NumberPixmap].Image;
        }
        if (NumberPixmap == static_cast<int>(ListPixmap.size()))
        {
            ReadySendPixmap = 0;
            NumberPixmap = 0;
        }
        break;
    }
    case ModeDisk::SendConfig:
    {
        out << quint64(0) << mode << filters << permission;
        break;
    }
    default:
        break;
    }
    out.device()->seek(0);
    out << quint64(Data.size() - sizeof(quint64));
    socket->write(Data);
}
