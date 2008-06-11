#include "application.hpp"

#include "mixerpanelview.hpp"
#include <QApplication>
#include <QWidget>
#include <QCleanlooksStyle>

#include <QHBoxLayout>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QTableView>

#include <QSplitter>

#include <QHeaderView>
#include <QSortFilterProxyModel>

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

void DataJockeyApplication::connectMixerPanelModelDriver(MixerPanelModel * model, AudioDriver * driver){
}

