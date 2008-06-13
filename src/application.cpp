#include "application.hpp"

#include <iostream>
using std::cout;
using std::endl;

#define NUM_MIXERS 4

#include <QApplication>
#include <QWidget>
#include <QCleanlooksStyle>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QTableView>
#include <QSplitter>
#include <QErrorMessage>
#include <QTimer>

#include "audioworktablemodel.hpp"
#include "audioworkdbview.hpp"

#include "mixerchannelmodel.hpp"
#include "mixerchannelview.hpp"

#include "crossfadeview.hpp"
#include "crossfademodel.hpp"

#include "djmixerchannelview.hpp"
#include "djmixerchannelmodel.hpp"

#include "djmixercontrolview.hpp"
#include "djmixercontrolmodel.hpp"

#include "eqview.hpp"
#include "eqmodel.hpp"

#include "masterview.hpp"
#include "mastermodel.hpp"

#include "mixerpanelmodel.hpp"
#include "mixerpanelview.hpp"

#include "workdetailview.hpp"
#include "djmixerworkinfoview.hpp"

#include "audiodriver.hpp"
#include "workloader.hpp"

#include "audioio.hpp"


//for now we'll just have a gui app
int DataJockeyApplication::run(int argc, char *argv[]){
	QApplication app(argc, argv);
	app.setStyle(new QCleanlooksStyle);

	//open up the database
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("dj_development");
	if (!db.open()) {
		QMessageBox::critical(0, app.tr("Cannot open database"),
				app.tr("Unable to establish a database connection.\n"
					"Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}

	//our window
	QWidget * window = new QWidget;

	//models
	AudioWorkTableModel tableModel(db);
	MixerPanelModel * mixerPanelModel = new MixerPanelModel(NUM_MIXERS);

	//audio driver
	AudioDriver * audioDriver = new AudioDriver(mixerPanelModel);

	//views
	WorkDetailView * detailView = new WorkDetailView(db, window);
	MixerPanelView * mixerPanelView = new MixerPanelView(NUM_MIXERS, window);
	AudioWorkDBView * tableView = new AudioWorkDBView(&tableModel, window);

	//WorkLoader
	WorkLoader * workLoader = new WorkLoader(db, mixerPanelModel, mixerPanelView);

	//layouts
	QSplitter *splitter = new QSplitter(Qt::Vertical, window);
	QSplitter *splitter2 = new QSplitter(Qt::Horizontal, window);
	QVBoxLayout * layout = new QVBoxLayout(window);

	splitter->addWidget(mixerPanelView);
	splitter->addWidget(detailView);

	splitter2->addWidget(splitter);
	splitter2->addWidget(tableView);
	splitter2->setStretchFactor(0,0);
	splitter2->setStretchFactor(1,10);

	layout->addWidget(splitter2);
	layout->setContentsMargins(2,2,2,2);
	window->setLayout(layout);

	QObject::connect(tableView, SIGNAL(workSelected(int)),
			detailView, SLOT(setWork(int)));

	tableModel.setFiltered(true);
	tableModel.query();

	//connect up our signals
	connectMixerPanelModelView(mixerPanelModel, mixerPanelView);
	connectMixerPanelModelDriver(mixerPanelModel, audioDriver);

	//WorkLoader
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
			SIGNAL(mixerLoad(unsigned int, DataJockey::AudioBufferPtr, DataJockey::BeatBufferPtr, bool)),
			audioDriver, 
			SLOT(mixerLoad(unsigned int, DataJockey::AudioBufferPtr, DataJockey::BeatBufferPtr, bool)),
			Qt::QueuedConnection);

	window->setWindowTitle("data jockey");
	window->show();

	//set up the audio driver thread and start it
	AudioDriverThread * audioDriverThread = new AudioDriverThread(window);
	audioDriverThread->setAudioDriver(audioDriver);

	//start the driver and the driver thread
	audioDriver->start();
	audioDriverThread->start();

	//XXX just for now, connect to the physical outputs
	audioDriver->audioIO()->connectToPhysical(0,0);
	audioDriver->audioIO()->connectToPhysical(1,1);

	//when the app is about to quit, stop our jack client and kill the driver thread
	QObject::connect(&app,
			SIGNAL(aboutToQuit()),
			audioDriverThread,
			SLOT(stop()));

	//make the warning messages graphical
	QErrorMessage::qtHandler();

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
				SLOT(setSync(bool)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(syncModeChanged(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(setSync(bool)));
		//progress
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(progressChanged(float)),
				djMixerView->DJMixerControl(),
				SLOT(setProgress(float)));
		//seek [just from view to model]
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(seekingFwd()),
				djMixerModel->DJMixerControl(),
				SLOT(seekFwd()));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(seekingBwd()),
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
				SIGNAL(resetClicked()),
				djMixerModel->DJMixerControl(),
				SLOT(resetWorkPosition()));
		//load
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(loadClicked()),
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
			SIGNAL(mixerSeeking(unsigned int, int)),
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

	//report progress
	QObject::connect(
			driver,
			SIGNAL(progressChanged(unsigned int, float)),
			model,
			SLOT(mixerUpdateProgress(unsigned int, float)),
			Qt::QueuedConnection);

	//master
	QObject::connect(
			model->master(),
			SIGNAL(volumeChanged(float)),
			driver,
			SLOT(masterSetVolume(float)),
			Qt::QueuedConnection);
	QObject::connect(
			model->master(),
			SIGNAL(tempoChanged(float)),
			driver,
			SLOT(masterSetTempo(float)),
			Qt::QueuedConnection);
	QObject::connect(
			model->master(),
			SIGNAL(tempoMulChanged(float)),
			driver,
			SLOT(masterSetTempoMul(float)),
			Qt::QueuedConnection);
	QObject::connect(
			model->master(),
			SIGNAL(syncSourceChanged(unsigned int)),
			driver,
			SLOT(masterSetSyncSrc(unsigned int)),
			Qt::QueuedConnection);
	//report tempo
	QObject::connect(
			driver,
			SIGNAL(tempoChanged(float)),
			model->master(),
			SLOT(setTempo(float)),
			Qt::QueuedConnection);
}

AudioDriverThread::AudioDriverThread(QObject * parent) :
	QThread(parent)
{
}

void AudioDriverThread::setAudioDriver(AudioDriver * driver){
	mDriver = driver;
	mDriver->moveToThread(this);
}

void AudioDriverThread::run(){
	if(mDriver){
		//start the driver if it hasn't been started already
		if(mDriver->audioIO()->getState() != JackCpp::AudioIO::active)
			mDriver->start();
		QTimer *timer = new QTimer(mDriver);
		connect(timer, SIGNAL(timeout()), mDriver, SLOT(processAudioEvents()));
		//every 5 ms
		timer->start(5);
	}
	exec();
}

void AudioDriverThread::stop(){
	if(mDriver){
		mDriver->stop();
	}
	quit();
}

