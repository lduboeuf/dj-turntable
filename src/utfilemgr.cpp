#include "utfilemgr.h"
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

UTFileMgr::UTFileMgr(QString appDir, QObject *parent) : QObject(parent)
{

    m_AppDataDir = appDir;
    qDebug() << "AppDir:" + m_AppDataDir;
    // Default user location
    if (!QFile(m_AppDataDir).exists())
    {
        qWarning() << "User config directory does not exist: " << QDir::toNativeSeparators(m_AppDataDir);
    }
    try
    {
        makeSureDirExistsAndIsWritable(m_AppDataDir);
    }
    catch (std::runtime_error &e)
    {
        qFatal("Error: cannot create user config directory: %s", e.what());
    }
}


void UTFileMgr::importFile(QString url){

    QString destFile = m_AppDataDir + QDir::separator() +QFileInfo(url).fileName();

   if (QFile::exists(destFile))
   {
       QFile::remove(destFile);
   }

   if (QFile::copy(url, destFile)){
       qDebug() << "Copied file to:" + destFile;
   }else{
        qWarning() << "Error while copying:" + url + " to:" + destFile;
   }
   emit fileImported(destFile);

}

void UTFileMgr::removeFile(QString filePath)
{
    QFileInfo file(filePath);
    if (file.exists()) {
        QFile::remove(filePath);
    }

}

void UTFileMgr::makeSureDirExistsAndIsWritable(const QString& dirFullPath)
{
    // Check that the dirFullPath directory exists
    QFileInfo uDir(dirFullPath);
    if (!uDir.exists())
    {
        // The modules directory doesn't exist, lets create it.
        qDebug() << "Creating directory " << QDir::toNativeSeparators(uDir.filePath());
        if (!QDir("/").mkpath(uDir.filePath()))
        {
            throw std::runtime_error(QString("Could not create directory: " +uDir.filePath()).toStdString());
        }
        QFileInfo uDir2(dirFullPath);
        if (!uDir2.isWritable())
        {
            throw std::runtime_error(QString("Directory is not writable: " +uDir2.filePath()).toStdString());
        }
    }
    else if (!uDir.isWritable())
    {
        throw std::runtime_error(QString("Directory is not writable: " +uDir.filePath()).toStdString());
    }
}
