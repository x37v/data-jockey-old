#include "audioworktablemodel.hpp"
#include <vector>
#include <sstream>
#include <QSqlDatabase>
#include <QSqlRecord>

#include <iostream>

using std::cout;

bool AudioWorkTableModel::cInited = false;
std::string AudioWorkTableModel::cFilteredQuery;
std::string AudioWorkTableModel::cUnFilteredQuery;

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
		"\talbums.name album, " << std::endl <<
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

	queryAfterFilter << "group by audio_works.id" << std::endl <<
			"order by artists.name, albums.name, audio_works.name";

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
	if(!cInited)
		init(db);
	setQuery(cFilteredQuery.c_str(), db);
}

void AudioWorkTableModel::setFiltered(bool filtered){
	if(filtered)
		setQuery(cFilteredQuery.c_str());
	else
		setQuery(cUnFilteredQuery.c_str());
}

