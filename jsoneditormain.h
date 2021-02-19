#ifndef JSONEDITORMAIN_H
#define JSONEDITORMAIN_H

#include <QMainWindow>
#include <QModelIndex>
#include <QTextEdit>
#include "jsonc/jsoncxx.h"
#include "jsontreeitem.h"
#include "finddialog.h"
#include "findreplacedialog.h"
#include <QMessageBox>
#include <QTreeView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QLineEdit>
#include <QAction>
#include <QMenu>
#include <QList>

namespace Ui {
    class JsonEditorMain;
}

class JsonEditorMain : public QDialog {
    Q_OBJECT
public:
    JsonEditorMain(QWidget *parent = 0);
    ~JsonEditorMain();

    void useSignalOnly(bool v);// { m_useSignalOnly = v; }
    void setData(QString v);
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

private:
    Ui::JsonEditorMain *ui;
    Json::Value jsonValue;
    bool m_useSignalOnly;

    const QString newInsertText;
    const QString treeViewColumnKey;
    const QString treeViewColumnValue;
    const QString treeViewColumnType;

    QString treeFormat(JsonTreeItem* treeItem = NULL, QString indent = "", bool noHeader = false);
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QString curFile;
    QTextEdit *textEdit;
    QString lastFilePath;
    FindDialog *m_findDialog;
    FindReplaceDialog *m_findReplaceDialog;
    void adjustForCurrentFile(const QString &filePath);
    void updateRecentActionList();

signals:
    void valueChanged(QString newValue);

private slots:
    void toggleFindToolbar(bool checked);    
    void refreshJsonTree();
    void insertTreeNode();
    void deleteTreeNode();
    void insertTreeChild();
    void updateActions();
    void treeViewDataChanged();
    void formatCode();
    void dataEdit(QModelIndex editIndex);
    void open();
    bool save();
    bool saveAs();
    void documentWasModified();
    void newFile();
    void close();
    void undo();
    void redo();
    void paste();
    void cut();
    void copy();
    void selectAll();
    void openRecent();

public:
    QAction *menuNewFile;
    QAction *menuOpenFile;
    QAction *menuSaveFile;
    QAction *menuSaveAs;
//    QAction *menuRecent;
    QAction *menuExit;
    QAction *menuUndo;
    QAction *menuRedo;
    QAction *menuCut;
    QAction *menuCopy;
    QAction *menuPaste;
    QAction *menuSelectAll;
    QAction *menuRefresh;
    QAction *menuFormat;
    QAction *menuInsertNode;
    QAction *menuDeleteNode;
    QAction *menuInsertChild;
    QAction *menuFind;
    QAction *menuReplace;
    QToolBar *fileToolBar;
    QToolBar *EditToolBar;
    QToolBar *toolsToolBar;
    QToolBar *findToolBar;
    QTextEdit *jsonCode;
    QTreeView *jsonTree;
    QMainWindow *mw;
    QVBoxLayout *layout;
    QMenuBar *mb;
    QMenu *filemenu;
    QMenu *editmenu;
    QMenu *toolsmenu;
    QMenu *findmenu;
    QHBoxLayout *layout1;
    QMenu* recentMenu;
    QMenu* recentFilesMenu;
    QAction* openAction;
    const int maxFileNr;
    QString currentFilePath;
    QList<QAction *> recentFileActionList;
};

#endif // JSONEDITORMAIN_H
