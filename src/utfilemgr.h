#ifndef UTFILEMGR_H
#define UTFILEMGR_H

#include <QObject>

class UTFileMgr : public QObject
{
    Q_OBJECT
public:
    explicit UTFileMgr(QString appDir, QObject *parent = nullptr);

    Q_INVOKABLE void importFile(QString url);

private:
    QString m_AppDataDir;

    static void makeSureDirExistsAndIsWritable(const QString& dirFullPath);

};
#endif // UTFILEMGR_H
