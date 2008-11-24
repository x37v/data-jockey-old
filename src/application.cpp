#include "application.hpp"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

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

#include "workdbview.hpp"
#include "mixerpanelmodel.hpp"

#include "audioio.hpp"

#include "audiodriver.hpp"
#include "workloader.hpp"
#include "workpreviewer.hpp"
#include "workdetailview.hpp"

#include "applicationmodel.hpp"
#include "applicationview.hpp"

#include "tageditor.hpp"
#include "workfilterlist.hpp"
#include "defaultworkfilters.hpp"


#include "oscreceiver.hpp"

#include "config.hpp"

#include <QFile>

//for now we'll just have a gui app
int DataJockeyApplication::run(int argc, char *argv[]){

	QApplication app(argc, argv);
	app.setStyle(new QCleanlooksStyle);

	QFile file(":/style.qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
	app.setStyleSheet(styleSheet);

	datajockey::Configuration * config = datajockey::Configuration::instance();

	//parse command line arguments
	try {
		// Declare the supported options.
		po::options_description desc("Data Jockey options");
		po::variables_map vm;        
		po::positional_options_description p;

		desc.add_options()
			("help,h", "Produce this help message.")
			("config,c", po::value<std::string>(), "Specify a configuration file to use.")
			;

		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm);    

		if (vm.count("help")) {
			cout << desc << endl;
			return 0;
		}

		//load the config file if specified
		//otherwise we need to search for it!
		try {
			if (vm.count("config"))
				config->loadFile(vm["config"].as<std::string>());
			else 
				config->loadDefault();
		} catch(std::exception& e) {
			std::string str("Error loading config file:");
			str.append("\n");
			str.append(e.what());
			str.append("\n");
			str.append("You can specify a config file location with the -c switch");
			qFatal(str.c_str());
			return app.exec();
		}

	} catch(std::exception& e) {
		std::string str("Error in command line arguments: ");
		str.append(e.what());
		qFatal(str.c_str());
		return app.exec();
	}

	try {
		ApplicationModel::setNumberOfMixers(NUM_MIXERS);
		ApplicationModel::setDataBase(
				config->databaseAdapter(),
				config->databaseName(),
				config->databaseUserName(),
				config->databasePassword()
				);
	} catch (std::exception& e) {
		QString text(app.tr(e.what()));
		text.append("\nMake sure the entries are correct in your config file"
			"\nClick Cancel to exit.");
		QMessageBox::critical(0, app.tr("Cannot open database"), text , QMessageBox::Cancel);
		return false;
	}

	ApplicationModel * model = ApplicationModel::instance();
	ApplicationView * view = new ApplicationView(model);
	WorkLoader * loader = new WorkLoader(model->db(), model->mixerPanel(), view->mixerPanel());
	AudioDriver * audioDriver = new AudioDriver(model->mixerPanel());
	WorkPreviewer * previewer = new WorkPreviewer(model->db(), model->mixerPanel(), audioDriver->audioIO());
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

	//previewing
	QObject::connect(
			view->workDB(),
			SIGNAL(workSelected(int)),
			previewer,
			SLOT(setWork(int)));
	QObject::connect(
			previewer,
			SIGNAL(previewing(bool)),
			view->workDetail(),
			SLOT(setPreviewing(bool)));
	QObject::connect(
			view->workDetail(),
			SIGNAL(previewing(bool)),
			previewer,
			SLOT(preview(bool)));

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
	RubyInterpreterThread * rubyThread = new RubyInterpreterThread;
	rubyThread->start();

	OscThread * oscThread = new OscThread(model->oscReceiver(), 10001);
	oscThread->start();

	view->show();
	return app.exec();
}

#include "ip/UdpSocket.h"

OscThread::OscThread(OscReceiver * receiver, unsigned int port) {
	mOscReceiver = receiver;
	mPort = port;
}

void OscThread::run(){
	UdpListeningReceiveSocket s(
			IpEndpointName( IpEndpointName::ANY_ADDRESS, mPort ),
			mOscReceiver );
	s.Run();
}

#include "ruby.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void RubyInterpreterThread::run(){
	struct stat buf;
	ruby_init();
	ruby_init_loadpath();

	//if we are running from the build dir then just run the file from there
	if(!stat("ruby/datajockey/interpreter.rb", &buf) && S_ISREG(buf.st_mode)){
		if(!stat("swig/", &buf) && S_ISDIR(buf.st_mode))
			rb_eval_string("$: << 'swig'\n");
		if(!stat("utils-swig/", &buf) && S_ISDIR(buf.st_mode))
			rb_eval_string("$: << 'utils-swig'\n");
		rb_eval_string(
				"$: << 'ruby'\n"
				"load 'ruby/datajockey/interpreter.rb'");
	} else {
		rb_eval_string(
				"begin; require 'datajockey/interpreter'\n"
				"rescue LoadError; puts 'cannot load interpreter'\n"
				"end");
	}
}

