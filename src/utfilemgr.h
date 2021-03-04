#ifndef UTFILEMGR_H
#define UTFILEMGR_H

#include <QObject>
#include <QVariant>

class UTFileMgr : public QObject
{
    Q_OBJECT
public:
    explicit UTFileMgr(QString appDir, QObject *parent = nullptr);

    Q_INVOKABLE void importFile(QString url);
    Q_INVOKABLE void removeFile(QString filePath);

signals:
    void fileImported(QVariant filePath);

private:
    QString m_AppDataDir;

    void makeSureDirExistsAndIsWritable(const QString& dirFullPath);

};
#endif // UTFILEMGR_H
