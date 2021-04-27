#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QtSql>

class databaseConnection
{
public:

  //STEP TO RUN MAPS:

  // 1.- install postgresql
  //      o - Ubuntu:
  /*
          sudo apt-get update
          sudo apt-get install postgresql postgresql-contrib
  */

  // 2.- install pgadmin3 (optional, this is a database administrator)
  //      o - Ubuntu:
  /*
          sudo apt-get install pgadmin3
  */

  // 3.- configure postgreSQL
  //      o - Ubuntu:
  //          Make sure you run line by line, if not, it may cause problems
  /*
          sudo -u postgres psql
          CREATE USER mapping WITH PASSWORD '1234567';
          CREATE DATABASE gps_map ENCODING 'UTF8';
          \connect gps_map
          CREATE SCHEMA gps_map;
          GRANT ALL PRIVILEGES ON SCHEMA gps_map TO mapping;
          GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA gps_map TO mapping;
  */

  databaseConnection();
  ~databaseConnection();
  const QSqlDatabase *const getDB();

private:
  void sError(QString txt, int line);
  QSqlDatabase db;
};

#endif // DATABASECONNECTION_H
