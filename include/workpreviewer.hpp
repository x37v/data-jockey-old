#ifndef WORK_PREVIEWER_HPP
#define WORK_PREVIEWER_HPP

#include <QObject>
#include <QThread>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

class MixerPanelModel;

class WorkPreviewerThread : public QThread {
	Q_OBJECT
	public:
		WorkPreviewerThread(QObject * parent);
	signals:
		void playing(bool playing);
	public slots:
		void playFile(QString file_location);
		void stop();
};

class WorkPreviewer : public QObject {
	Q_OBJECT
	public:
		WorkPreviewer(const QSqlDatabase &db, MixerPanelModel * mixerModel);
	signals:
		void previewing(bool p);
	public slots:
		void setWork(int work);
		void preview(bool p);
	private:
		WorkPreviewerThread mThread;
		int mWork;
		static QString cFileQueryString;
		QSqlQuery mFileQuery;
		MixerPanelModel * mMixerPanelModel;
};

#endif
