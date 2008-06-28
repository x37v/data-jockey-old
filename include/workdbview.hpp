#ifndef WORK_DB_VIEW_HPP
#define WORK_DB_VIEW_HPP

#include <QWidget>
#include <QModelIndex>
#include <QItemSelection>

class QTableView;
class QPushButton;
class QAbstractItemModel;

class WorkDBView : public QWidget {
	Q_OBJECT
	public:
		WorkDBView(QAbstractItemModel * model, QWidget *parent = NULL);
		QTableView * tableView();
		QPushButton * applyFilterButton();
		QPushButton * removeFilterButton();
	protected slots:
		void selectWork(const QModelIndex & index);
		void setSelection( const QItemSelection & selected);
		void setFiltered();
		void setUnFiltered();
	signals:
		void workSelected(int work);
		void applyFilterPushed();
		void removeFilterPushed();
	private:
		QTableView * mTableView;
		QPushButton * mApplyFilterButton;
		QPushButton * mRemoveFilterButton;
};

#endif

