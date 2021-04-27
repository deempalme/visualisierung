#ifndef INSTALLMAPS_H
#define INSTALLMAPS_H

#include <QtSql>
#include <QXmlStreamReader>
#include <QDir>
#include <QThread>

class InstallMaps : public QThread
{
  Q_OBJECT
public:
  InstallMaps(QObject *parent = 0, const QSqlDatabase *_db = nullptr);
  ~InstallMaps();

  void installMaps();

  bool occupied;

private:
  void sError(QString txt, int line);

  bool abort;
  const QSqlDatabase *db;

protected:
  void run() Q_DECL_OVERRIDE;

public slots:
  void stopInstallation();

signals:
  void osmFilesFound(uint _quantity);
  void osmReadingPercentage(uint _percentage);
  void allFilesInstalled();
  void osmFileError(QString _fileName, QString _errorText);
  void movementFeeling();
};

#endif // INSTALLMAPS_H
