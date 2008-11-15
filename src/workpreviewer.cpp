#include "workpreviewer.hpp"
#include "mixerpanelmodel.hpp"
#include <QSqlRecord>
#include <QVariant>
#include <QTimer>

#define MIN(x,y) ((x) < (y) ? (x) : (y))

WorkPreviewerThread::WorkPreviewerThread(DataJockey::AudioIO * audioIO, QObject * parent) : QThread(parent){
	mAudioIO = audioIO;
	mPreviewFrames = audioIO->previewFrames();
	mPreviewBuffer = new jack_default_audio_sample_t[mPreviewFrames * 2];
	mSoundFile = NULL;
	mFillTimer = NULL;
}

WorkPreviewerThread::~WorkPreviewerThread(){
	delete [] mPreviewBuffer;
	if(mFillTimer){
		mFillTimer->stop();
		delete mFillTimer;
	}
	if(mSoundFile)
		delete mSoundFile;
}

void WorkPreviewerThread::playFile(QString file){
	if(mFillTimer == NULL){
		mFillTimer = new QTimer(this);
		//connect us up
		QObject::connect(mFillTimer,
				SIGNAL(timeout()),
				this,
				SLOT(fillBuffer()));
	}

	mFillTimer->stop();
	try {
		if(mSoundFile)
			delete mSoundFile;
		mSoundFile = new SoundFile(file.toStdString());
		if(*mSoundFile){
			//run every millisecond
			mFillTimer->start(1);
		} else {
			delete mSoundFile;
			mSoundFile = NULL;
			emit(playing(false));
		}
	} catch (...) {
		mSoundFile = NULL;
	}
}

void WorkPreviewerThread::stop(){
	if(mFillTimer)
		mFillTimer->stop();
}

void WorkPreviewerThread::run(){
	exec();
}

void WorkPreviewerThread::fillBuffer(){
	if(mSoundFile && mAudioIO->previewFramesFree()){
		//mPreviewBuffer is the length of the audioIO's preview buffer, so we will never overstep
		unsigned int framesRead = mSoundFile->readf(mPreviewBuffer, MIN(mPreviewFrames, mAudioIO->previewFramesFree()));
		if(framesRead > 0){
			mAudioIO->queuePreviewFrames(mPreviewBuffer, framesRead);
		} else {
			//if we haven't read any frames then we stop
			mFillTimer->stop();
			emit(playing(false));
		}
	}
}

QString WorkPreviewer::cFileQueryString(
	"select audio_files.location audio_file\n"
	"from audio_works\n"
	"\tjoin audio_files on audio_files.id = audio_works.audio_file_id\n"
	"where audio_works.id = ");

WorkPreviewer::WorkPreviewer(const QSqlDatabase &db, MixerPanelModel * mixerModel, DataJockey::AudioIO * audioIO) :
	mThread(audioIO, this), mFileQuery("", db) 
{
	mWork = -1;
	mMixerPanelModel = mixerModel;
	mThread.start();

	//connect internal signals/slots
	QObject::connect(&mThread,
			SIGNAL(playing(bool)),
			this,
			SLOT(preview(bool)));
	QObject::connect(this,
			SIGNAL(playingFile(QString)),
			&mThread,
			SLOT(playFile(QString)));
	QObject::connect(this,
			SIGNAL(stopping()),
			&mThread,
			SLOT(stop()));
}

void WorkPreviewer::setWork(int work){
	mWork = work;
	emit(previewing(false));
	emit(stopping());
}

void WorkPreviewer::preview(bool p){
	if(p && (mWork > 0)){
		//build up query
		QString fileQueryStr(cFileQueryString);
		QString id;
		id.setNum(mWork);
		fileQueryStr.append(id);
		//execute
		mFileQuery.exec(fileQueryStr);
		QSqlRecord rec = mFileQuery.record();
		int audioFileCol = rec.indexOf("audio_file");

		if(mFileQuery.first()){
			//emit the signal so it is queued up in the thread..
			emit(playingFile(mFileQuery.value(audioFileCol).toString()));
		} else {
			emit(previewing(false));
		}
	} else {
		emit(stopping());
		emit(previewing(false));
	}
}

