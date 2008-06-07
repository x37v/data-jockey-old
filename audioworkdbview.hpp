#ifndef AUDIO_WORK_DB_VIEW_HPP
#define AUDIO_WORK_DB_VIEW_HPP

#include <QWidget>
#include <QModelIndex>
#include <QItemSelection>

class AudioWorkTableModel;
class QTableView;
class QPushButton;

class AudioWorkDBView : public QWidget {
	Q_OBJECT
	public:
		AudioWorkDBView(AudioWorkTableModel * model, QWidget *parent = NULL);
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
		AudioWorkTableModel * mModel;
		QTableView * mTableView;
		QPushButton * mApplyFilterButton;
		QPushButton * mRemoveFilterButton;
};

#endif

