#include "headers/parameters.h"
#include "headers/installmaps.h"

InstallMaps::InstallMaps(QObject *parent, const QSqlDatabase *_db) :
  QThread(parent),
  occupied(false),
  abort(false),
  db(_db)
{
}

InstallMaps::~InstallMaps(){
  abort = true;
  wait();
}

void InstallMaps::installMaps(){
  if(!occupied){
    occupied = true;
    abort = false;

    start();
  }
}

void InstallMaps::sError(QString txt, int line){
  qWarning() << txt << " - AT LINE:" << line;
}

void InstallMaps::stopInstallation(){
  abort = true;
}

void InstallMaps::run(){
  bool ok = true;

  //this will search for any OSM files in the directory "maps" inside the executable folder
  const QString filters = "*.osm";
  QDir dirIt(E_MAPSFOLDER, filters);

  //it will emit a signal with the quantity of files found
  emit osmFilesFound(dirIt.count());

  if(dirIt.count() > 0)
    emit osmReadingPercentage(0);

  int tt = 0, tr = 0, rtr = 0, rtr2 = 0, atr = 0;
  uint percentage = 0;

  //counting all the lines to know the percentage
  for(int i = 0; i < dirIt.count(); i++){
    const QString fileName = dirIt.filePath(dirIt.entryList().at(i));
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
      sError(file.errorString(), __LINE__);
    else{
      QXmlStreamReader reader(&file);
      //the next loop will only obtain the quantity of lines from the OSM file
      //obtain the total number of elements inside the osm file
      while(!reader.atEnd()){
        if(reader.isStartElement() && !reader.isWhitespace()){
          if(reader.name() != ""){
            tt++;//increases in each element reading
          }
        }
        reader.readNext();
      }

      atr = qCeil(tt/100.0f);//represents the quantity of elements equal to one percent

      file.close();
    }
  }

  QSqlQuery searchNode(*db), searchWay(*db), searchRelation(*db), searchNds(*db), searchMember(*db);
  QSqlQuery searchTagNodes(*db), searchTagWays(*db), searchTagRelations(*db);
  QSqlQuery deleteNds(*db);
  QSqlQuery updateNode(*db), updateNode2(*db), updateWay(*db), updateWay2(*db), updateRelation(*db), updateRelation2(*db), updateNds(*db), updateMember(*db);
  QSqlQuery updateTagNodes(*db), updateTagWays(*db), updateTagRelations(*db);
  QSqlQuery insertNode(*db), insertNode2(*db), insertWay(*db), insertWay2(*db), insertRelation(*db), insertRelation2(*db), insertNds(*db), insertMember(*db);

  searchNode.prepare("SELECT cdate, lat, lon FROM gps_map.nodes WHERE ref=? LIMIT 1;");
  searchWay.prepare("SELECT cdate FROM gps_map.ways WHERE ref=? LIMIT 1;");
  searchRelation.prepare("SELECT cdate FROM gps_map.relations WHERE ref=? LIMIT 1;");
  searchNds.prepare("SELECT pway FROM gps_map.nds WHERE (way=?) AND (ref=?) LIMIT 1;");
  searchMember.prepare("SELECT type, role FROM gps_map.members WHERE (rel=?) AND (ref=?) LIMIT 1;");

  searchTagNodes.prepare("SELECT tags FROM gps_map.nodes WHERE ref=?;");
  searchTagWays.prepare("SELECT tags FROM gps_map.ways WHERE ref=?;");
  searchTagRelations.prepare("SELECT tags FROM gps_map.relations WHERE ref=?;");

  deleteNds.prepare("DELETE FROM gps_map.nds WHERE way=?;");

  updateNode.prepare("UPDATE gps_map.nodes SET cdate=?, lat=?, lon=?, tags=NULL WHERE ref=?;");
  updateNode2.prepare("UPDATE gps_map.nodes SET lat=?, lon=?, tags=NULL WHERE ref=?;");
  updateWay.prepare("UPDATE gps_map.ways SET cdate=?, tags=NULL WHERE ref=?;");
  updateWay2.prepare("UPDATE gps_map.ways SET tags=NULL WHERE ref=?;");
  updateRelation.prepare("UPDATE gps_map.relations SET cdate=?, tags=NULL WHERE ref=?;");
  updateRelation2.prepare("UPDATE gps_map.relations SET tags=NULL WHERE ref=?;");
  updateNds.prepare("UPDATE gps_map.nds SET pway=? WHERE (way=?) AND (ref=?)");
  updateMember.prepare("UPDATE gps_map.members SET type=?, role=? WHERE (rel=?) AND (ref=?);");

  updateTagNodes.prepare("UPDATE gps_map.nodes SET tags=? WHERE ref=?;");
  updateTagWays.prepare("UPDATE gps_map.ways SET tags=? WHERE ref=?;");
  updateTagRelations.prepare("UPDATE gps_map.relations SET tags=? WHERE ref=?;");

  insertNode.prepare("INSERT INTO gps_map.nodes (ref, cdate, lat, lon) VALUES (?, ?, ?, ?);");
  insertNode2.prepare("INSERT INTO gps_map.nodes (ref, lat, lon) VALUES (?, ?, ?);");
  insertWay.prepare("INSERT INTO gps_map.ways (ref, cdate) VALUES (?, ?);");
  insertWay2.prepare("INSERT INTO gps_map.ways (ref) VALUES (?);");
  insertRelation.prepare("INSERT INTO gps_map.relations (ref, cdate) VALUES (?, ?);");
  insertRelation2.prepare("INSERT INTO gps_map.relations (ref) VALUES (?);");
  insertNds.prepare("INSERT INTO gps_map.nds (ref, way, pway) VALUES (?, ?, ?)");
  insertMember.prepare("INSERT INTO gps_map.members (type, role, rel, ref) VALUES (?, ?, ?, ?);");

  //this will go through all the osm files
  for(int i = 0; i < dirIt.count(); i++){
    const QString fileName = dirIt.filePath(dirIt.entryList().at(i));
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
      sError(file.errorString(), __LINE__);
    else{
      QXmlStreamReader reader(&file);
      QString tags, cdate, role, mtype, pretag;
      int type = 0, eln = 0;
      long long ref1 = 0, ref2 = 0;
      double lat = 0, lon = 0;

      //read all the elements and save them into the database
      while(!reader.atEnd()){
        if(reader.isStartElement() && !reader.isWhitespace()){
          if(reader.name() != ""){
            if((reader.name() == "node") || (reader.name() == "way") || (reader.name() == "relation")){
              type = -1;
              //timestamp="2016-05-01T13:38:10Z"
              cdate = reader.attributes().value("timestamp").toString();
              ref2 = ref1 = reader.attributes().value("id").toLongLong();

              if(reader.name() == "node"){
                type = 0;
                lat = reader.attributes().value("lat").toDouble();
                lon = reader.attributes().value("lon").toDouble();
                searchNode.bindValue(0, ref1);
                if(!searchNode.exec())
                  sError(searchNode.lastError().databaseText(), __LINE__);
                else{
                  if(searchNode.size() > 0){
                    searchNode.first();
                    if((cdate == "") && ((searchNode.value(1).toDouble() != lat) || (searchNode.value(2).toDouble() != lon))){
                      updateNode2.bindValue(0, lat);
                      updateNode2.bindValue(1, lon);
                      updateNode2.bindValue(2, ref1);
                      if(!updateNode2.exec())
                        sError(updateNode2.lastError().databaseText(), __LINE__);
                    }else if((searchNode.value(0).toString() != cdate) || (searchNode.value(1).toDouble() != lat) || (searchNode.value(2).toDouble() != lon)){
                      updateNode.bindValue(0, cdate);
                      updateNode.bindValue(1, lat);
                      updateNode.bindValue(2, lon);
                      updateNode.bindValue(3, ref1);
                      if(!updateNode.exec())
                        sError(updateNode.lastError().databaseText(), __LINE__);
                    }
                  }else if(searchNode.size() == 0){
                    if(cdate == ""){
                      insertNode2.bindValue(0, ref1);
                      insertNode2.bindValue(1, lat);
                      insertNode2.bindValue(2, lon);
                      if(!insertNode2.exec())
                        sError(insertNode2.lastError().databaseText(), __LINE__);
                    }else{
                      insertNode.bindValue(0, ref1);
                      insertNode.bindValue(1, cdate);
                      insertNode.bindValue(2, lat);
                      insertNode.bindValue(3, lon);
                      if(!insertNode.exec())
                        sError(insertNode.lastError().databaseText(), __LINE__);
                    }
                  }
                }
              }else if(reader.name() == "way"){
                type = 1;
                eln = 0;
                searchWay.bindValue(0, ref1);
                if(!searchWay.exec())
                  sError(searchWay.lastError().databaseText(), __LINE__);
                else{
                  if(searchWay.size() > 0){
                    deleteNds.bindValue(0, ref2);
                    if(!deleteNds.exec())
                      sError(deleteNds.lastError().databaseText(), __LINE__);
                    searchWay.first();
                    if(cdate == ""){
                      updateWay2.bindValue(0, ref1);
                      if(!updateWay2.exec())
                        sError(updateWay2.lastError().databaseText(), __LINE__);
                    }else if(searchWay.value(0).toString() != cdate){
                      updateWay.bindValue(0, cdate);
                      updateWay.bindValue(1, ref1);
                      if(!updateWay.exec())
                        sError(updateWay.lastError().databaseText(), __LINE__);
                    }
                  }else if(searchWay.size() == 0){
                    if(cdate == ""){
                      insertWay2.bindValue(0, ref1);
                      if(!insertWay2.exec())
                        sError(insertWay2.lastError().databaseText(), __LINE__);
                    }else{
                      insertWay.bindValue(0, ref1);
                      insertWay.bindValue(1, cdate);
                      if(!insertWay.exec())
                        sError(insertWay.lastError().databaseText(), __LINE__);
                    }
                  }
                }
              }else if(reader.name() == "relation"){
                type = 2;
                searchRelation.bindValue(0, ref1);
                if(!searchRelation.exec())
                  sError(searchRelation.lastError().databaseText(), __LINE__);
                else{
                  if(searchRelation.size() > 0){
                    searchRelation.first();
                    if(cdate == ""){
                      updateRelation2.bindValue(0, ref1);
                      if(!updateRelation2.exec())
                        sError(updateRelation2.lastError().databaseText(), __LINE__);
                    }else if(searchRelation.value(0).toString() != cdate){
                      updateRelation.bindValue(0, cdate);
                      updateRelation.bindValue(1, ref1);
                      if(!updateRelation.exec())
                        sError(updateRelation.lastError().databaseText(), __LINE__);
                    }
                  }else if(searchRelation.size() == 0){
                    if(cdate == ""){
                      insertRelation2.bindValue(0, ref1);
                      if(!insertRelation2.exec())
                        sError(insertRelation2.lastError().databaseText(), __LINE__);
                    }else{
                      insertRelation.bindValue(0, ref1);
                      insertRelation.bindValue(1, cdate);
                      if(!insertRelation.exec())
                        sError(insertRelation.lastError().databaseText(), __LINE__);
                    }
                  }
                }
              }
            }else if(reader.name() == "tag"){
              if(type >= 0){
                tags = reader.attributes().value("k").toString()+"="+reader.attributes().value("v").toString();

                if(type == 0){
                  searchTagNodes.bindValue(0, ref2);
                  if(!searchTagNodes.exec())
                    sError(searchTagNodes.lastError().databaseText(), __LINE__);
                  else{
                    while(searchTagNodes.next()){
                      pretag = searchTagNodes.value(0).toString();
                      if(pretag != "")
                        tags = pretag + "+" + tags;
                      updateTagNodes.bindValue(0, tags);
                      updateTagNodes.bindValue(1, ref2);
                      if(!updateTagNodes.exec())
                        sError(updateTagNodes.lastError().databaseText(), __LINE__);
                    }
                  }
                }else if(type == 1){
                  searchTagWays.bindValue(0, ref2);
                  if(!searchTagWays.exec())
                    sError(searchTagWays.lastError().databaseText(), __LINE__);
                  else{
                    while(searchTagWays.next()){
                      pretag = searchTagWays.value(0).toString();
                      if(pretag != "")
                        tags = pretag + "+" + tags;
                      updateTagWays.bindValue(0, tags);
                      updateTagWays.bindValue(1, ref2);
                      if(!updateTagWays.exec())
                        sError(updateTagWays.lastError().databaseText(), __LINE__);
                    }
                  }
                }else if(type == 2){
                  searchTagRelations.bindValue(0, ref2);
                  if(!searchTagRelations.exec())
                    sError(searchTagRelations.lastError().databaseText(), __LINE__);
                  else{
                    while(searchTagRelations.next()){
                      pretag = searchTagRelations.value(0).toString();
                      if(pretag != "")
                        tags = pretag + "+" + tags;
                      updateTagRelations.bindValue(0, tags);
                      updateTagRelations.bindValue(1, ref2);
                      if(!updateTagRelations.exec())
                        sError(updateTagRelations.lastError().databaseText(), __LINE__);
                    }
                  }
                }
              }
            }else if(reader.name() == "member"){
              if(type >= 0){
                mtype = reader.attributes().value("type").toString();
                role = reader.attributes().value("role").toString();
                role = (role == "")? NULL : role;
                ref1 = reader.attributes().value("ref").toLongLong();

                //look for previous inserted members
                searchMember.bindValue(0, ref2);
                searchMember.bindValue(1, ref1);
                if(!searchMember.exec()){
                  sError(searchMember.lastError().databaseText(), __LINE__);
                }else{
                  if(searchMember.size() > 0){
                    searchMember.first();
                    if((searchMember.value(0).toString() != mtype) || (searchMember.value(1).toString() != role)){
                      //if found one it will update the data
                      updateMember.bindValue(0, mtype);
                      updateMember.bindValue(1, role);
                      updateMember.bindValue(2, ref2);
                      updateMember.bindValue(3, ref1);
                      if(!updateMember.exec())
                        sError(updateMember.lastError().databaseText(), __LINE__);
                    }
                  }else if(searchMember.size() == 0){
                    //if not found, it will insert new data
                    insertMember.bindValue(0, mtype);
                    insertMember.bindValue(1, role);
                    insertMember.bindValue(2, ref2);
                    insertMember.bindValue(3, ref1);
                    if(!insertMember.exec())
                      sError(insertMember.lastError().databaseText(), __LINE__);
                  }
                }
              }
            }else if(reader.name() == "nd"){
              if(type >= 0){
                ref1 = reader.attributes().value("ref").toLongLong();

                //look for previous inserted nodes
                searchNds.bindValue(0, ref2);
                searchNds.bindValue(1, ref1);
                if(!searchNds.exec()){
                  sError(searchNds.lastError().databaseText(), __LINE__);
                }else{
                  if(searchNds.size() > 0){
                    searchNds.first();
                    if(searchNds.value(0).toInt() != eln){
                      //if found one it will update the data
                      updateNds.bindValue(0, eln);
                      updateNds.bindValue(1, ref2);
                      updateNds.bindValue(2, ref1);
                      if(!updateNds.exec()){
                        sError(updateNds.lastError().databaseText(), __LINE__);
                      }else
                        eln++;
                    }else
                      eln++;
                  }else if(searchNds.size() == 0){
                    //if not found, it will insert new data
                    insertNds.bindValue(0, ref1);
                    insertNds.bindValue(1, ref2);
                    insertNds.bindValue(2, eln);
                    if(!insertNds.exec()){
                      sError(insertNds.lastError().databaseText(), __LINE__);
                    }else
                      eln++;
                  }
                }
              }
            }
            tr++;//increases in each element reading
            rtr++;
            rtr2++;
            //compares if it has been read the quantity equivalent to one unity of percentage
            if(rtr >= atr){
              percentage++;
              //put here a trigger if you want to do something in each percentage change
              emit osmReadingPercentage(percentage);
              rtr = 0;
            }
            if(rtr2 >= 150){
              emit movementFeeling();
              rtr2 = 0;
            }
          }
        }
        if(abort){
          occupied = false;
          return;
        }

        reader.readNext();
      }

      file.close();
    }
    if(file.remove()){
      qWarning("OSM file removed");
    }else{
      emit osmFileError(fileName, file.errorString());
      ok = false;
    }
  }

  if(ok == true){
    emit allFilesInstalled();
    if(dirIt.count() > 0)
      emit osmReadingPercentage(100);
  }

  occupied = false;
}
