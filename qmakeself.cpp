#include "qmakeself.h"
#include "ui_qmakeself.h"

QMakeSelf::QMakeSelf(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QMakeSelf)
{
    ui->setupUi(this);
    buildGUI();
}

QMakeSelf::~QMakeSelf()
{
    delete ui;
}

void QMakeSelf::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void QMakeSelf::buildGUI()
{
    this->setWindowIcon(QIcon(":pic/main.png"));
    buildToolBar();
    configureTreeView();
    detectAlgorithms();
    analyzeAlgorithmFeatures(ui->cbCompressionType->currentText());
    fillUnpackTypes();
    analyzeUnpackFeatures(ui->cbUnpackType->currentText());
    connect(ui->cbCompressionType, SIGNAL(currentIndexChanged(QString)), this, SLOT(analyzeAlgorithmFeatures(QString)));
    connect(ui->cbUnpackType, SIGNAL(currentIndexChanged(QString)), this, SLOT(analyzeUnpackFeatures(QString)));
}

void QMakeSelf::buildToolBar()
{
    //TODO Set correct names for actions
    QAction* action = NULL;
    ui->tbToolBar->setFloatable(false);
    ui->tbToolBar->setMovable(false);
    ui->tbToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->tbToolBar->setIconSize(QSize(48, 48));
    ui->tbToolBar->setOrientation(Qt::Horizontal);
    action = ui->tbToolBar->addAction(QIcon(":pic/new.png"),tr("Новый"), this, SLOT(newArchive()));
    action->setIconText(tr("Новый"));
    action = ui->tbToolBar->addAction(QIcon(":pic/open.png"), tr("Открыть"), this, SLOT(openArchive()));
    action->setIconText(tr("Открыть"));
    action = ui->tbToolBar->addAction(QIcon(":pic/save.png"), tr("Сохранить"), this, SLOT(saveArchive()));
    action->setIconText(tr("Сохранить"));
    ui->tbToolBar->addSeparator();
    action = ui->tbToolBar->addAction(QIcon(":pic/export.png"),tr("Выполнить"), this, SLOT(createArchive()));
    action->setIconText(tr("Выполнить"));
    ui->tbToolBar->addSeparator();
    action = ui->tbToolBar->addAction(QIcon(":pic/sets_save.png"), "", this, SLOT(saveSettings()));
    //action->setIconText(tr("Сохранить параметры"));
    action = ui->tbToolBar->addAction(QIcon(":pic/sets_reset.png"), "", this, SLOT(loadDefaultSettings()));
    ui->tbToolBar->addSeparator();
    action = ui->tbToolBar->addAction(QIcon(":pic/help.png"), tr("Справка"), this, SLOT(showHelp()));
}

void QMakeSelf::newArchive()
{
    qDebug()<< "newArchive()";
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    if(dialog.exec())
    {
        sourcePath = dialog.selectedFiles().at(0);
        qDebug() << sourcePath;
        QFileSystemModel* fsModel = new QFileSystemModel(ui->tvArchiveRoot);
        fsModel->setRootPath(sourcePath);
        fsModel->setReadOnly(false);
        fsModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
        ui->tvArchiveRoot->setModel(fsModel);
        ui->tvArchiveRoot->setRootIndex(fsModel->index(sourcePath));
    }
    else
    {

    }
}

void QMakeSelf::openArchive()
{
    qDebug()<< "openArchive()";
}

void QMakeSelf::saveArchive()
{
    qDebug()<< "saveArchive()";
}

void QMakeSelf::createArchive()
{
    qDebug()<< "createArchive()";
}

void QMakeSelf::saveSettings()
{
    qDebug()<< "saveSettings()";
}

void QMakeSelf::loadDefaultSettings()
{
    qDebug()<< "loadDefaultSettings()";
}

void QMakeSelf::configureTreeView()
{
    ui->tvArchiveRoot->setDragEnabled(true);
    ui->tvArchiveRoot->viewport()->setAcceptDrops(true);
    ui->tvArchiveRoot->setDropIndicatorShown(true);
    ui->tvArchiveRoot->setDragDropMode(QAbstractItemView::DragDrop);
}

void QMakeSelf::showHelp()
{
    qDebug()<< "showHelp()";
}

void QMakeSelf::detectAlgorithms()
{
    int result;
    QStringList allAlgorithms;
    allAlgorithms << "gzip" << "bzip2" << "pbzip2" << "xz" << "compress" << "base64";
    QStringList availableAlgorithms;
    QString program("/usr/bin/which");
    QString algrorithm;
    foreach (algrorithm, allAlgorithms)
    {
        QString out;
        QStringList args;
        args << algrorithm;
        QProcess *proc = new QProcess(this);
        proc->start(program, args);
        if(proc->waitForStarted());
        if(proc->waitForReadyRead()) out = proc->readAllStandardOutput();
        if(proc->waitForFinished());
        if(out.length()>0) availableAlgorithms.append(algrorithm);
        delete proc;
    }
    ui->cbCompressionType->addItems(availableAlgorithms);
    ui->cbCompressionType->addItem(tr("Не сжимать"), 0);
}

void QMakeSelf::fillUnpackTypes()
{
    QStringList unpackTypes;
    unpackTypes << tr("В подкаталог") << tr("В рабочий каталог") << tr("В целевой каталог") << tr("Добавить к существующему");
    unpackTypes.sort();
    ui->cbUnpackType->addItems(unpackTypes);
}

void QMakeSelf::analyzeAlgorithmFeatures(QString algorithm)
{
    bool enable = !(algorithm == tr("Не сжимать") || algorithm == "base64" || algorithm == "compress");
    ui->sbCompressionRatio->setEnabled(enable);
}

void QMakeSelf::analyzeUnpackFeatures(QString type)
{
    ui->leDestination->setEnabled(!(type == tr("В подкаталог") || type == tr("В рабочий каталог")));
    ui->leLabel->setEnabled(type != tr("Добавить к существующему"));
}

void QMakeSelf::dropEvent(QDropEvent *de)
{
    const QMimeData* mime = de->mimeData();
    qDebug() << mime->formats();
    de->accept();
}
