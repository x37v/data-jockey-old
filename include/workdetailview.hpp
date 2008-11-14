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
class QPushButton;

class WorkDetailView : public QWidget {
	Q_OBJECT
	public:
		WorkDetailView(
				TagModel * tagModel,
				const QSqlDatabase & db,
				QWidget * parent = NULL);
		virtual void keyPressEvent ( QKeyEvent * event );
	signals:
		void previewing(bool previewing);
	public slots:
		void setWork(int work_id);
		void clear();
		void expandTags(bool expand = true);
		void setPreviewing(bool down);
	protected slots:
		void setPreviewingInternal(bool down);
	private:
		static const QString cWorkQuery;
		QGridLayout * mLayout;
		QLineEdit * mArtist;
		QLineEdit * mTitle;
		TagView * mTagView;
		QPushButton * mPreviewButton;

		WorkTagModelFilter * mTagModel;
		bool mDisplayingWork;

		QSqlQuery mWorkQuery;
};

#endif

