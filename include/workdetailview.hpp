#ifndef WORK_DETAIL_VIEW_HPP
#define WORK_DETAIL_VIEW_HPP

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

class QLineEdit;
class QGridLayout;
class TagView;
class WorkTagModelFilter;
class TagModel;

class WorkDetailView : public QWidget {
	Q_OBJECT
	public:
		WorkDetailView(
				TagModel * tagModel,
				const QSqlDatabase & db = QSqlDatabase(),
				QWidget * parent = NULL);
		virtual void keyPressEvent ( QKeyEvent * event );
	public slots:
		void setWork(int work_id);
		void clear();
		void expandTags(bool expand = true);
	private:
		static const QString cWorkQuery;
		QGridLayout * mLayout;
		QLineEdit * mArtist;
		QLineEdit * mTitle;
		TagView * mTagView;

		WorkTagModelFilter * mTagModel;

		QSqlQuery mWorkQuery;
};

#endif

