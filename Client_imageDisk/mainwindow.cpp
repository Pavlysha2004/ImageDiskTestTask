#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
*@brief Конструктор класса способствует подключению к серверу
*@param parent указатель на родительский класс
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , ImageShow(new ViewingImage)
{

    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1", 2323);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    ui->setupUi(this);
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setIconSize(QSize(100, 100));
    connect(ui->listWidget, &Custom_QListWidget::ImageLoaded, this, &MainWindow::ImageLoaded);
    connect(this, &MainWindow::ImageSignalLoad, this, &MainWindow::ImageLoaded);
    connect(this, &MainWindow::imageClicked, ImageShow, &ViewingImage::ShowImage);
    connect(ui->listWidget, &Custom_QListWidget::itemDoubleClicked, this, &MainWindow::handleImageClick);
    connect(ui->listWidget, &Custom_QListWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    if (!socket->waitForConnected()) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to connect to host"));
        return;
    }
    UpdateFolder();
}


/**
*@brief Диструктор класса, удаление всех динамисеских элеметов
*/
MainWindow::~MainWindow()
{
    delete ImageShow;
    int itemCount = ui->listWidget->count();
    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem* item = ui->listWidget->takeItem(0);
        delete item;
    }
    ui->listWidget->clear();
    delete ui;
    ListPixmap.clear();
}


/**
*@brief Обновление всех QListWidgetItem в ui->listWidget
*/
void MainWindow::UpdateFolder()
{
    for (int i = 0; i < static_cast<int>(ListItem.size()); i++)
    {
        ui->listWidget->addItem(ListItem[i]);
    }
}


/**
 *@brief загружает в QPixmap и отправление информации о файле на сервер
 *@param FilePatch Место нахождения файла
 *@param fileInfo информация о файле
 *@param size количество загружаемых картинок
*/
void MainWindow::ImageLoaded(QString FilePatch, QFileInfo fileInfo, int size)
{
    if (permission == "allow")
    {
        if (!PixmapLoad)
        {
            QPixmap pixmap(FilePatch);
            if (!pixmap.isNull()) {
                QString basename = fileInfo.fileName();
                bool NamesMatched = 0;
                for (int i =0; i < static_cast<int>(ListPixmap.size()); i++)
                {
                    if (basename == ListPixmap[i].NameFile)
                    {
                        NamesMatched = 1;
                        break;
                    }
                }
                if (!NamesMatched)
                {
                    SendToServer(ModeDisk::ImageLoadToServer, fileInfo, pixmap, size);
                }
                else
                {
                    QMessageBox::warning(this, tr("Error"), tr("same file name"));
                    return;
                }
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Error"), tr("files loading"));
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("the image cannot be uploaded"));
    }
}


/**
 * @brief Метод вызывает контекстное меню, которое вызывается в соответствии с расположением мыши
 * @param pos Позиция курсора мыши
 */
void MainWindow::showContextMenu(const QPoint &pos)
{
    currentItem = ui->listWidget->itemAt(pos);
    QMenu contextMenu(tr("Menu"), this);
    if (currentItem)
    {
        if (currentItem->data(Qt::UserRole).toString() == "Image")
        {
            QAction actionView("Show image", this);
            connect(&actionView, &QAction::triggered, this, &MainWindow::viewImage);
            contextMenu.addAction(&actionView);

            QAction actionDownload("Download image", this);
            connect(&actionDownload, &QAction::triggered, this, &MainWindow::downloadImage);
            contextMenu.addAction(&actionDownload);

            QAction actionDelete("Delete image", this);
            connect(&actionDelete, &QAction::triggered, this, &MainWindow::deleteImage);
            contextMenu.addAction(&actionDelete);
            contextMenu.exec(ui->listWidget->mapToGlobal(pos));
        }
        else if (currentItem->data(Qt::UserRole).toString() == "Folder")
        {
            QAction actionOpenFolder("Open Folder", this);
            connect(&actionOpenFolder, &QAction::triggered, this, &MainWindow::OpenFolder);
            contextMenu.addAction(&actionOpenFolder);

            QAction actionDeleteFolder("Delete Folder", this);
            connect(&actionDeleteFolder, &QAction::triggered, this, &MainWindow::DeleteFolder);
            contextMenu.addAction(&actionDeleteFolder);
            contextMenu.exec(ui->listWidget->mapToGlobal(pos));
        }
    }
    else
    {
        QAction actionCreateFolder("Create Folder", this);
        connect(&actionCreateFolder, &QAction::triggered, this, &MainWindow::createFolder);
        contextMenu.addAction(&actionCreateFolder);

        QAction actionLoadImage("Load Image", this);
        connect(&actionLoadImage, &QAction::triggered, this, &MainWindow::FileDialogLoadImage);
        contextMenu.addAction(&actionLoadImage);

        contextMenu.exec(ui->listWidget->mapToGlobal(pos));
    }
}

/**
 * @brief Показ изображения в отдельном окне сами изображения подгружаются с сервера и хранятся у клиента в векторе vector<QPixmap>
*/
void MainWindow::viewImage()
{
    if(!PixmapLoad)
    {
        int row = ui->listWidget->row(currentItem);
        if (!PixmapLoad || NumberPixmap > row - FoldersNumber)
            emit imageClicked(ListPixmap[row - FoldersNumber].Image);
        else
        {
            QMessageBox::warning(this, tr("Error"), tr("image loading"));
            return;
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("files loading"));
    }
}


/**
 *  @brief Загрузка изображения непосредственно из vector<QPixmap> в формате JPG
*/
void MainWindow::downloadImage()
{
    if(!PixmapLoad)
    {
        int row = ui->listWidget->row(currentItem);
        QString fileName = QFileDialog::getSaveFileName(nullptr, "Donwload Image", QDir::homePath(), "JPEG (*.jpg)");
        if (!fileName.isEmpty()) {
            QImage image = ListPixmap[row - FoldersNumber].Image.toImage();
            if (image.isNull()) {
                QMessageBox::warning(this, tr("Error"), tr("image is empty"));
                return;
            }
            if (!image.save(fileName, "JPG")) {
                QMessageBox::warning(this, tr("Error"), tr("Failed to save image"));
                return;
            }
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("files loading"));
    }
}


/**
 *@brief Удаление фотографии на сервере
*/
void MainWindow::deleteImage()
{
    if(!PixmapLoad)
    {
        int row = ui->listWidget->row(currentItem);
        SendToServer(ModeDisk::DeleteImage, row);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("files loading"));
    }
}

/**
 * @brief Оброботка нажатия на объекты ui->listWidget
 * @param item Объект listWidget
*/
void MainWindow::handleImageClick(QListWidgetItem *item)
{
    if(!PixmapLoad)
    {
        int row = ui->listWidget->row(item);
        if(item->data(Qt::UserRole).toString() == "Image")
        {
            if (!PixmapLoad || NumberPixmap > row - FoldersNumber)
                emit imageClicked(ListPixmap[row - FoldersNumber].Image);
            else
            {
                QMessageBox::warning(this, tr("Error"), tr("image loading"));
                return;
            }
        }
        else if(item->data(Qt::UserRole).toString() == "Folder")
        {
            SendToServer(ModeDisk::OpenFolder, row);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("files loading"));
    }
}


/**
 * @brief Открытие папки на сервере
*/
void MainWindow::OpenFolder()
{
    if(!PixmapLoad)
    {
        int row = ui->listWidget->row(currentItem);
        SendToServer(ModeDisk::OpenFolder, row);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("files loading"));
    }
}

/**
 *  @brief Удаление папки на сервере
 */
void MainWindow::DeleteFolder()
{
    if(!PixmapLoad)
    {
        int row = ui->listWidget->row(currentItem);
        SendToServer(ModeDisk::DeleteFolder, row);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("files loading"));
    }
}


/**
 * @brief Загрузка изображения на сервер в соответствии с конфигурационным файлом и проверка на то, можно ли это делать
 */
void MainWindow::FileDialogLoadImage()
{
    if (permission == "allow")
    {
        if(!PixmapLoad)
        {
            QString filesType = "Image Files (";
            for (int i = 0; i < filters.size(); i++)
            {
                if (i != 0) {
                    filesType += " ";
                }
                filesType += filters[i];
            }
            filesType += ")";
            QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", filesType);
            QFileInfo fileInfo(fileName);

            emit ImageSignalLoad(fileName, fileInfo, 1);
        }
        else
        {
            QMessageBox::warning(this, tr("Error"), tr("files loading"));
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("the image cannot be uploaded"));
    }

}


/**
 *  @brief создание папки на сервере
 */
void MainWindow::createFolder()
{
    if(!PixmapLoad)
    {
        bool ok;
        QString NameFolder;
        do
        {
            NameFolder = QInputDialog::getText(this, tr("Input Dialog"),
                                               tr("Please enter name File:"), QLineEdit::Normal,
                                               "", &ok);
            if(ok && !NameFolder.isEmpty())
                break;

            else if (ok && NameFolder.isEmpty())
                QMessageBox::warning(this, tr("Input Error"), tr("Text cannot be empty!"));
            else if (!ok)
                break;
        }
        while(NameFolder.isEmpty() || !ok);
        if (ok)
        {
            bool SaveFolder = 1;
            for (int i = 0; i < ui->listWidget->count(); i++) {
                QListWidgetItem *item = ui->listWidget->item(i);
                if (item->text() == NameFolder) {
                    SaveFolder = 0;
                }
            }
            if(SaveFolder)
            {
                SendToServer(ModeDisk::createFolder, NameFolder);
            }
            else
            {
                QMessageBox::warning(this, tr("Error"), tr("same file name"));
            }
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("files loading"));
    }
}

/**
 *  @brief Возвращение из текущей папки в папку родитель
 */
void MainWindow::on_BackFolder_clicked()
{
    if(!PixmapLoad)
    {
        SendToServer(ModeDisk::BackFolder);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("files loading"));
    }
}

/**
 * @brief Возвращение в корневую папку
 */
void MainWindow::on_BackRoot_clicked()
{
    if(!PixmapLoad)
    {
        SendToServer(ModeDisk::BackRoot);
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("files loading"));
    }
}


/**
 * @brief Метод для принятия сообщений
*/
void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);
    if(in.status() == QDataStream::Ok)
    {
        int sizeItems, sizePixmap;
        ImageInfo pixmap;
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
            int mode;
            in >> mode;
            switch (mode) {
                // Сервер прислал иконки для отображения их в приложении
            case ModeDisk::SendItems:
            {
                for (int i = 0; i < static_cast<int>(ListItem.size()); i++)
                {
                    delete ListItem[i];
                }
                ListPixmap.clear();
                PixmapLoad = 1;
                ListItem.clear();
                ui->listWidget->clear();
                in >> FoldersNumber >> sizeItems;
                for (int i = 0; i < sizeItems; i++)
                {
                    QString strName, strType;
                    QIcon icon;
                    in >> strName >> icon >> strType;
                    QListWidgetItem *item = new QListWidgetItem(icon, strName);
                    item->setData(Qt::UserRole, strType);
                    ListItem.push_back(item);
                }
                SendToServer(ModeDisk::ReadyGetNewMess, ModeDisk::SendPixmap);
                break;
            }
                /*Сервер прислал список изображений формата QPixmap и происходит
                загрузка этих изображений в список vector<QPixmap> ListPixmap
                так же приходит информация о количестве изображений, так как они прислыаются \
                парционо по 5 штук, и пока все изображения не загрузятся, остальные действия
                блокируются с помощью флага  PixmapLoad*/
            case ModeDisk::SendPixmap:
            {
                if(!PixmapLoad)
                {
                    ListPixmap.clear();
                }
                PixmapLoad = 1;
                in >> sizePixmap;
                int stop = NumberPixmap;
                for (NumberPixmap; NumberPixmap < sizePixmap && NumberPixmap < stop + 5; NumberPixmap++)
                {
                    in >> pixmap.NameFile >> pixmap.Image;
                    ListPixmap.push_back(pixmap);
                }
                if (NumberPixmap == sizePixmap)
                {
                    NumberPixmap = 0;
                    PixmapLoad = 0;
                }
                else
                {
                    SendToServer(ModeDisk::ReadyGetNewMess, ModeDisk::SendPixmap);
                }
                break;
            }
                //загрузка информации из конфигурационного файла
            case ModeDisk::SendConfig:
            {
                in >> filters >> permission;
                ui->listWidget->filters = this->filters;
                //отправка на сервер сообщения о том, что клиент готов получать новое сообщение и информация о том, какое действие выполнялось
                SendToServer(ModeDisk::ReadyGetNewMess, ModeDisk::SendConfig);
                break;
            }
            default:
                break;
            }
            UpdateFolder();
            NextBlockSize = 0;
            break;
        }
    }
}

 /* перегруженные функции отправки сообщений. В данных кусках кода происходит
  загрузка в out информации о том, какое действие совершил клиент, и нужные данные в соответствии
  с выполненным действием. Так же стоит отметить, что в начало каждого сообщения устанавливается
  размер сообщения, чтобы оно было корректно считаться.*/
/**
 * @brief Отправка сообщения на сервер
 * @param mode Режим работы
 * @param numberItem номер айтема
 */
void MainWindow::SendToServer(int mode, int numberItem)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint64(0) << mode << numberItem;
    out.device()->seek(0);
    out << quint64(Data.size() - sizeof(quint64));
    socket->write(Data);
    socket->waitForBytesWritten();
}

/**
 * @brief Отправка сообщения на сервер
 * @param mode Режим работы
 * @param fileinfo информация о файле
 * @param pixmap карта изображения
 * @param size количество загружаемых изображений
 */
void MainWindow::SendToServer(int mode, QFileInfo fileinfo, QPixmap pixmap, int size)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    out << quint64(0) << mode << QImageReader::imageFormat(fileinfo.filePath()) << fileinfo.fileName();
    out << pixmap << size;

    out.device()->seek(0);
    out << quint64(Data.size() - sizeof(quint64));

    socket->write(Data);
    socket->waitForBytesWritten();
}


/**
 * @brief Отправка сообщения на сервер
 * @param mode режим работы
 * @param str имя файла
 */
void MainWindow::SendToServer(int mode, QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint64(0) << mode << str;
    out.device()->seek(0);
    out << quint64(Data.size() - sizeof(quint64));
    socket->write(Data);
    socket->waitForBytesWritten();
}


/**
 * @brief Отправка сообщения на сервер
 * @param mode режим работы
 */
void MainWindow::SendToServer(int mode)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint64(0) << mode;
    out.device()->seek(0);
    out << quint64(Data.size() - sizeof(quint64));
    socket->write(Data);
    socket->waitForBytesWritten();
}


