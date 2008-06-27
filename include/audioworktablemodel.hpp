#ifndef AUDIO_WORK_TABLE_MODEL_HPP
#define AUDIO_WORK_TABLE_MODEL_HPP

//QSortFilterProxyModel

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QString>

class QSqlDatabase;
class QSqlRecord;
#define TIME_COLUMN 5

class AudioWorkTableModel : public QSqlQueryModel {
	Q_OBJECT
	public:
		AudioWorkTableModel(
				const QSqlDatabase & db,
				QObject * parent = NULL
				);
		virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

		const static unsigned int idColumn;
		const static unsigned int artistColumn;
		const static unsigned int albumColumn;
		const static unsigned int titleColumn;
		const static unsigned int trackColumn;
	private:
		static void init(const QSqlDatabase & db);
		static bool cInited;
		static QString cQuery;
};

#endif

