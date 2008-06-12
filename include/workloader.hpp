#ifndef WORK_LOADER
#define WORK_LOADER

#include <QThread>
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QSqlQuery>
#include "buffer.hpp"

class MixerPanelModel;
class MixerPanelView;

class BufferLoaderThread : public QThread {
	Q_OBJECT
	public:
		BufferLoaderThread(unsigned int index, QString audiobufloc, QString beatbufloc, QObject * parent = NULL);
		void run();
	private:
		unsigned int mIndex;
		QString mAudioBufLoc;
		QString mBeatBufLoc;
	signals:
		void buffersLoaded(unsigned int index, DataJockey::AudioBufferPtr audio_buffer, DataJockey::BeatBufferPtr beat_buffer);
};

class WorkLoader : public QObject {
	Q_OBJECT
	public:
		WorkLoader(const QSqlDatabase & db, MixerPanelModel * mixerModel, MixerPanelView * mixerView);
	public slots:
		void selectWork(int work);
		void loadWork(unsigned int mixer);
	signals:
		void mixerLoad(unsigned int mixer, DataJockey::AudioBufferPtr audiobuf, 
				DataJockey::BeatBufferPtr beatbuf, bool wait_for_measure = false);
	protected slots:
		void workLoaded(unsigned int index, 
				DataJockey::AudioBufferPtr audio_buffer, 
				DataJockey::BeatBufferPtr beat_buffer);
	private:
		static bool cTypesRegistered;
		unsigned int mNumMixers;
		static QString cFileQueryString;
		static QString cWorkInfoQueryString;
		int mWork;
		MixerPanelView * mMixerPanelView;
		QSqlQuery mFileQuery;
		QSqlQuery mWorkInfoQuery;
};

#endif

