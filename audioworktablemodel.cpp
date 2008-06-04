#include "audioworktablemodel.hpp"
#include <vector>
#include <sstream>
#include <QSqlDatabase>
#include <QSqlRecord>

#include <iostream>

bool AudioWorkTableModel::cInited = false;
std::string AudioWorkTableModel::cFilteredQuery;
std::string AudioWorkTableModel::cUnFilteredQuery;
std::string AudioWorkTableModel::cSortByArtistASC = "order by artists.name ASC, albums.name ASC, track ASC, audio_works.name ASC";
std::string AudioWorkTableModel::cSortByArtistDESC = "order by artists.name DESC, albums.name ASC, track ASC, audio_works.name ASC";
std::string AudioWorkTableModel::cSortByAlbumASC = "order by albums.name ASC, track ASC, artists.name ASC, audio_works.name ASC";
std::string AudioWorkTableModel::cSortByAlbumDESC = "order by albums.name DESC, track ASC, artists.name ASC, audio_works.name ASC";
std::string AudioWorkTableModel::cSortByTitleASC = "order by audio_works.name ASC, artists.name ASC, albums.name ASC, track ASC ";
std::string AudioWorkTableModel::cSortByTitleDESC = "order by audio_works.name DESC, artists.name ASC, albums.name ASC, track ASC";
std::string AudioWorkTableModel::cSortByOther = ", artists.name ASC, albums.name ASC, track ASC, audio_works.name ASC";

void AudioWorkTableModel::init(const QSqlDatabase & db){
	if(cInited)
		return;
	std::stringstream queryBeforeFilter, queryAfterFilter;
	std::vector<std::string> descriptorJoinClauses;
	std::vector<std::string> descriptorFieldNames;
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
		std::string name(descriptorTypeTable.value(nameCol).toString().toStdString());
		int id = descriptorTypeTable.value(idCol).toInt();
		fieldName << "d" << id << ".float_value as '" << name << "'";
		join << "left join descriptors as d" << id << 
			" on audio_works.id = d" << id << 
			".audio_work_id and d" << id << ".descriptor_type_id = " << id; 
		descriptorFieldNames.push_back(fieldName.str());
		descriptorJoinClauses.push_back(join.str());
	}

	std::stringstream mainQuery;
	queryBeforeFilter << "select " <<  std::endl <<
		"\taudio_works.id as id," << std::endl <<
		"\tartists.name as artist," << std::endl <<
		"\taudio_works.name as title," << std::endl <<
		"\talbums.name as album, " << std::endl <<
		"\talbum_audio_works.track as track, " << std::endl <<
		"\taudio_files.milliseconds as duration, " << std::endl;
	for(unsigned int i = 0; i < descriptorFieldNames.size() - 1; i++)
		queryBeforeFilter << "\t" << descriptorFieldNames[i] << ", " << std::endl;
	queryBeforeFilter << "\t" << descriptorFieldNames.back() << " " << std::endl;

	queryBeforeFilter << "from audio_works " << std::endl;
	queryAfterFilter <<
		"\tinner join audio_files on audio_files.id = audio_works.audio_file_id " << std::endl <<
		"\tinner join artist_audio_works on artist_audio_works.audio_work_id = audio_works.id  " << std::endl <<
		"\tinner join artists on artists.id = artist_audio_works.artist_id " << std::endl <<
		"\tinner join album_audio_works on audio_works.id = album_audio_works.audio_work_id " << std::endl <<
		"\tinner join albums on album_audio_works.album_id = albums.id " << std::endl;
	for(unsigned int i = 0; i < descriptorJoinClauses.size(); i++)
		queryAfterFilter << "\t" << descriptorJoinClauses[i] << std::endl;

	queryAfterFilter << "group by audio_works.id";
		//<< std::endl <<
			//"order by artists.name, albums.name, audio_works.name";

	std::stringstream tmpUnfiltered;
	tmpUnfiltered << queryBeforeFilter.str() << queryAfterFilter.str();
	cUnFilteredQuery = tmpUnfiltered.str();

	std::stringstream tmpFiltered;
	tmpFiltered << queryBeforeFilter.str() <<
		"\tjoin filtered_audio_works on audio_works.id = filtered_audio_works.audio_work_id" << std::endl <<
		queryAfterFilter.str();
	cFilteredQuery = tmpFiltered.str();
	cInited = true;
}

AudioWorkTableModel::AudioWorkTableModel(
		const QSqlDatabase & db,
		QObject * parent):
	QSqlQueryModel(parent)
{
	std::stringstream query;
	if(!cInited)
		init(db);
	mSortString = cSortByArtistASC;
	query << cFilteredQuery.c_str() << std::endl << mSortString << std::endl;
	setQuery(query.str().c_str(), db);
}

//XXX this is sort of a hack..
void AudioWorkTableModel::sort(int column, Qt::SortOrder order){
	std::stringstream queryStr;
	//build up the sortBy string..
	switch(column){
		case 1:
			if(order == Qt::AscendingOrder)
				mSortString = cSortByArtistASC;
			else
				mSortString = cSortByArtistDESC;
			break;
		case 2:
			if(order == Qt::AscendingOrder)
				mSortString = cSortByTitleASC;
			else
				mSortString = cSortByTitleDESC;
			break;
		case 3:
			if(order == Qt::AscendingOrder)
				mSortString = cSortByAlbumASC;
			else
				mSortString = cSortByAlbumDESC;
			break;
		default:
			//if we're not in one of the above columns then first sort by
			//that columns data up/down, then by artist down, album down, title down.
			QVariant header = headerData(column, Qt::Horizontal);
			if(header.isValid() && header.canConvert(QVariant::String)){
				std::stringstream tmp;
				tmp << "order by `" << header.toString().toStdString();
				if(order == Qt::AscendingOrder)
					tmp << "` ASC" << cSortByOther << std::endl;
				else
					tmp << "` DESC" << cSortByOther << std::endl;
				mSortString = tmp.str();
			}
			break;
	};
	queryStr << cFilteredQuery.c_str() << std::endl << mSortString;
	setQuery(queryStr.str().c_str());
	query();
}

void AudioWorkTableModel::setFiltered(bool filtered){
	std::stringstream query;
	if(filtered)
		query << cFilteredQuery;
	else
		query << cUnFilteredQuery;
	query << std::endl << mSortString << std::endl;
	setQuery(query.str().c_str());
}

/*
QVariant AudioWorkTableModel::data( const QModelIndex & item, int role) const {
	QVariant ret = QSqlQueryModel::data(item,role);
	if(item.isValid() && (item.column() == 4) && (role == Qt::DisplayRole)){
		QVariant itemData = item.data(Qt::DisplayRole);
			//item.data(Qt::DisplayRole).canConvert(QVariant::UInt)){
			//QString timeString;
			//QString seconds;
			//QString minutes;
			//unsigned int ms = item.data(Qt::DisplayRole).toUInt();
			//minutes.setNum(ms / 60000);
	}
	return ret;
}
*/
