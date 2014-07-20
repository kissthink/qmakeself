#ifndef QMAKESELF_H
#define QMAKESELF_H

#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>
#include <QDebug>
#include <QFileDialog>
#include <QString>
#include <QFileSystemModel>
#include <QDropEvent>
#include <QMimeData>
#include <QLabel>
#include <QProcess>
#include <QFile>
#include <QDataStream>

namespace Ui {
class QMakeSelf;
}


class QMakeSelf : public QMainWindow
{
    Q_OBJECT

public:
    static const int PROC_ERROR = -2;
    static const int PROC_CRASH = -1;
    static const int PROC_OK = 0;
    static const int magicID = 0x6969;
    explicit QMakeSelf(QWidget *parent = 0);
    ~QMakeSelf();

protected:
    void changeEvent(QEvent *e);

private:
    QString sourcePath;
    QString compressionType;
    int compressionLevel;
    QString tarExtra;
    bool nocrc, nomd5, notemp;
    QString label;
    QString unpackType;
    QString destination;
    bool followLinks;
    bool noProgress, nox11, nowait;
    QString lsmFile, licenseFile, headerFile, initScript;

    Ui::QMakeSelf *ui;

    void buildGUI();
    void buildToolBar();

private slots:
    void newArchive();
    void openArchive();
    void saveArchive();
    void createArchive();
    void saveSettings();
    void loadDefaultSettings();
    void configureTreeView();
    void showHelp();
    void detectAlgorithms();
    void fillUnpackTypes();
    void analyzeAlgorithmFeatures(QString);
    void analyzeUnpackFeatures(QString);
    void getParams();
    void setParams();
    void adjustTreeView();


    // QWidget interface
protected:
    void dropEvent(QDropEvent *);
};

#endif // QMAKESELF_H
