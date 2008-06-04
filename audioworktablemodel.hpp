#ifndef AUDIO_WORK_TABLE_MODEL_HPP
#define AUDIO_WORK_TABLE_MODEL_HPP

//QSortFilterProxyModel

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
		virtual void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );
		void setFiltered(bool filtered = true);
		//virtual QVariant data ( const QModelIndex & item, int role = Qt::DisplayRole ) const;
	private:
		static bool cInited;
		static std::string cFilteredQuery;
		static std::string cUnFilteredQuery;
		static std::string cSortByArtistASC;
		static std::string cSortByArtistDESC;
		static std::string cSortByAlbumASC;
		static std::string cSortByAlbumDESC;
		static std::string cSortByTitleASC;
		static std::string cSortByTitleDESC;
		static std::string cSortByOther;
		std::string mSortString;
};

#endif

