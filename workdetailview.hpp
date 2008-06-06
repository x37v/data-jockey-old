#ifndef WORK_DETAIL_VIEW_HPP
#define WORK_DETAIL_VIEW_HPP

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

class QLineEdit;
class QGridLayout;
class TagView;

class WorkDetailView : public QWidget {
	Q_OBJECT
	public:
		WorkDetailView(
				const QSqlDatabase & db = QSqlDatabase(),
				QWidget * parent = NULL
				);
	public slots:
		void setWork(int work_id);
		void clear();
	private:
		static const QString cWorkQuery;
		QGridLayout * mLayout;
		QLineEdit * mArtist;
		QLineEdit * mTitle;
		TagView * mTagView;

		QSqlQuery mWorkQuery;
};

#endif

