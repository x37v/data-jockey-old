#include <QApplication>
#include <QErrorMessage>
#include <QCleanlooksStyle>
#include <QWidget>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>

#include <boost/program_options.hpp>
#include <iostream>
#include <string>

#include "config.hpp"
#include "tagmodel.hpp"
#include "tageditor.hpp"
#include "workdetailview.hpp"
#include "worktablemodel.hpp"
#include "workdbview.hpp"

namespace po = boost::program_options;
using std::cout;
using std::endl;

#define UNDEFINED_RATING -100

int main(int argc, char *argv[]){

	QApplication app(argc, argv);
	datajockey::Configuration * config = datajockey::Configuration::instance();
	std::string inputFile;
	bool runGui = true;
	int rating = UNDEFINED_RATING;
	std::vector<std::string> tags;

	try {
		// Declare the supported options.
		po::options_description desc("Data Jockey Annotator options");
		po::variables_map vm;        
		po::positional_options_description p;

		desc.add_options()
			("help,h", "Produce this help message")
			("non-graphical,a", "Apply tags and rating then exit, don't show any GUI.")
			("config,c", po::value<std::string>(), "Specify the config file")
			("input-file,f", po::value<std::string>(), "The input file")
			("rating,r", po::value<int>(), "Apply a rating")
			("tag,t", po::value<std::vector<std::string> >(), 
			 "Apply the tag to this file."
			 "\nThe format is tag_name[,tag_class]"
			 "\nUse quotes to get multi word names or classes"
			 "\nIf the tag class is not found then the first tag with the name given will be applied"
			 "\nIf the tag does not exist the program will attempt to create it, the class must be provided in this case"
			 )
			;
		p.add("input-file", -1);

		po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
		po::notify(vm);    

		if (vm.count("help")) {
			cout << desc << endl;
			return 0;
		}

		//load the config file if specified
		//otherwise we need to search for it!
		if (vm.count("config"))
			config->loadFile(vm["config"].as<std::string>());
		else {
			try {
				config->loadDefault();
			} catch(std::exception& e) {
				cout << "Error loading config file:" << endl << e.what() << endl;
				cout << "You can specify a config file location with the -c switch" << endl;
				return 1;
			}
		}

		if (vm.count("non-graphical"))
			runGui = false;
		if (vm.count("rating")) 
			rating = vm["rating"].as<int>();
		if (vm.count("tag")) 
			tags = vm["tag"].as<std::vector<std::string> >();

		if (vm.count("input-file")) {
			inputFile = vm["input-file"].as<std::string>();
		} else {
			cout << "You must specify an input file" << endl;
			cout << desc << endl;
			return 1;
		}

	} catch(std::exception& e) {
		cout << "Error in command line arguments: " << e.what() << endl;
		return 1;
	}

	QSqlDatabase db;
	try {
		db = QSqlDatabase::addDatabase(config->databaseAdapter().c_str());
		db.setDatabaseName(config->databaseName().c_str());
		if(config->databaseUserName() != "")
			db.setUserName(config->databaseUserName().c_str());
		if(config->databasePassword() != "")
			db.setPassword(config->databasePassword().c_str());
	} catch(std::exception& e) {
		cout << "Invalid database configuration data, have you provided a correct configuration file?" << endl;
		return 1;
	}

	//QWidget w;
	//w.show();
	for(std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); it++)
		cout << *it << endl;

	if(runGui){
		QWidget * topWidget = new QWidget;
		topWidget->setWindowTitle("Data Jockey Annotator");
		QErrorMessage::qtHandler();
		app.setStyle(new QCleanlooksStyle);

		//make the warning messages graphical
		if(!db.open()){
			QString text(
					"\nCannot open database."
					"\nMake sure the entries are correct in your config file."
					"\nClick Cancel to exit.");
			QMessageBox::critical(0, app.tr("Cannot open database"), text, QMessageBox::Cancel);
			return 1;
		}

		QVBoxLayout * layout = new QVBoxLayout;
		QSplitter * vertSplit = new QSplitter(Qt::Vertical, topWidget);
		QSplitter * horiSplit = new QSplitter(Qt::Horizontal, topWidget);

		TagModel * tagModel = new TagModel(db);
		TagEditor * tagEditor = new TagEditor(tagModel);
		WorkTableModel * workTableModel = new WorkTableModel(db);
		WorkDetailView * workDetailView = new WorkDetailView(tagModel, db);
		WorkDBView * workDBView = new WorkDBView(workTableModel);

		horiSplit->addWidget(workDetailView);
		horiSplit->addWidget(tagEditor);
		vertSplit->addWidget(horiSplit);
		vertSplit->addWidget(workDBView);

		//connect stuff up!
		QObject::connect(workDBView,
				SIGNAL(workSelected(int)),
				workDetailView,
				SLOT(setWork(int)));

		layout->addWidget(vertSplit);
		topWidget->setLayout(layout);
		topWidget->show();

		return app.exec();
	} else {
		if(!db.open()){
			cout <<  "ERROR: cannot open database" << endl;
			return 1;
		}
		return 0;
	}
}
