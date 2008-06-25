#ifndef AUDIO_WORK_DB_VIEW_HPP
#define AUDIO_WORK_DB_VIEW_HPP

#include <QWidget>
#include <QModelIndex>
#include <QItemSelection>

class QTableView;
class QPushButton;
class QAbstractItemModel;

class AudioWorkDBView : public QWidget {
	Q_OBJECT
	public:
		AudioWorkDBView(QAbstractItemModel * model, QWidget *parent = NULL);
		QTableView * tableView();
		QPushButton * applyFilterButton();
		QPushButton * removeFilterButton();
	protected slots:
		void selectWork(const QModelIndex & index);
		void selectionChanged( const QItemSelection & selected);
		void setFiltered();
		void setUnFiltered();
	signals:
		void workSelected(int work);
	private:
		QTableView * mTableView;
		QPushButton * mApplyFilterButton;
		QPushButton * mRemoveFilterButton;
};

#endif

