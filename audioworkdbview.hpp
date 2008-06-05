#ifndef AUDIO_WORK_DB_VIEW_HPP
#define AUDIO_WORK_DB_VIEW_HPP

#include <QWidget>

class AudioWorkTableModel;
class QTableView;
class QPushButton;

class AudioWorkDBView : public QWidget {
	Q_OBJECT
	public:
		AudioWorkDBView(AudioWorkTableModel * model, QWidget *parent = NULL);
	private:
		QTableView * mTableView;
		QPushButton * mApplyFilterButton;
		QPushButton * mRemoveFilterButton;
};

#endif

