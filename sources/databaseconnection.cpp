#include "headers/parameters.h"
#include "headers/databaseconnection.h"

databaseConnection::databaseConnection(){

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
          GRANT ALL PRIVILEGES ON SCHEMA gps_map TO mapping WITH GRANT OPTION;


          CREATE TABLE gps_map.members (
            id   BIGSERIAL PRIMARY KEY NOT NULL,
            rel  DOUBLE PRECISION NOT NULL,
            ref  DOUBLE PRECISION NOT NULL,
            type TEXT NOT NULL,
            role TEXT NULL
          ) WITH ( OIDS=FALSE );


          CREATE TABLE gps_map.nds (
            id   BIGSERIAL PRIMARY KEY NOT NULL,
            way  DOUBLE PRECISION NOT NULL,
            ref  DOUBLE PRECISION NOT NULL,
            pway INTEGER NOT NULL
          ) WITH ( OIDS=FALSE );


          CREATE TABLE gps_map.nodes (
            id    BIGSERIAL PRIMARY KEY NOT NULL,
            ref   BIGINT NOT NULL,
            cdate TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT now(),
            lat   DOUBLE PRECISION NOT NULL,
            lon   DOUBLE PRECISION NOT NULL,
            tags  TEXT NULL
          ) WITH ( OIDS=FALSE );


          CREATE TABLE gps_map.relations (
            id    BIGSERIAL PRIMARY KEY NOT NULL,
            ref   DOUBLE PRECISION NOT NULL,
            cdate TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT now(),
            tags  TEXT NULL
          ) WITH ( OIDS=FALSE );


          CREATE TABLE gps_map.ways (
            id    BIGSERIAL PRIMARY KEY NOT NULL,
            ref   DOUBLE PRECISION NOT NULL,
            cdate TIMESTAMP WITHOUT TIME ZONE NOT NULL DEFAULT now(),
            tags  TEXT NULL
          ) WITH ( OIDS=FALSE );

          GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA gps_map TO mapping;
          GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA gps_map TO mapping;
  */

  //connecting to database
  db = QSqlDatabase::addDatabase("QPSQL");
  db.setHostName(E_SERVER);
  db.setDatabaseName(E_DATABASE); // key press: strg + left mouse click (to see the variable definitions)
  db.setUserName(E_USERNAME);
  db.setPassword(E_PASSWORD);

  if(!db.open())
    sError(db.lastError().text(), __LINE__);
}

databaseConnection::~databaseConnection(){
  db.close();
}

const QSqlDatabase *const databaseConnection::getDB(){
  return &db;
}

void databaseConnection::sError(QString txt, int line){
  qWarning() << txt << " - AT LINE:" << line;
}
