#ifndef APPLICATION_HPP
#define APPLICATION_HPP

class MixerPanelView;
class MixerPanelModel;
class AudioDriver;

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QThread>
#include <QMetaObject>
#include "buffer.hpp"

class WorkLoaderProxy;
class AudioDriverThread;


class DataJockeyApplication {
	public:
		static int run(int argc, char *argv[]);
		static void connectMixerPanelModelView(MixerPanelModel * model, MixerPanelView * view);
		static void connectMixerPanelModelDriver(MixerPanelModel * model, AudioDriver * driver);
};

class AudioDriverThread : public QThread {
	Q_OBJECT
	public:
		AudioDriverThread(QObject * parent = NULL);
		void setAudioDriver(AudioDriver * driver);
		void run();
	private:
		AudioDriver * mDriver;
};

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

class WorkLoaderProxy : public QObject {
	Q_OBJECT
	public:
		WorkLoaderProxy(const QSqlDatabase & db, MixerPanelModel * mixerModel, MixerPanelView * mixerView);
	public slots:
		void selectWork(int work);
		void loadWork(unsigned int mixer);
	signals:
		void mixerLoad(unsigned int mixer, QString audiobufloc, QString beatbufloc, bool wait_for_measure = false);
		void mixerLoad(unsigned int mixer, DataJockey::AudioBufferPtr audiobuf, 
				DataJockey::BeatBufferPtr beatbuf, bool wait_for_measure = false);
	protected slots:
		void workLoaded(unsigned int index, 
				DataJockey::AudioBufferPtr audio_buffer, 
				DataJockey::BeatBufferPtr beat_buffer);
	private:
		unsigned int mNumMixers;
		static QString cFileQueryString;
		static QString cWorkInfoQueryString;
		int mWork;
		MixerPanelView * mMixerPanelView;
		QSqlQuery mFileQuery;
		QSqlQuery mWorkInfoQuery;
};

#endif

