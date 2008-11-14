#include "workpreviewer.hpp"
#include "mixerpanelmodel.hpp"
#include <QSqlRecord>
#include <QVariant>

#include <iostream>

WorkPreviewerThread::WorkPreviewerThread(QObject * parent) : QThread(parent){
}

void WorkPreviewerThread::playFile(QString file){
	std::cout << "previewing: " << file.toStdString() << std::endl;
}

void WorkPreviewerThread::stop(){
}

QString WorkPreviewer::cFileQueryString(
	"select audio_files.location audio_file\n"
	"from audio_works\n"
	"\tjoin audio_files on audio_files.id = audio_works.audio_file_id\n"
	"where audio_works.id = ");

WorkPreviewer::WorkPreviewer(const QSqlDatabase &db, MixerPanelModel * mixerModel) :
	mThread(this), mFileQuery("", db) 
{
	mWork = -1;
	mMixerPanelModel = mixerModel;

	//connect internal signals/slots
	QObject::connect(&mThread,
			SIGNAL(playing(bool)),
			this,
			SLOT(preview(bool)));
}

void WorkPreviewer::setWork(int work){
	mWork = work;
	emit(previewing(false));
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
			mThread.playFile(mFileQuery.value(audioFileCol).toString());
		} else {
			emit(previewing(false));
		}
	} else
		emit(previewing(false));
}

