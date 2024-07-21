#ifndef FOLDERTREE_H
#define FOLDERTREE_H

#include <vector>
#include <string>
#include <QPixmap>
#include <QListWidgetItem>
#include <QIcon>
#include <QString>
#include "ImagesInf.h"
using namespace std;

class FolderTree
{
public:
    FolderTree(const string& name);
    ~FolderTree();

    bool AddNewNode(string name);
    void DelNode(int NumberPos);
    bool AddNewImage(string, QPixmap);
    void DelImage(int NumberPos);
    void DelAllTree();
    bool CheckNodeForRoot();
    void SaveAllTreeFolder();
    QPixmap GetImage(int NumberPos);
    FolderTree* GoNode(int NumberPos);
    FolderTree* BackNode();
    FolderTree *GoRoot();
    vector <QListWidgetItem* > ExtractFiles();


private:
    string NameNode;
    vector <FolderTree*> children;
    FolderTree* Parent;
    vector <ImagesInf> Images;
    FolderTree *root;

    void deleteTree(FolderTree*);
    void SaveTree(FolderTree*);
signals:
    void SaveInFolder(string, vector <ImagesInf>, bool ImagesDownload, bool ThereAreFolders);
    void GoInChildrenSave(string);
    void GoInParent();
};

#endif // FOLDERTREE_H
