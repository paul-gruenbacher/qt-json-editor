#include "jsoneditormain.h"
#include "ui_jsoneditormain.h"
#include "jsontreemodel.h"
#include "addnodedlg.h"
#include "jsedit.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
//#include <QtGui>

JsonEditorMain::JsonEditorMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JsonEditorMain),
    m_useSignalOnly(false),
    newInsertText(tr("Insert new data")),
    treeViewColumnKey(tr("Node")),
    treeViewColumnValue(tr("Value")),
    treeViewColumnType(tr("Type")),
    maxFileNr(4)
{
    ui->setupUi(this);
    mw = new QMainWindow();
    layout = new QVBoxLayout(this);
    mb = new QMenuBar();
    filemenu = new QMenu("&File");
    editmenu = new QMenu("&Edit");
    toolsmenu = new QMenu("&Tools");
    findmenu = new QMenu("&Find");
    jsonCode = new QTextEdit();
    jsonTree = new QTreeView();
    mb->addMenu(filemenu);
    mb->addMenu(editmenu);
    mb->addMenu(toolsmenu);
    mb->addMenu(findmenu);
    menuNewFile = new QAction(QIcon(":/ToolbarIcon/images/document-new.png"),tr("New"),filemenu);
    filemenu->addAction(menuNewFile);
    menuOpenFile = new QAction(QIcon(":/ToolbarIcon/images/document-open.png"),tr("Open"),filemenu);
    filemenu->addAction(menuOpenFile);
    menuSaveFile = new QAction(QIcon(":/ToolbarIcon/images/media-floppy.png"),tr("Save"),filemenu);
    filemenu->addAction(menuSaveFile);
    menuSaveAs = new QAction(QIcon(":/ToolbarIcon/images/document-save.png"),tr("SaveAs"),filemenu);
    filemenu->addAction(menuSaveAs);
    QAction* recentFileAction = 0;
    for(int i = 0; i < 5; ++i)
    {
        recentFileAction = new QAction(this);
        recentFileAction->setVisible(false);
        connect(recentFileAction, SIGNAL(triggered()),this, SLOT(openRecent()));
        recentFileActionList.append(recentFileAction);
    }
    recentMenu = filemenu->addMenu(tr("&Open Recent"));
    for(int i = 0; i < maxFileNr; ++i)
        recentMenu->addAction(recentFileActionList.at(i));
    menuExit = new QAction(QIcon(":/ToolbarIcon/images/system-log-out.png"),tr("Exit"),filemenu);
    filemenu->addAction(menuExit);
    menuUndo = new QAction(QIcon(":/ToolbarIcon/images/edit-undo.png"),tr("Undo"),editmenu);
    editmenu->addAction(menuUndo);
    menuRedo = new QAction(QIcon(":/ToolbarIcon/images/edit-redo.png"),tr("Redo"),editmenu);
    editmenu->addAction(menuRedo);
    editmenu->addSeparator();
    menuCut = new QAction(QIcon(":/ToolbarIcon/images/edit-cut.png"),tr("Cut"),editmenu);
    editmenu->addAction(menuCut);
    menuCopy = new QAction(QIcon(":/ToolbarIcon/images/edit-copy.png"),tr("Copy"),editmenu);
    editmenu->addAction(menuCopy);
    menuPaste = new QAction(QIcon(":/ToolbarIcon/images/edit-paste.png"),tr("Paste"),editmenu);
    editmenu->addAction(menuPaste);
    menuSelectAll = new QAction(tr("Select All"),editmenu);
    editmenu->addAction(menuSelectAll);
    menuRefresh = new QAction(QIcon(":/ToolbarIcon/images/view-refresh.png"),tr("Refresh"),toolsmenu);
    toolsmenu->addAction(menuRefresh);
    menuFormat = new QAction(QIcon(":/ToolbarIcon/images/format-justify-right.png"),tr("Format"),toolsmenu);
    toolsmenu->addAction(menuFormat);
    menuInsertNode = new QAction(QIcon(":/ToolbarIcon/images/list-add.png"),tr("Insert Node"),toolsmenu);
    toolsmenu->addAction(menuInsertNode);
    menuDeleteNode = new QAction(QIcon(":/ToolbarIcon/images/list-remove.png"),tr("Delete Node"),toolsmenu);
    toolsmenu->addAction(menuDeleteNode);
    menuInsertChild = new QAction(QIcon(":/ToolbarIcon/images/list-addchild.png"),tr("Insert Child"),toolsmenu);
    toolsmenu->addAction(menuInsertChild);
    menuFind = new QAction(QIcon(":/ToolbarIcon/images/edit-find.png"),tr("Find"),findmenu);
    findmenu->addAction(menuFind);
    menuReplace = new QAction(QIcon(":/ToolbarIcon/images/edit-find-replace.png"),tr("Replace"),findmenu);
    findmenu->addAction(menuReplace);
    layout->setMenuBar(mb);
    layout->addWidget(mw);
    fileToolBar = new QToolBar();
    EditToolBar = new QToolBar();
    toolsToolBar = new QToolBar();
    findToolBar = new QToolBar();
    fileToolBar->addAction(menuNewFile);
    fileToolBar->addAction(menuOpenFile);
    fileToolBar->addAction(menuSaveFile);
    fileToolBar->addAction(menuExit);
    EditToolBar->addAction(menuCut);
    EditToolBar->addAction(menuUndo);
    EditToolBar->addAction(menuRedo);
    EditToolBar->addAction(menuCut);
    EditToolBar->addAction(menuCopy);
    EditToolBar->addAction(menuPaste);
    toolsToolBar->addAction(menuRefresh);
    toolsToolBar->addAction(menuFormat);
    toolsToolBar->addAction(menuInsertNode);
    toolsToolBar->addAction(menuDeleteNode);
    toolsToolBar->addAction(menuInsertChild);
    findToolBar->addAction(menuFind);
    findToolBar->addAction(menuReplace);
    mw->addToolBar(fileToolBar);
    mw->addToolBar(EditToolBar);
    mw->addToolBar(toolsToolBar);
    mw->addToolBar(findToolBar);
    layout1 = new QHBoxLayout();
    layout->addLayout(layout1,1);
    layout1->addWidget(jsonCode);
    layout1->addWidget(jsonTree);
    connect(menuFind, SIGNAL(toggled(bool)), this, SLOT(toggleFindToolbar(bool))); 
    connect(menuRefresh, SIGNAL(triggered()), this, SLOT(refreshJsonTree()));
    connect(menuInsertNode, SIGNAL(triggered()), this, SLOT(insertTreeNode()));
    connect(menuInsertChild, SIGNAL(triggered()), this, SLOT(insertTreeChild()));
    connect(menuDeleteNode, SIGNAL(triggered()), this, SLOT(deleteTreeNode()));
    connect(jsonTree, SIGNAL(clicked(QModelIndex)), this, SLOT(updateActions()));
    connect(menuFormat, SIGNAL(triggered()), this, SLOT(formatCode()));
    connect(jsonTree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(dataEdit(QModelIndex)));

    connect(menuOpenFile, SIGNAL(triggered()), this, SLOT(open()));
    connect(menuSaveFile, SIGNAL(triggered()), this, SLOT(save()));
    connect(menuSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(menuExit, SIGNAL(triggered()), this, SLOT(close()));


    JSHighlighter * highlight = new JSHighlighter(jsonCode->document());

    textEdit = jsonCode;
    connect(textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

    m_findDialog = new FindDialog(this);
    m_findDialog->setModal(false);
    m_findDialog->setTextEdit(textEdit);

    m_findReplaceDialog = new FindReplaceDialog(this);
    m_findReplaceDialog->setModal(false);
    m_findReplaceDialog->setTextEdit(textEdit);

    connect(menuFind, SIGNAL(triggered()), m_findDialog, SLOT(show()));
    connect(menuReplace, SIGNAL(triggered()), m_findReplaceDialog, SLOT(show()));
    connect(menuNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(menuUndo, SIGNAL(triggered()), this, SLOT(undo()));
    connect(menuRedo, SIGNAL(triggered()), this, SLOT(redo()));
    connect(menuCut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(menuCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(menuPaste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(menuSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));
    readSettings();
    updateRecentActionList();
}

JsonEditorMain::~JsonEditorMain()
{
    delete ui;
}

void JsonEditorMain::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void JsonEditorMain::toggleFindToolbar(bool checked)
{
    checked ? findToolBar->show() : findToolBar->hide();
}

void JsonEditorMain::refreshJsonTree()
{
    if (!jsonCode->document()->isEmpty())
    {
        QByteArray ss = jsonCode->toPlainText().toLocal8Bit();
        std::string json = jsonCode->toPlainText().toStdString();
        Json::Reader jsonReader;
        jsonValue.clear();
        jsonReader.parse(ss.data(), jsonValue);

        QStringList headers;
        headers << treeViewColumnKey << treeViewColumnValue << treeViewColumnType;



        QAbstractItemModel *oldModel = jsonTree->model();
        if (oldModel != NULL)
            oldModel->disconnect(SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        JsonTreeModel *model = new JsonTreeModel(headers, jsonValue);
        QItemSelectionModel *selectionModel = jsonTree->selectionModel();
        connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(treeViewDataChanged()));
        jsonTree->setModel(model);
        jsonTree->reset();
        delete selectionModel;
        delete oldModel;


        jsonTree->expandAll();

        for (int i = 0; i < jsonTree->model()->columnCount(); i++)
            jsonTree->resizeColumnToContents(i);
    }
}

void JsonEditorMain::insertTreeNode()
{
    AddNodeDlg dlg;
    if (dlg.exec() == QDialog::Accepted)
    {
        if(jsonTree->selectionModel() == 0)
        {
            QMessageBox::warning(this,this->windowTitle(),"Adding a node to an empty tree is not supported.");
            return;
        }

        QModelIndex index = jsonTree->selectionModel()->currentIndex();
        QAbstractItemModel *model = jsonTree->model();

        if (!model->insertRow(index.row()+1, index.parent()))
            return;

        updateActions();

        QModelIndex child = model->index(index.row()+1, 0, index.parent());
        model->setData(child, QVariant(dlg.getName()), Qt::EditRole);

        child = model->index(index.row()+1, 1, index.parent());
        model->setData(child, QVariant(dlg.getValue()), Qt::EditRole);

        child = model->index(index.row()+1, 2, index.parent());
        model->setData(child, QVariant(dlg.getType()), Qt::EditRole);
    }
}

void JsonEditorMain::insertTreeChild()
{
    AddNodeDlg dlg;
    if (dlg.exec() == QDialog::Accepted)
    {
        QModelIndex index = jsonTree->selectionModel()->currentIndex();
        JsonTreeModel *model = (JsonTreeModel*)jsonTree->model();

        if (index.column() != 0)
        {
            index = index.sibling(index.row(), 0);
        }

        if (model->columnCount(index) == 0)
        {
            if (!model->insertColumn(0, index))
                return;
        }

        if (!model->insertRow(0, index))
            return;

        QModelIndex child = model->index(0, 0, index);
        model->setData(child, QVariant(dlg.getName()), Qt::EditRole);

        child = model->index(0, 1, index);
        model->setData(child, QVariant(dlg.getValue()), Qt::EditRole);

        child = model->index(0, 2, index);
        model->setData(child, QVariant(dlg.getType()), Qt::EditRole);

        jsonTree->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                                        QItemSelectionModel::ClearAndSelect);
        updateActions();
    }
}

void JsonEditorMain::deleteTreeNode()
{
    QModelIndex index = jsonTree->selectionModel()->currentIndex();
    QAbstractItemModel *model = jsonTree->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void JsonEditorMain::updateActions()
 {
     bool hasSelection = !jsonTree->selectionModel()->selection().isEmpty();
     menuDeleteNode->setEnabled(hasSelection);

     bool hasCurrent = jsonTree->selectionModel()->currentIndex().isValid();
     menuInsertNode->setEnabled(hasCurrent);
     menuInsertChild->setEnabled(hasCurrent);
 }

void JsonEditorMain::treeViewDataChanged()
{
    for (int i = 0; i < jsonTree->model()->columnCount(); i++)
    {
        jsonTree->resizeColumnToContents(i);
        jsonTree->setColumnWidth(i, jsonTree->columnWidth(i) + 20);
    }
}

void JsonEditorMain::formatCode()
{
    if (jsonTree->model() != NULL)
    {
        jsonCode->clear();

        QString codeText = "";
        JsonTreeModel *model;
        model = (JsonTreeModel*)jsonTree->model();
        codeText += treeFormat(model->getRootItem(), "", true);

        jsonCode->setPlainText(codeText);
    }
}

QString JsonEditorMain::treeFormat(JsonTreeItem *treeItem, QString indent, bool noHeader)
{
    QString resultStr;
    QString objectKey = treeItem->data(0).toString();
    QString objectValue = treeItem->data(1).toString();
    QString objectType = treeItem->data(2).toString();

    if (noHeader)
    {
        resultStr = indent;
    }
    else
    {
        resultStr = indent + QString("\"") + objectKey + "\": ";
    }

    if (objectType.compare(tr("object"), Qt::CaseInsensitive) == 0
        || objectType.compare(treeViewColumnType, Qt::CaseInsensitive) == 0)
    {
        resultStr += "{\n";
        JsonTreeItem *subObjectItem;
        for (int i = 0; i < treeItem->childCount(); i++)
        {
            subObjectItem = treeItem->child(i);
            resultStr += treeFormat(subObjectItem, indent + "    ");
        }
        if(treeItem->childCount() > 0)
            resultStr.remove(resultStr.length() - 2, 1);// remove trailing comma
        resultStr += indent + "},\n";
    }
    else if (objectType.compare(tr("array"), Qt::CaseInsensitive) == 0)
    {
        resultStr += "[\n";
        JsonTreeItem *subObjectItem;
        for (int i = 0; i < treeItem->childCount(); i++)
        {
            subObjectItem = treeItem->child(i);
            resultStr += treeFormat(subObjectItem, indent + "    ", true);
        }
        if(treeItem->childCount() > 0)
            resultStr.remove(resultStr.length() - 2, 1);// remove trailing comma
        resultStr += indent + "],\n";
    }
    else
    {
        if (objectType.compare(tr("string"), Qt::CaseInsensitive) == 0 ||
            objectType.compare(newInsertText, Qt::CaseInsensitive) == 0)
        {
            resultStr += "\"" + objectValue + "\",\n";
        }
        else
        {
            resultStr += objectValue + ",\n";
        }
    }

    return resultStr;
}

void JsonEditorMain::dataEdit(QModelIndex editIndex)
{
    QString vType = editIndex.sibling(editIndex.row(), 2).data(Qt::EditRole).toString();

    if (vType.compare(tr("object")) != 0
        && vType.compare(tr("array")) != 0)
    {
        if (editIndex.column() >= 2)
        {
            QModelIndex newIndex = editIndex.sibling(editIndex.row(), 1);
            jsonTree->edit(newIndex);
        }
    }
    else
    {
        QModelIndex newIndex = editIndex.sibling(editIndex.row(), 0);
        jsonTree->edit(newIndex);
    }
}

void JsonEditorMain::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void JsonEditorMain::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void JsonEditorMain::close()
{
    QDialog::close();
}

void JsonEditorMain::undo()
{
    textEdit->undo();
}

void JsonEditorMain::redo()
{
    textEdit->redo();
}

void JsonEditorMain::cut()
{
   textEdit->cut();
}

void JsonEditorMain::copy()
{
    textEdit->copy();
}

void JsonEditorMain::paste()
{
    textEdit->paste();
}

void JsonEditorMain::selectAll()
{
    textEdit->selectAll();
}

void JsonEditorMain::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,"Open Json File",lastFilePath);
        if (!fileName.isEmpty())
        {
            QFileInfo info(fileName);
            lastFilePath = info.absoluteDir().absolutePath();
            loadFile(fileName);
        }
    }

    menuRefresh->trigger();
}

void JsonEditorMain::openRecent(){
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}

void JsonEditorMain::newFile()
{

    textEdit->clear();
    jsonTree->setModel(NULL);
    setCurrentFile("");
}

void JsonEditorMain::useSignalOnly(bool v)
{
    (void)v;
}

void JsonEditorMain::setData(QString v) {
    textEdit->setPlainText(v);
    menuRefresh->trigger();
}

bool JsonEditorMain::save()
{
    if (m_useSignalOnly) {
        jsonCode->document()->setModified(false);
        setWindowModified(false);
        emit valueChanged(textEdit->toPlainText());
        return true;
    }

    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool JsonEditorMain::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (files.isEmpty())
        return false;

    return saveFile(files.at(0));
}

void JsonEditorMain::readSettings()
{
    QSettings settings;
    this->restoreGeometry(settings.value("geometry").toByteArray());
    lastFilePath = (settings.value("file_path").toString());
    curFile = settings.value("last_opened_file").toString();
    if(this->curFile.length() != 0)
    {
        loadFile(curFile);
        menuRefresh->trigger();
    }
}

void JsonEditorMain::writeSettings()
{
    QSettings settings;
    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("file_path", lastFilePath);
    settings.setValue("last_opened_file", this->curFile);
}

bool JsonEditorMain::maybeSave()
{
    if (jsonCode->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, this->windowTitle(),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void JsonEditorMain::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, this->windowTitle(),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }
adjustForCurrentFile(fileName);
    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
}

bool JsonEditorMain::saveFile(const QString &fileName)
{
    menuFormat->trigger();

    QFile file(fileName);
    QFileInfo info(fileName);
    lastFilePath = info.absoluteDir().absolutePath();
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, this->windowTitle(),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    return true;
}

void JsonEditorMain::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    jsonCode->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.json";
    setWindowFilePath(shownName);
}

QString JsonEditorMain::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void JsonEditorMain::adjustForCurrentFile(const QString &filePath){
    currentFilePath = filePath;
    setWindowFilePath(currentFilePath);
    QSettings settings;
    QStringList recentFilePaths =
            settings.value("recentFiles").toStringList();
    recentFilePaths.removeAll(filePath);
    recentFilePaths.prepend(filePath);
    while (recentFilePaths.size() > 5)
        recentFilePaths.removeLast();
    settings.setValue("recentFiles", recentFilePaths);

    // see note
    updateRecentActionList();
}

void JsonEditorMain::updateRecentActionList(){
    QSettings settings;
    QStringList recentFilePaths =
            settings.value("recentFiles").toStringList();

    size_t itEnd = 0u;
    if(recentFilePaths.size() <= maxFileNr)
        itEnd = recentFilePaths.size();
    else
        itEnd = maxFileNr;

    for (size_t i = 0u; i < itEnd; ++i) {
        QString strippedName = QFileInfo(recentFilePaths.at(i)).fileName();
        recentFileActionList.at(i)->setText(strippedName);
        recentFileActionList.at(i)->setData(recentFilePaths.at(i));
        recentFileActionList.at(i)->setVisible(true);
    }

    for (size_t i = itEnd; i < (size_t) maxFileNr; ++i)
        recentFileActionList.at(i)->setVisible(false);
}

