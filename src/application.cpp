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

#include "audioworkdbview.hpp"
#include "mixerpanelmodel.hpp"

#include "audioio.hpp"

#include "audiodriver.hpp"
#include "workloader.hpp"

#include "applicationmodel.hpp"
#include "applicationview.hpp"

#include "tageditor.hpp"
#include "workfilterlist.hpp"
#include "defaultworkfilters.hpp"

//for now we'll just have a gui app
int DataJockeyApplication::run(int argc, char *argv[]){
	QApplication app(argc, argv);
	app.setStyle(new QCleanlooksStyle);

	try {
		ApplicationModel::setNumberOfMixers(NUM_MIXERS);
		ApplicationModel::setDataBase("QMYSQL", "dj_development");
	} catch (std::exception& e) {
		QString text(app.tr(e.what()));
		text.append("\nClick Cancel to exit.");
		QMessageBox::critical(0, app.tr("Cannot open database"), text , QMessageBox::Cancel);
		return false;
	}

	ApplicationModel * model = ApplicationModel::instance();
	ApplicationView * view = new ApplicationView(model);
	WorkLoader * loader = new WorkLoader(model->db(), model->mixerPanel(), view->mixerPanel());
	AudioDriver * audioDriver = new AudioDriver(model->mixerPanel());
	AudioDriverThread * audioDriverThread = new AudioDriverThread(model);

	audioDriverThread->setAudioDriver(audioDriver);

	//add our default filters
	TagSelectionFilter * tagFilter = new TagSelectionFilter(model, model);
	TempoRangeFilter * tempoFilter = new TempoRangeFilter(model, model);
	model->workFilterList()->addFilter(tagFilter);
	model->workFilterList()->addFilter(tempoFilter);
	//connect the tag editor's selection to the tagFilter
	QObject::connect(
			view->tagEditor(), SIGNAL(tagSelectionChanged(QList<int>)),
			tagFilter, SLOT(setTags(QList<int>)));

	//make connections ********
	//WorkLoader
	QObject::connect(
			view->workDB(),
			SIGNAL(workSelected(int)),
			loader,
			SLOT(selectWork(int)));
	QObject::connect(
			model->mixerPanel(),
			SIGNAL(mixerLoading(unsigned int, int)),
			loader,
			SLOT(mixerLoadWork(unsigned int, int)));
	QObject::connect(
			loader,
			SIGNAL(mixerLoaded(unsigned int, DataJockey::AudioBufferPtr, DataJockey::BeatBufferPtr, bool)),
			audioDriver, 
			SLOT(mixerLoad(unsigned int, DataJockey::AudioBufferPtr, DataJockey::BeatBufferPtr, bool)),
			Qt::QueuedConnection);

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

	view->show();
	return app.exec();
}

