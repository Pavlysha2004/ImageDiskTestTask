#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QPixmap>
#include <QString>

struct ImageInfo
{
    ImageInfo (QString name, QPixmap pixmap)
    {
        NameFile = name;
        Image = pixmap;
    }
    ImageInfo() = default;
    QString NameFile;
    QPixmap Image;
};

#endif // IMAGEINFO_H
