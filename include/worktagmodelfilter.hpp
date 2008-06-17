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
		bool filterAcceptsRow(int sourceRow,
				const QModelIndex &sourceParent) const;
	signals:
		void workChanged(int);
	public slots:
		void setWork(int work);
		void clear();
	private:
		QSqlQuery mWorkTagsQuery;
		int mWork;
		std::set<int> mTagClassIds;
		std::set<int> mTagIds;
};

#endif

