#ifndef WORK_FILTER_MODEL_HPP
#define WORK_FILTER_MODEL_HPP

#include <QObject>
#include <QSortFilterProxyModel>
#include <string>

class AudioWorkTableModel;

class WorkFilterModel : public QObject {
	Q_OBJECT
	public:
		WorkFilterModel(QObject * parent = NULL);
		virtual ~WorkFilterModel();
		//beforeFilter indicates if the filtering actually needs to be done
		//for instance, if the filter is based on tempo and them tempo hasn't
		//changed, then no work needs to be done
		virtual bool beforeFilter();
		virtual bool acceptsWork(int work_id) = 0;
		virtual std::string description() = 0;
		virtual std::string name() = 0;
};

class TagSelectionFilter : public WorkFilterModel {
	Q_OBJECT
	public:
		TagSelectionFilter(QObject * parent = NULL);
		virtual bool acceptsWork(int work_id);
		virtual std::string description();
		virtual std::string name();
};

//this is the one that actually does the filtering [based on a WorkFilterModel]
class WorkFilterModelProxy : public QSortFilterProxyModel {
	Q_OBJECT
	public:
		WorkFilterModelProxy(AudioWorkTableModel * parent = NULL);
		WorkFilterModel * workFilter() const;
		virtual bool filterAcceptsRow(int sourceRow,
				const QModelIndex &sourceParent) const;
		virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	public slots:
		void setFilter(WorkFilterModel * model);
		//should we filter or let everything through?
		void filter(bool filter = true);
		void clearFilter();
	private:
		WorkFilterModel * mFilter;
		bool mFiltering;
};

#endif

