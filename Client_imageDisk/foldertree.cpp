#include "foldertree.h"

FolderTree::FolderTree(const string& name)
{
    NameNode = name;
    this->Parent = nullptr;
    this->root = this;
}

FolderTree::~FolderTree()
{
}

bool FolderTree::AddNewNode(string name)
{
    for (int i = 0; i < static_cast<int>(children.size()); i++)
    {
        if (name == children[i]->NameNode)
            return 0;
    }
    FolderTree *NewNode = new FolderTree(name);
    NewNode->Parent = this;
    NewNode->root = this->root;
    children.push_back(NewNode);
    return 1;
}

void FolderTree::DelAllTree()
{
    deleteTree(this->root);
}

void FolderTree::deleteTree(FolderTree* TRoot) {
    if (TRoot == nullptr) return;

    for (FolderTree* child : TRoot->children) {
        deleteTree(child);
    }

    delete TRoot;
}

void FolderTree::SaveAllTreeFolder()
{
    SaveTree(this->root);
}

void FolderTree::SaveTree(FolderTree *TRoot)
{
    if (TRoot == nullptr) return;
    bool ImagesDownload = 0,
        ThereAreFolder = 0;
    for (FolderTree* child : TRoot->children)
    {
        ThereAreFolder = 1;
        SaveInFolder(child->NameNode, Images, ImagesDownload, ThereAreFolder);
        ImagesDownload = 1;
        GoInChildrenSave(child->NameNode);
        SaveTree(child);
    }

    SaveInFolder(child->NameNode, Images, ImagesDownload, ThereAreFolder);

    GoInParent();


}

void FolderTree::DelNode(int NumberPos)
{
    deleteTree(this->children[NumberPos]);
    this->children.erase(children.begin() + NumberPos);
}

bool FolderTree::AddNewImage(string name, QPixmap pixmap)
{
    for (int i = 0; i < static_cast<int>(Images.size()); i++)
    {
        if (name == Images[i].name)
            return 0;
    }
    ImagesInf TImages(name, pixmap);
    Images.push_back(TImages);
    return 1;
}

void FolderTree::DelImage(int NumberPos)
{
    NumberPos -= static_cast<int>(children.size());
    Images.erase(Images.begin() + NumberPos);

}

bool FolderTree::CheckNodeForRoot()
{
    if (this == this->root)
        return 0;
    else
        return 1;
}

QPixmap FolderTree::GetImage(int NumberPos)
{
    NumberPos -= static_cast<int>(this->children.size());
    return Images[NumberPos].ImagePixmap;
}

FolderTree* FolderTree::GoNode(int NumberPos)
{
    return this->children[NumberPos];
}

FolderTree* FolderTree::BackNode()
{
    return this->Parent;
}

FolderTree *FolderTree::GoRoot()
{
    return this->root;
}

vector<QListWidgetItem *> FolderTree::ExtractFiles()
{
    vector<QListWidgetItem *> Items;
    QPixmap TPixmap("/home/pavlyha/image/papka.png");
    for (int i = 0; i < static_cast<int>(children.size()); i++)
    {
        QString nameIcon = QString::fromStdString(children[i]->NameNode);
        QListWidgetItem *item = new QListWidgetItem(QIcon(TPixmap.scaled(100, 100, Qt::KeepAspectRatio)), nameIcon);
        item->setData(Qt::UserRole, "Folder");
        Items.push_back(item);
    }
    for (int i = 0; i < static_cast<int>(Images.size()); i++)
    {
        QString nameIcon = QString::fromStdString(Images[i].name);
        QListWidgetItem *item = new QListWidgetItem(QIcon(Images[i].ImagePixmap.scaled(100, 100, Qt::KeepAspectRatio)), nameIcon);
        item->setData(Qt::UserRole, "Image");
        Items.push_back(item);
    }
    return Items;
}


