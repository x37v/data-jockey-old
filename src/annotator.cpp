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
#include "workfiltermodel.hpp"
#include "workdbview.hpp"

namespace po = boost::program_options;
using std::cout;
using std::endl;

#define UNDEFINED_RATING -100

int main(int argc, char *argv[]){

	QApplication app(argc, argv);
	datajockey::Configuration * config = datajockey::Configuration::instance();
	std::string inputFile;
	int selectedWorkId = -1;
	bool runGui = true;
	int rating = UNDEFINED_RATING;
	std::vector<std::string> inputTags;
	QSqlDatabase db;

	//parse command line arguments
	try {
		// Declare the supported options.
		po::options_description desc("Data Jockey Annotator options");
		po::variables_map vm;        
		po::positional_options_description p;

		desc.add_options()
			("help,h", "Produce this help message.")
			("non-graphical,a", "Apply tags and/or rating then exit, don't show any GUI.")
			("config,c", po::value<std::string>(), "Specify a configuration file to use.")
			("input-file,f", po::value<std::string>(), "An input sound file to annotate.")
			("rating,r", po::value<int>(), "Apply a rating.")
			("tag,t", po::value<std::vector<std::string> >(), 
			 "Apply the tag to this file."
			 "\nThe format is tag_name[,tag_class]"
			 "\nUse quotes to get multi word names or classes."
			 "\nIf the tag class is not found then the first tag with the name given will be applied."
			 "\nIf the tag does not exist the program will attempt to create it, the class must be provided in this case."
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
				std::string str("Error loading config file:");
				str.append("\n");
				str.append(e.what());
				str.append("\n");
				str.append("You can specify a config file location with the -c switch");
				qFatal(str.c_str());
				return app.exec();
			}
		}

		if (vm.count("non-graphical"))
			runGui = false;
		if (vm.count("rating")) 
			rating = vm["rating"].as<int>();
		if (vm.count("tag")) 
			inputTags = vm["tag"].as<std::vector<std::string> >();

		//if we're not running graphically we must provide an input file
		if (vm.count("input-file")) {
			inputFile = vm["input-file"].as<std::string>();
		} else if(!runGui){
			cout << endl << "You must specify an input file!" << endl << endl;
			cout << desc << endl;
			return 1;
		}

	} catch(std::exception& e) {
		std::string str("Error in command line arguments: ");
		str.append(e.what());
		qFatal(str.c_str());
		return app.exec();
	}

	//open the database
	try {
		db = QSqlDatabase::addDatabase(config->databaseAdapter().c_str());
		db.setDatabaseName(config->databaseName().c_str());
		if(config->databaseUserName() != "")
			db.setUserName(config->databaseUserName().c_str());
		if(config->databasePassword() != "")
			db.setPassword(config->databasePassword().c_str());
	} catch(std::exception& e) {
		qFatal("Invalid database configuration data, have you provided a correct configuration file?");
		return app.exec();
	}

	if(!db.open()){
		qFatal("Cannot open database"
				"\nMake sure the entries are correct in your configuration file.");
		return app.exec();
	}

	//models
	TagModel * tagModel = new TagModel(db);
	WorkTableModel * workTableModel = new WorkTableModel(db);
	WorkFilterModelProxy * filteredWorkTableModel = new WorkFilterModelProxy(workTableModel);

	if(!inputFile.empty())
		selectedWorkId = workTableModel->findWorkByPath(inputFile);

	//if there is an input file and there are tags, deal with them
	if(!inputFile.empty() && !inputTags.empty()){
		for(std::vector<std::string>::iterator it = inputTags.begin(); it != inputTags.end(); it++){
			std::string tagName;
			size_t pos = it->find_first_of(',');
			int tagId = -1;
			//found a comma
			if(pos != std::string::npos){
				//make sure there isn't another one
				if(it->find_first_of(',', pos + 1) == std::string::npos){
					std::string tagClass;
					tagName.assign(*it, 0, pos);
					tagClass.assign(*it, pos + 1, it->length() - 1);
					tagId = tagModel->find(tagName, tagClass);
					//if the tag isn't found then the id is negative, create new tag
					//otherwise apply the tag to the selected work
					if(tagId < 0){
						tagModel->addClassAndTag(QString(tagClass.c_str()), QString(tagName.c_str()));
						tagId = tagModel->find(tagName, tagClass);
					} 
					//just in case, this shouldn't ever be false..
					if(tagId > 0)
						tagModel->addWorkTagAssociation(selectedWorkId, tagId);
				} else {
					std::cerr << "Skipping ambiguous tag definition: " <<  *it << endl;
					continue;
				}
			} else {
				tagId = tagModel->find(*it);
				//if we cannot find the tag then we skip it
				//otherwise we apply it to the selected work
				if(tagId < 0)
					std::cerr << "Cannot find tag: " << *it << " skipping." << std::endl;
				else 
					tagModel->addWorkTagAssociation(selectedWorkId, tagId);
			}
		}
	}

	if(runGui){
		QWidget * topWidget = new QWidget;
		topWidget->setWindowTitle("Data Jockey Annotator");
		app.setStyle(new QCleanlooksStyle);

		//views
		TagEditor * tagEditor = new TagEditor(tagModel);
		WorkDetailView * workDetailView = new WorkDetailView(tagModel, db);
		WorkDBView * workDBView = new WorkDBView(filteredWorkTableModel);
		workDBView->showFilterButtons(false);

		//layouts
		QVBoxLayout * layout = new QVBoxLayout;
		QSplitter * vertSplit = new QSplitter(Qt::Vertical, topWidget);
		QSplitter * horiSplit = new QSplitter(Qt::Horizontal, topWidget);
		horiSplit->addWidget(workDetailView);
		horiSplit->addWidget(tagEditor);
		vertSplit->addWidget(horiSplit);
		vertSplit->addWidget(workDBView);
		layout->addWidget(vertSplit);

		//connections
		QObject::connect(workDBView,
				SIGNAL(workSelected(int)),
				workDetailView,
				SLOT(setWork(int)));

		if(selectedWorkId >= 0)
			workDBView->selectWork(selectedWorkId);

		topWidget->setLayout(layout);
		topWidget->show();

		return app.exec();
	} else 
		return 0;
}
