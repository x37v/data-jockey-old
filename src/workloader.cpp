#include "workloader.hpp"
#include "mixerpanelmodel.hpp"
#include "mixerpanelview.hpp"
#include "djmixerchannelview.hpp"
#include "djmixerworkinfoview.hpp"
#include <QString>
#include <QSqlRecord>
#include <QVariant>
#include <QMetaObject>

BufferLoaderThread::BufferLoaderThread(unsigned int index, QString audiobufloc, QString beatbufloc, QObject * parent) :
	QThread(parent)
{
	mIndex = index;
	mAudioBufLoc = audiobufloc;
	mBeatBufLoc = beatbufloc;
}

void BufferLoaderThread::run(){
	DataJockey::BeatBufferPtr beat_buffer = new DataJockey::BeatBuffer(mBeatBufLoc.toStdString());
	DataJockey::AudioBufferPtr audio_file = new DataJockey::AudioBuffer(mAudioBufLoc.toStdString());
	emit(buffersLoaded(mIndex, audio_file, beat_buffer));
}

bool WorkLoader::cTypesRegistered = false;

QString WorkLoader::cFileQueryString(
	"select audio_files.location audio_file, annotation_files.location beat_file\n"
	"from audio_works\n"
	"\tjoin audio_files on audio_files.id = audio_works.audio_file_id\n"
	"\tjoin annotation_files on annotation_files.audio_work_id = audio_works.id\n"
	"where audio_works.id = ");

QString WorkLoader::cWorkInfoQueryString(
		"select audio_works.name title,\n"
		"\tartists.name\n"
		"artist from audio_works"
		"\tinner join artist_audio_works on artist_audio_works.audio_work_id = audio_works.id\n"
		"\tinner join artists on artists.id = artist_audio_works.artist_id\n"
		"where audio_works.id = ");

WorkLoader::WorkLoader(const QSqlDatabase & db, MixerPanelModel * model, MixerPanelView * mixerView) :
	QObject(model), mFileQuery("", db), mWorkInfoQuery("",db)
{
	mWork = -1;
	mMixerPanelView = mixerView;
	mNumMixers = model->mixerChannels()->size();
	if(!cTypesRegistered){
		qRegisterMetaType<DataJockey::AudioBufferPtr>("DataJockey::AudioBufferPtr");
		qRegisterMetaType<DataJockey::BeatBufferPtr>("DataJockey::BeatBufferPtr");
		cTypesRegistered = true;
	}
}

void WorkLoader::selectWork(int work){
	mWork = work;
}

void WorkLoader::loadWork(unsigned int mixer){
	if(mWork >= 0 && mixer < mNumMixers){
		//build up query
		QString fileQueryStr(cFileQueryString);
		QString workQueryStr(cWorkInfoQueryString);
		QString id;
		id.setNum(mWork);
		fileQueryStr.append(id);
		workQueryStr.append(id);
		//execute
		mFileQuery.exec(fileQueryStr);
		QSqlRecord rec = mFileQuery.record();
		int audioFileCol = rec.indexOf("audio_file");
		int beatFileCol = rec.indexOf("beat_file");
		//if we can grab it
		if(mFileQuery.first()){
			QString audiobufloc = mFileQuery.value(audioFileCol).toString();
			QString beatbufloc = mFileQuery.value(beatFileCol).toString();

			//use a thread to load the stuff!
			BufferLoaderThread * loaderThread = new BufferLoaderThread(mixer, audiobufloc, beatbufloc, this);
			QObject::connect(loaderThread,
					SIGNAL(buffersLoaded(unsigned int, DataJockey::AudioBufferPtr, DataJockey::BeatBufferPtr)),
					this,
					SLOT(workLoaded(unsigned int, DataJockey::AudioBufferPtr, DataJockey::BeatBufferPtr)),
					Qt::QueuedConnection);
			emit(mixerLoad(mixer, NULL, NULL));
			loaderThread->start();
			
			mWorkInfoQuery.exec(workQueryStr);
			if(mWorkInfoQuery.first()){
				rec = mWorkInfoQuery.record();
				int titleCol = rec.indexOf("title");
				int artistCol = rec.indexOf("artist");
				mMixerPanelView->mixerChannels()->at(mixer)->DJMixerWorkInfo()->setArtistText(
						mWorkInfoQuery.value(artistCol).toString()
						);
				mMixerPanelView->mixerChannels()->at(mixer)->DJMixerWorkInfo()->setTitleText(
						mWorkInfoQuery.value(titleCol).toString()
						);
			}
		} else {
			//XXX ERROR
		}
	} else {
		//XXX no work selected
	}
}


void WorkLoader::workLoaded(unsigned int index, 
		DataJockey::AudioBufferPtr audio_buffer, 
		DataJockey::BeatBufferPtr beat_buffer){
	emit(mixerLoad(index, audio_buffer, beat_buffer));
}
