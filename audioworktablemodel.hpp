#ifndef AUDIO_WORK_TABLE_MODEL_HPP
#define AUDIO_WORK_TABLE_MODEL_HPP

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <string>

class QSqlDatabase;
class QSqlRecord;

class AudioWorkTableModel : public QSqlQueryModel {
	Q_OBJECT
	public:
		static void init(const QSqlDatabase & db = QSqlDatabase());
		AudioWorkTableModel(
				const QSqlDatabase & db = QSqlDatabase(),
				QObject * parent = NULL
				);
		void setFiltered(bool filtered = true);
	private:
		static bool cInited;
		static std::string cFilteredQuery;
		static std::string cUnFilteredQuery;
};

#endif

