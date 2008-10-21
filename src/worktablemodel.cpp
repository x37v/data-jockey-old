#include "worktablemodel.hpp"
#include <vector>
#include <sstream>
#include <QSqlDatabase>
#include <QSqlRecord>

#include <iostream>

const unsigned int WorkTableModel::idColumn = 0;
const unsigned int WorkTableModel::artistColumn = 1;
const unsigned int WorkTableModel::titleColumn = 2;
const unsigned int WorkTableModel::albumColumn = 3;
const unsigned int WorkTableModel::trackColumn = 4;

bool WorkTableModel::cInited = false;
QString WorkTableModel::cQuery;

void WorkTableModel::init(const QSqlDatabase & db){
	if(cInited)
		return;
	std::vector<QString> descriptorJoinClauses;
	std::vector<QString> descriptorFieldNames;
	int nameCol;
	int idCol;
	//grab the descriptor types
	QSqlQuery descriptorTypeTable("SELECT id, name FROM descriptor_types", db);
	descriptorTypeTable.exec();
	QSqlRecord rec = descriptorTypeTable.record();

	nameCol = rec.indexOf("name");
	idCol = rec.indexOf("id");
	while(descriptorTypeTable.next()){
		std::stringstream fieldName;
		std::stringstream join;
		QString name(descriptorTypeTable.value(nameCol).toString());
		int id = descriptorTypeTable.value(idCol).toInt();
		fieldName << "d" << id << ".float_value as '" << name.toStdString() << "'";
		join << "left join descriptors as d" << id << 
			" on audio_works.id = d" << id << 
			".audio_work_id and d" << id << ".descriptor_type_id = " << id; 
		descriptorFieldNames.push_back(QString(fieldName.str().c_str()));
		descriptorJoinClauses.push_back(QString(join.str().c_str()));
	}

	QString queryStr;
	queryStr.append("select\n"
		"\taudio_works.id as id,\n"
		"\tartists.name as artist,\n"
		"\taudio_works.name as title,\n"
		"\talbums.name as album, \n"
		"\talbum_audio_works.track as track, \n"
		"\taudio_files.milliseconds as duration, \n"
		"\taudio_file_types.name as \"file type\", ");

	for(unsigned int i = 0; i < descriptorFieldNames.size() - 1; i++){
		queryStr.append("\t");
		queryStr.append(descriptorFieldNames[i]);
		queryStr.append(", \n");
	}
	queryStr.append("\t");
	queryStr.append(descriptorFieldNames.back());
	queryStr.append(" \n");

	queryStr.append("from audio_works \n"
		"\tinner join audio_files on audio_files.id = audio_works.audio_file_id \n"
		"\tinner join audio_file_types on audio_file_types.id = audio_files.audio_file_type_id \n"
		"\tinner join artist_audio_works on artist_audio_works.audio_work_id = audio_works.id  \n"
		"\tinner join artists on artists.id = artist_audio_works.artist_id \n"
		"\tinner join album_audio_works on audio_works.id = album_audio_works.audio_work_id \n"
		"\tinner join albums on album_audio_works.album_id = albums.id \n");
	for(unsigned int i = 0; i < descriptorJoinClauses.size(); i++){
		queryStr.append("\t");
		queryStr.append(descriptorJoinClauses[i]);
		queryStr.append("\n");
	}

	queryStr.append("group by audio_works.id\n");
	queryStr.append("order by artists.name ASC, albums.name ASC, track ASC, audio_works.name ASC\n");

	cQuery = queryStr;
	cInited = true;
}

WorkTableModel::WorkTableModel(
		const QSqlDatabase & db,
		QObject * parent):
	QSqlQueryModel(parent)
{
	std::stringstream query;
	mDB = &db;
	if(!cInited)
		init(db);
	setQuery(cQuery, db);
}

//reformat the time display role so that it is in MM:SS not milliseconds
QVariant WorkTableModel::data( const QModelIndex & index, int role) const {
	QVariant ret = QSqlQueryModel::data(index,role);
	if(index.isValid() && (index.column() == TIME_COLUMN) && (role == Qt::DisplayRole)){
		QVariant itemData = QSqlQueryModel::data(index, Qt::DisplayRole);
		if(itemData.canConvert(QVariant::Int)){
			QString timeString;
			int seconds;
			int minutes;
			int ms = itemData.toInt();
			minutes = ms / 60000;
			seconds = (ms - (minutes * 60000)) / 1000;
			timeString.sprintf("%02d:%02d", minutes, seconds);
			return QString(timeString);
		}
	}
	return ret;
}

int WorkTableModel::findWorkByPath(std::string path){
	QString queryStr;
	//construct the query
	queryStr.append("select audio_works.id from audio_files"
			" left join audio_works on audio_files.id = audio_works.audio_file_id"
			" where location = '");
	queryStr.append(path.c_str());
	queryStr.append("'");

	//execute the query
	QSqlQuery query(queryStr, *mDB);
	query.exec();
	//if there are results return the index, otherwise return -1
	if(query.next())
		return query.value(0).toInt();
	return -1;
}

