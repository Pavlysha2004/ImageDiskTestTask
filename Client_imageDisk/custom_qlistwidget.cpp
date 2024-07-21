#include "custom_qlistwidget.h"

Custom_QListWidget::Custom_QListWidget(QWidget *parent) : QListWidget(parent)
{
    setAcceptDrops(true);
    viewport()->installEventFilter(this);
}

void Custom_QListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void Custom_QListWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void Custom_QListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
    event->acceptProposedAction();
}

bool stringInList(QString string, QStringList list) {
    for (int i = 0 ; i < list.size(); i++) {
        if (string == list[i].mid(2)) {
            return true;
        }
    }
    return false;
}

void Custom_QListWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urlList = event->mimeData()->urls();
        for (int i = 0; i < urlList.size(); i++) {
            QString fileName = urlList[i].toLocalFile();
            QFileInfo fileInfo(fileName);
            if (stringInList(fileInfo.suffix().toLower(), filters)) {
                emit ImageLoaded(fileName, fileInfo, urlList.size());
            }
            else
            {
                QMessageBox::warning(this, tr("Error"), tr("unsuitable file extension"));
                return;
            }
        }
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
