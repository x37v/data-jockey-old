#ifndef WORK_TAG_MODEL_FILTER_HPP
#define WORK_TAG_MODEL_FILTER_HPP

#include <QSortFilterProxyModel>
#include <QSqlQuery>
#include <QModelIndex>
#include <set>

class TagModel;

class WorkTagModelFilter : public QSortFilterProxyModel {
	Q_OBJECT
	public:
		WorkTagModelFilter(TagModel * model);
		int work();
		TagModel * sourceTagModel();
		virtual bool filterAcceptsRow(int sourceRow,
				const QModelIndex &sourceParent) const;
		//the below are here to allow for droping of drag data
		Qt::DropActions supportedDropActions() const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		virtual bool dropMimeData( const QMimeData * data, 
				Qt::DropAction action, int row, int column, 
				const QModelIndex & parent );
	signals:
		void workChanged(int);
	public slots:
		void setWork(int work);
		void clear();
		void refilter();
	private:
		QSqlQuery mWorkTagsQuery;
		int mWork;
		std::set<int> mTagClassIds;
		std::set<int> mTagIds;
		TagModel * mModel;
};

#endif

