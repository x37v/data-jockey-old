#ifndef WORK_PREVIEWER_HPP
#define WORK_PREVIEWER_HPP

#include <QObject>
#include <QThread>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "soundfile.hpp"
#include "audioio.hpp"

class MixerPanelModel;
class QTimer;

class WorkPreviewerThread : public QThread {
	Q_OBJECT
	public:
		WorkPreviewerThread(DataJockey::AudioIO * audioIO, QObject * parent);
		virtual ~WorkPreviewerThread();
	signals:
		void playing(bool playing);
	public slots:
		void playFile(QString file_location);
		void stop();
	protected slots:
		void fillBuffer();
	protected:
		void run();
	private:
		size_t mPreviewFrames;
		jack_default_audio_sample_t * mPreviewBuffer;
		DataJockey::AudioIO * mAudioIO;
		QTimer * mFillTimer;
		SoundFile * mSoundFile;
};

class WorkPreviewer : public QObject {
	Q_OBJECT
	public:
		WorkPreviewer(const QSqlDatabase &db, MixerPanelModel * mixerModel, DataJockey::AudioIO * audioIO);
	signals:
		void previewing(bool p);
		void playingFile(QString file_location);
		void stopping();
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
