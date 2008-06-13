#ifndef WORK_LOADER
#define WORK_LOADER

#include <QThread>
#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QSqlQuery>
#include "buffer.hpp"
#include <vector>

class MixerPanelModel;
class MixerPanelView;

class BufferLoaderThread : public QThread {
	Q_OBJECT
	public:
		BufferLoaderThread(QObject * parent);
		void start(unsigned int index, int work_id, QString audiobufloc, QString beatbufloc);
	protected:
		void run();
	private:
		unsigned int mIndex;
		int mWorkId;
		QString mAudioBufLoc;
		QString mBeatBufLoc;
	signals:
		void buffersLoaded(unsigned int index, int work_id, 
				DataJockey::AudioBufferPtr audio_buffer, DataJockey::BeatBufferPtr beat_buffer);
		void outOfMemory(unsigned int index, int work_id, QString audioFileLoc, QString beatFileLoc);
};

class WorkLoader : public QObject {
	Q_OBJECT
	public:
		WorkLoader(const QSqlDatabase & db, MixerPanelModel * mixerModel, MixerPanelView * mixerView);
	public slots:
		void selectWork(int work);
		void loadWork(unsigned int mixer);
	signals:
		void mixerLoaded(unsigned int mixer, DataJockey::AudioBufferPtr audiobuf, 
				DataJockey::BeatBufferPtr beatbuf, bool wait_for_measure = false);
	protected slots:
		void setWork(unsigned int mixer_index, 
				int work_id,
				DataJockey::AudioBufferPtr audio_buffer, 
				DataJockey::BeatBufferPtr beat_buffer);
		void outOfMemory(unsigned int index, int work_id, QString audioFileLoc, QString beatFileLoc);
	private:
		static bool cTypesRegistered;
		static QString cFileQueryString;
		static QString cWorkInfoQueryString;

		//the number of mixers in the model
		unsigned int mNumMixers;
		//the currently selected work
		int mWork;
		std::vector<BufferLoaderThread *> mLoaderThreads;
		MixerPanelView * mMixerPanelView;
		MixerPanelModel * mMixerPanelModel;
		QSqlQuery mFileQuery;
		QSqlQuery mWorkInfoQuery;
};

#endif

