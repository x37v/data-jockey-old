#ifndef WORK_FILTER_LIST_HPP
#define WORK_FILTER_LIST_HPP

#include <QObject>
#include <QList>

class WorkFilterModel;

class WorkFilterList : public QObject {
	Q_OBJECT
	public:
		WorkFilterList(QObject * parent = NULL);
		WorkFilterModel * selected() const;
		unsigned int selectedIndex() const;
		QList<WorkFilterModel *> list() const;
		WorkFilterModel * at(unsigned int i) const;
		unsigned int size() const;
	signals:
		void filterAdded(unsigned int list_size);
		void filterRemoved(unsigned int index);
		void selectionChanged(unsigned int new_index, unsigned int old_index);
	public slots:
		void addFilter(WorkFilterModel * filter);
		void removeFilter(unsigned int index);
		void removeFilter(WorkFilterModel * filter);
		void selectFilter(unsigned int index);
		void selectFilter(WorkFilterModel * filter);
	private:
		int mSelected;
		QList<WorkFilterModel *> mModelList;
};

#endif

