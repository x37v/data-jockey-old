#ifndef WORK_FILTER_LIST_VIEW_HPP
#define WORK_FILTER_LIST_VIEW_HPP

#include <QWidget>
#include <QItemSelection>

class WorkFilterList;
class QTableView;
class QTextEdit;
class QStandardItemModel;
class QStandardItem;

class WorkFilterListView : public QWidget {
	Q_OBJECT
	public:
		WorkFilterListView(WorkFilterList * sourceList, QWidget * parent = NULL);
	public slots:
		void addFilter(unsigned int list_size);
		void removeFilter(unsigned int index);
		//void setSelection(unsigned int index);
		void updateSelection(unsigned int index, unsigned int old_index);
	signals:
		void selectionChanged(unsigned int selection);
	protected slots:
		void listDataChanged(QStandardItem * item);
		void setSelection( const QItemSelection & selected);
	private:
		unsigned int mSelected;
		WorkFilterList * mFilterList;
		QTextEdit * mDetails;
		QTableView * mSelectionTable;
		QStandardItemModel * mModel;
};

#endif

