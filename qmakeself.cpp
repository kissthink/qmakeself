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
        adjustTreeView();
    }
    else
    {

    }
}

void QMakeSelf::openArchive()
{
    qDebug()<< "openArchive()";
    QString pathToOpen = QFileDialog::getOpenFileName(this, tr("Открыть архив"), QString(), QString("*.qms"), new QString("*.qms"));
    QFile in(pathToOpen);
    if(in.open(QFile::ReadOnly))
    {
        QDataStream inStream(&in);
        inStream.setVersion(QDataStream::Qt_5_3);
        int fileID = -1;
        inStream >> fileID;
        if(fileID == magicID)
        {
            inStream >> sourcePath >>  compressionType >> compressionLevel >> tarExtra >> nocrc >> nomd5 >> notemp >>label;
            inStream >> unpackType >> destination >> followLinks >> noprogress >> nox11 >> nowait >> lsmFile;
            inStream >> licenseFile >> headerFile >> initScript;
            setParams();
        }
        else
        {
            qDebug() << "wrong file type";
        }
        in.close();
    }
    else
    {
        qDebug() << "error";
    }
}

void QMakeSelf::saveArchive()
{
    qDebug()<< "saveArchive()";
    if(!sourcePath.isEmpty())
    {
        getParams();
        QString pathToSave = QFileDialog::getSaveFileName(this, tr("Сохранить архив"), QString(), QString("*.qms"), new QString("*.qms"));
        if (!pathToSave.contains(".qms")) pathToSave += ".qms";
        QFile out(pathToSave);
        if(out.open(QFile::WriteOnly))
        {
            QDataStream outStream(&out);
            outStream.setVersion(QDataStream::Qt_5_3);
            outStream << magicID << sourcePath <<  compressionType << compressionLevel << tarExtra << nocrc << nomd5 << notemp << label;
            outStream << unpackType << destination << followLinks << noprogress << nox11 << nowait << lsmFile;
            outStream << licenseFile << headerFile << initScript;
            out.close();
        }
        else
        {
            qDebug() << "error";
        }
    }
}

void QMakeSelf::createArchive()
{
    getParams();
    qDebug()<< "createArchive()";
    QProcess *proc = new QProcess(this);
    QString program = "/usr/bin/which";
    QString out;
    QStringList args;
    args << "makeself";
    proc->start(program, args);
    if(proc->waitForStarted());
    if(proc->waitForReadyRead()) out = proc->readAllStandardOutput().trimmed();
    if(proc->waitForFinished());
    if(out.length() > 0)
    {
        args.clear();
        program = out;
        args << "--" + compressionType << "--complevel" << QString::number(compressionLevel);
        if(!notemp) args << "--copy";
        if(nocrc) args << "--nocrc";
        if(nomd5) args << "--nomd5";
        if(nox11) args << "--nox11";
        if(noprogress) args << "--noprogress";
        if(nowait) args << "--nowait";
        if(followLinks) args << "--follow";
        if(tarExtra.length()>0) args << "--tar-extra " << tarExtra;
        if(lsmFile.length()>0) args << "--lsm" << lsmFile;
        if(licenseFile.length() > 0) args << "--license" << licenseFile;

        proc->start(program, args);
        if(proc->waitForStarted());
        if(proc->waitForReadyRead()) out = proc->readAll();
        if(proc->waitForFinished());
        qDebug() << args;
    }
    else
    {
        qDebug() << "makeself not installed";
    }
    delete proc;
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
        if(proc->waitForReadyRead()) out = proc->readAllStandardOutput().trimmed();
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

void QMakeSelf::getParams()
{
    compressionType = ui->cbCompressionType->currentText();
    compressionLevel = ui->sbCompressionRatio->value();
    tarExtra = ui->leTarAdditional->text().trimmed();
    nocrc = !ui->cbCRC->isChecked();
    nomd5 = !ui->cbMD5->isChecked();
    notemp = !ui->cbCopyToTemporary->isChecked();
    label = ui->leLabel->text().trimmed();
    unpackType = ui->cbUnpackType->currentText();
    destination = ui->leDestination->text().trimmed();
    followLinks = ui->cbFollowSymLinks->isChecked();
    noprogress = ui->cbUnpackProgress->isChecked();
    nox11 = ui->cbXtermAuto->isChecked();
    nowait = ui->cbXtermClose->isChecked();
    lsmFile = ui->leDestinationLSM->text().trimmed();
    licenseFile = ui->leDestinationLicense->text().trimmed();
    headerFile = ui->leDestinationHeader->text().trimmed();
    initScript = ui->leDestinationInitScript->text().trimmed();
}

void QMakeSelf::setParams()
{
    ui->cbCompressionType->setCurrentText(compressionType);
    ui->sbCompressionRatio->setValue(compressionLevel);
    ui->leTarAdditional->setText(tarExtra);
    ui->cbCRC->setChecked(!nocrc);
    ui->cbMD5->setChecked(!nomd5);
    ui->cbCopyToTemporary->setChecked(!notemp);
    ui->leLabel->setText(label);
    ui->cbUnpackType->setCurrentText(unpackType);
    ui->leDestination->setText(destination);
    ui->cbFollowSymLinks->setChecked(followLinks);
    ui->cbUnpackProgress->setChecked(noprogress);
    ui->cbXtermAuto->setChecked(nox11);
    ui->cbXtermClose->setChecked(!nowait);
    ui->leDestinationLSM->setText(lsmFile);
    ui->leDestinationLicense->setText(licenseFile);
    ui->leDestinationHeader->setText(headerFile);
    ui->leDestinationInitScript->setText(initScript);
    adjustTreeView();
}

void QMakeSelf::adjustTreeView()
{
    QFileSystemModel* fsModel = new QFileSystemModel(ui->tvArchiveRoot);
    fsModel->setRootPath(sourcePath);
    fsModel->setReadOnly(false);
    fsModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    ui->tvArchiveRoot->setModel(fsModel);
    ui->tvArchiveRoot->setRootIndex(fsModel->index(sourcePath));
}

void QMakeSelf::dropEvent(QDropEvent *de)
{
    const QMimeData* mime = de->mimeData();
    qDebug() << mime->formats();
    de->accept();
}
