#include "application.hpp"

#include "mixerpanelview.hpp"
#include <QApplication>
#include <QWidget>
#include <QCleanlooksStyle>

#include <QHBoxLayout>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QMessageBox>
#include <QTableView>

#include <QSplitter>

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include <QSignalMapper>

#include "crossfadeview.hpp"
#include "audioworktablemodel.hpp"
#include "audioworkdbview.hpp"

#include "mixerchannelmodel.hpp"
#include "mixerchannelview.hpp"

#include "djmixerchannelview.hpp"
#include "djmixerchannelmodel.hpp"

#include "djmixercontrolview.hpp"
#include "djmixercontrolmodel.hpp"

#include "eqview.hpp"
#include "eqmodel.hpp"

#include "masterview.hpp"
#include "mastermodel.hpp"

#include "mixerpanelmodel.hpp"

#include "workdetailview.hpp"

#include "audiodriver.hpp"

#define NUM_MIXERS 4

//for now we'll just have a gui app
int DataJockeyApplication::run(int argc, char *argv[]){
	QApplication app(argc, argv);
	app.setStyle(new QCleanlooksStyle);

	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("dj_development");
	if (!db.open()) {
		QMessageBox::critical(0, app.tr("Cannot open database"),
				app.tr("Unable to establish a database connection.\n"
					"Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}

	QWidget * window = new QWidget;

	//models
	AudioWorkTableModel tableModel(db);
	MixerPanelModel * mixerPanelModel = new MixerPanelModel(NUM_MIXERS);

	//audio driver
	AudioDriver * audioDriver = new AudioDriver(mixerPanelModel);

	//WorkLoaderProxy
	WorkLoaderProxy * workLoader = new WorkLoaderProxy(db, mixerPanelModel);

	//views
	WorkDetailView * detailView = new WorkDetailView(db, window);
	MixerPanelView * mixerPanelView = new MixerPanelView(NUM_MIXERS, window);
	AudioWorkDBView * tableView = new AudioWorkDBView(&tableModel, window);

	window->setWindowTitle("floatme");


	QSplitter *splitter = new QSplitter(Qt::Vertical, window);
	QSplitter *splitter2 = new QSplitter(Qt::Horizontal, window);

	splitter->addWidget(mixerPanelView);
	splitter->addWidget(detailView);
	splitter2->addWidget(splitter);
	splitter2->addWidget(tableView);
	QVBoxLayout * layout = new QVBoxLayout(window);
	splitter2->setStretchFactor(0,0);
	splitter2->setStretchFactor(1,10);
	layout->addWidget(splitter2);
	layout->setContentsMargins(2,2,2,2);
	window->setLayout(layout);

	QObject::connect(tableView, SIGNAL(workSelected(int)),
			detailView, SLOT(setWork(int)));

	tableModel.setFiltered(true);
	tableModel.query();


	//layout->addWidget(splitter,0,0);
	//layout->addWidget(tableView,0,1);
	//layout->addWidget(mixerPannel, 0, 0);
	//layout->addWidget(tableView, 0, 1, 3, 1);
	//layout->addWidget(splitter, 1, 0);
	//layout->addWidget(detailView, 2, 0);


	//layout->setColumnStretch(1,10);
	//layout->setColumnStretch(0,0);
	//layout->setRowStretch(0,10);
	//layout->setRowStretch(1,0);
	

	//connect up our signals
	connectMixerPanelModelView(mixerPanelModel, mixerPanelView);
	connectMixerPanelModelDriver(mixerPanelModel, audioDriver);

	//WorkLoaderProxy
	QObject::connect(
			tableView,
			SIGNAL(workSelected(int)),
			workLoader,
			SLOT(selectWork(int)));
	QObject::connect(
			mixerPanelModel,
			SIGNAL(mixerLoad(unsigned int)),
			workLoader,
			SLOT(loadWork(unsigned int)));
	QObject::connect(
			workLoader,
			SIGNAL(mixerLoad(unsigned int, QString, QString, bool)),
			audioDriver, 
			SLOT(mixerLoad(unsigned int, QString, QString, bool)),
			Qt::QueuedConnection);


	/*
	QObject::connect(
			(*mixerPannel->mixerChannels())[0]->DJMixerControl(),
			SIGNAL(pausedChanged(bool)),
			djModel, SLOT(setPaused(bool)));
	QObject::connect(
			(*mixerPannel->mixerChannels())[0]->DJMixerControl(),
			SIGNAL(syncModeChanged(bool)),
			djModel, SLOT(setRunFree(bool)));
	QObject::connect(
			djModel,
			SIGNAL(progressChanged(float)),
			(*mixerPannel->mixerChannels())[0]->DJMixerControl(),
			SLOT(setProgress(float)));
	djModel->setProgress(0.324);

	EQView * eqView = mixerChan->eq();
	EQModel * eqModel = new EQModel();

	QObject::connect(
			eqView,
			SIGNAL(highValueChanged(float)),
			eqModel, SLOT(setHigh(float)));
	QObject::connect(
			eqModel,
			SIGNAL(highChanged(float)),
			eqView, SLOT(setHigh(float)));

	QObject::connect(
			eqView,
			SIGNAL(midValueChanged(float)),
			eqModel, SLOT(setMid(float)));
	QObject::connect(
			eqModel,
			SIGNAL(midChanged(float)),
			eqView, SLOT(setMid(float)));

	QObject::connect(
			eqView,
			SIGNAL(lowValueChanged(float)),
			eqModel, SLOT(setLow(float)));
	QObject::connect(
			eqModel,
			SIGNAL(lowChanged(float)),
			eqView, SLOT(setLow(float)));

	QObject::connect(
			mixerChan,
			SIGNAL(volumeChanged(float)),
			mixerModel, SLOT(setVolume(float)));

	QObject::connect(
			mixerModel,
			SIGNAL(volumeChanged(float)),
			mixerChan,
			SLOT(setVolume(float)));

	QObject::connect(
			mixerChan,
			SIGNAL(mutedChanged(bool)),
			mixerModel,
			SLOT(setMuted(bool)));
	QObject::connect(
			mixerModel,
			SIGNAL(mutedChanged(bool)),
			mixerChan,
			SLOT(setMuted(bool)));
	*/

	/*
	 //just to test cuts
	QObject::connect(
			(*mixerPannel->mixerChannels())[0]->DJMixerControl(),
			SIGNAL(syncModeChanged(bool)),
			eqModel, SLOT(toggleLowCut()));

	QObject::connect(
			eqModel, SIGNAL(lowCutChanged(bool)),
			(*mixerPannel->mixerChannels())[0]->DJMixerControl(),
			SLOT(setRunningFree(bool)));
			*/
	/*
	MasterView * master = mixerPannelView->master();
	MasterModel * masterModel = mixerPannelModel->master();
	QObject::connect(master, SIGNAL(syncSourceChanged(unsigned int)),
			masterModel, SLOT(setSyncSource(unsigned int)));
	QObject::connect(masterModel, SIGNAL(syncSourceChanged(unsigned int)),
			master, SLOT(setSyncSource(unsigned int)));
		*/

	window->show();
	audioDriver->start();
	return app.exec();
}

void DataJockeyApplication::connectMixerPanelModelView(MixerPanelModel * model, MixerPanelView * view){
	for(unsigned int i = 0; i < model->mixerChannels()->size(); i++){
		DJMixerChannelModel * djMixerModel = model->mixerChannels()->at(i);
		DJMixerChannelView * djMixerView = view->mixerChannels()->at(i);

		//****** mixer signals
		//volume
		QObject::connect(
				djMixerModel->mixerChannel(),
				SIGNAL(volumeChanged(float)),
				djMixerView->mixerChannel(),
				SLOT(setVolume(float)));
		QObject::connect(
				djMixerView->mixerChannel(),
				SIGNAL(volumeChanged(float)),
				djMixerModel->mixerChannel(),
				SLOT(setVolume(float)));
		//mute
		QObject::connect(
				djMixerModel->mixerChannel(),
				SIGNAL(mutedChanged(bool)),
				djMixerView->mixerChannel(),
				SLOT(setMuted(bool)));
		QObject::connect(
				djMixerView->mixerChannel(),
				SIGNAL(mutedChanged(bool)),
				djMixerModel->mixerChannel(),
				SLOT(setMuted(bool)));
		//eq
		EQModel * eqModel = djMixerModel->mixerChannel()->eq();
		EQView * eqView = djMixerView->mixerChannel()->eq();
		QObject::connect(
				eqModel,
				SIGNAL(highChanged(float)),
				eqView,
				SLOT(setHigh(float)));
		QObject::connect(
				eqModel,
				SIGNAL(midChanged(float)),
				eqView,
				SLOT(setMid(float)));
		QObject::connect(
				eqModel,
				SIGNAL(lowChanged(float)),
				eqView,
				SLOT(setLow(float)));
		QObject::connect(
				eqView,
				SIGNAL(highChanged(float)),
				eqModel,
				SLOT(setHigh(float)));
		QObject::connect(
				eqView,
				SIGNAL(midChanged(float)),
				eqModel,
				SLOT(setMid(float)));
		QObject::connect(
				eqView,
				SIGNAL(lowChanged(float)),
				eqModel,
				SLOT(setLow(float)));

		//cue
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(cueModeChanged(bool)),
				djMixerView->DJMixerControl(),
				SLOT(setCueing(bool)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(cueModeChanged(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(setCueing(bool)));
		//pause
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(pausedChanged(bool)),
				djMixerView->DJMixerControl(),
				SLOT(setPaused(bool)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(pausedChanged(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(setPaused(bool)));
		//sync mode
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(syncModeChanged(bool)),
				djMixerView->DJMixerControl(),
				SLOT(setRunFree(bool)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(syncModeChanged(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(setRunFree(bool)));
		//progress
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(progressChanged(float)),
				djMixerView->DJMixerControl(),
				SLOT(setProgress(float)));
		//seek [just from view to model]
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(seekFwdClicked(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(seekFwd()));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(seekBwdClicked(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(seekBkwd()));
		//beat offset
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(beatOffsetChanged(int)),
				djMixerView->DJMixerControl(),
				SLOT(setBeatOffset(int)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(beatOffsetChanged(int)),
				djMixerModel->DJMixerControl(),
				SLOT(setBeatOffset(int)));
		//reset 
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(resetClicked(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(reset()));
		//load
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(loadClicked(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(loadWork()));
	}

	//master
	//volume
	QObject::connect(
			model->master(),
			SIGNAL(volumeChanged(float)),
			view->master(),
			SLOT(setVolume(float)));
	QObject::connect(
			view->master(),
			SIGNAL(volumeChanged(float)),
			model->master(),
			SLOT(setVolume(float)));
	//tempo
	QObject::connect(
			model->master(),
			SIGNAL(tempoChanged(float)),
			view->master(),
			SLOT(setTempo(float)));
	QObject::connect(
			view->master(),
			SIGNAL(tempoChanged(float)),
			model->master(),
			SLOT(setTempo(float)));
	//tempo mul
	QObject::connect(
			model->master(),
			SIGNAL(tempoMulChanged(float)),
			view->master(),
			SLOT(setTempoMul(float)));
	QObject::connect(
			view->master(),
			SIGNAL(tempoMulChanged(float)),
			model->master(),
			SLOT(setTempoMul(float)));
	//sync src
	QObject::connect(
			model->master(),
			SIGNAL(syncSourceChanged(unsigned int)),
			view->master(),
			SLOT(setSyncSource(unsigned int)));
	QObject::connect(
			view->master(),
			SIGNAL(syncSourceChanged(unsigned int)),
			model->master(),
			SLOT(setSyncSource(unsigned int)));
}

//these are queued connections, as the driver runs in another thread.
void DataJockeyApplication::connectMixerPanelModelDriver(MixerPanelModel * model, AudioDriver * driver){
	//volume + mute
	QObject::connect(
			model,
			SIGNAL(mixerVolumeChanged(unsigned int, float)),
			driver,
			SLOT(mixerSetVolume(unsigned int, float)),
			Qt::QueuedConnection);
	//eq
	QObject::connect(
			model,
			SIGNAL(mixerEQValuesChanged(unsigned int, float, float, float)),
			driver,
			SLOT(mixerSetEQVals(unsigned int, float, float, float)),
			Qt::QueuedConnection);
	//cue mode
	QObject::connect(
			model,
			SIGNAL(mixerCueModeChanged(unsigned int, bool)),
			driver,
			SLOT(mixerSetCue(unsigned int, bool)),
			Qt::QueuedConnection);
	//pause
	QObject::connect(
			model,
			SIGNAL(mixerPausedChanged(unsigned int, bool)),
			driver,
			SLOT(mixerSetPause(unsigned int, bool)),
			Qt::QueuedConnection);
	//sync
	QObject::connect(
			model,
			SIGNAL(mixerSyncModeChanged(unsigned int, bool)),
			driver,
			SLOT(mixerSetSync(unsigned int, bool)),
			Qt::QueuedConnection);
	//seek
	QObject::connect(
			model,
			SIGNAL(mixerSeek(unsigned int, int)),
			driver,
			SLOT(mixerSeek(unsigned int, int)),
			Qt::QueuedConnection);
	//playback position
	QObject::connect(
			model,
			SIGNAL(mixerPlaybackPosChanged(unsigned int, int)),
			driver,
			SLOT(mixerSetPlaybackPosition(unsigned int, int)),
			Qt::QueuedConnection);
		
}

QString WorkLoaderProxy::cQueryString(
	"select audio_files.location audio_file, annotation_files.location beat_file\n"
	"from audio_works\n"
	"\tjoin audio_files on audio_files.id = audio_works.audio_file_id\n"
	"\tjoin annotation_files on annotation_files.audio_work_id = audio_works.id\n"
	"where audio_works.id = ");

WorkLoaderProxy::WorkLoaderProxy(const QSqlDatabase & db, QObject * parent) : 
	QObject(parent), mQuery("", db)
{
	mWork = -1;
}

void WorkLoaderProxy::selectWork(int work){
	mWork = work;
}

void WorkLoaderProxy::loadWork(unsigned int mixer){
	if(mWork >= 0){
		//build up query
		QString queryStr(cQueryString);
		QString id;
		id.setNum(mWork);
		queryStr.append(id);
		//execute
		mQuery.exec(queryStr);
		QSqlRecord rec = mQuery.record();
		int audioFileCol = rec.indexOf("audio_file");
		int beatFileCol = rec.indexOf("beat_file");
		//if we can grab it
		if(mQuery.first()){
			QString audiobufloc = mQuery.value(audioFileCol).toString();
			QString beatbufloc = mQuery.value(beatFileCol).toString();
			emit(mixerLoad(mixer, audiobufloc, beatbufloc));
		} else {
			//XXX ERROR
		}
	} else {
		//XXX no work selected
	}
}

