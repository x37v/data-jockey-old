#include <QApplication>
#include <QErrorMessage>
#include <QCleanlooksStyle>
#include <QWidget>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>

#include <QMainWindow>
#include <QFile>

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

#include "annotator.hpp"

AnnotatorModel::AnnotatorModel(datajockey::Configuration * config, QObject * parent) 
	throw(std::runtime_error) : QObject(parent) {
	//open the database
	try {
		mDB = QSqlDatabase::addDatabase(config->databaseAdapter().c_str());
		mDB.setDatabaseName(config->databaseName().c_str());
		if(config->databaseUserName() != "")
			mDB.setUserName(config->databaseUserName().c_str());
		if(config->databasePassword() != "")
			mDB.setPassword(config->databasePassword().c_str());
		//open, and if it won't open, throw an error
		if(!mDB.open())
			throw std::exception();
	} catch(std::exception& e) {
		throw std::runtime_error("Invalid database configuration data."
				"\nHave you provided a correct configuration file?");
	}
	mTagModel = new TagModel(mDB);
	mWorkTableModel = new WorkTableModel(mDB);
	mFilteredWorkTableModel = new WorkFilterModelProxy(mWorkTableModel);
}

AnnotatorModel::~AnnotatorModel(){
	delete mFilteredWorkTableModel;
	delete mWorkTableModel;
	delete mTagModel;
	mDB.close();
}

TagModel * AnnotatorModel::tagModel(){
	return mTagModel;
}

WorkTableModel * AnnotatorModel::workTableModel(){
	return mWorkTableModel;
}

WorkFilterModelProxy * AnnotatorModel::filteredWorkTableModel(){
	return mFilteredWorkTableModel;
}

QSqlDatabase * AnnotatorModel::db(){
	return &mDB;
}

AnnotatorView::AnnotatorView(AnnotatorModel * model, QWidget * parent) 
: QWidget(parent)
{
	setWindowTitle("Data Jockey Annotator");
	mModel = model;

	//views
	mTagEditor = new TagEditor(model->tagModel());
	mWorkDetailView = new WorkDetailView(model->tagModel(), *model->db());
	mWorkDBView = new WorkDBView(model->filteredWorkTableModel());
	mWorkDBView->showFilterButtons(false);

	//layouts
	QVBoxLayout * layout = new QVBoxLayout(this);
	QSplitter * vertSplit = new QSplitter(Qt::Vertical, this);
	QSplitter * horiSplit = new QSplitter(Qt::Horizontal, this);
	horiSplit->addWidget(mWorkDetailView);
	horiSplit->addWidget(mTagEditor);
	vertSplit->addWidget(horiSplit);
	vertSplit->addWidget(mWorkDBView);
	layout->addWidget(vertSplit);

	//connections
	QObject::connect(mWorkDBView,
			SIGNAL(workSelected(int)),
			mWorkDetailView,
			SLOT(setWork(int)));
}

void AnnotatorView::selectWork(int work_id){
	if(work_id >= 0)
		mWorkDBView->selectWork(work_id);
}

//XXX this is a hack I think.
void AnnotatorView::deleteModel(){
	delete mModel;
}

namespace po = boost::program_options;
using std::cout;
using std::endl;

#define UNDEFINED_RATING -100

int main(int argc, char *argv[]){

	QApplication app(argc, argv);
	AnnotatorModel * model;
	datajockey::Configuration * config = datajockey::Configuration::instance();
	std::string inputFile;
	int selectedWorkId = -1;
	bool runGui = true;
	int rating = UNDEFINED_RATING;
	std::vector<std::string> inputTags;
	std::string styleFile;

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
			("style-sheet,s", po::value<std::string>(), "Specify a style sheet file to use.")
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

		if (vm.count("style-sheet"))
			styleFile = vm["style-sheet"].as<std::string>();
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

	//open the database (by creating the model)
	try {
		model = new AnnotatorModel(config);
	} catch(std::exception& e) {
		qFatal(e.what());
		return app.exec();
	}

	if(!inputFile.empty()){
		selectedWorkId = model->workTableModel()->findWorkByPath(inputFile);
		if(selectedWorkId < 0){
			std::string str("Selected work:\n");
			str.append(inputFile);
			str.append("\ncannot be found in the database");
			//XXX make graphical
			qWarning(str.c_str());
		} else {
			//if there are tags, deal with them
			if(!inputTags.empty()){
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
							tagId = model->tagModel()->find(tagName, tagClass);
							//if the tag isn't found then the id is negative, create new tag
							//otherwise apply the tag to the selected work
							if(tagId < 0){
								model->tagModel()->addClassAndTag(QString(tagClass.c_str()), QString(tagName.c_str()));
								tagId = model->tagModel()->find(tagName, tagClass);
							} 
							//just in case, this shouldn't ever be false..
							if(tagId > 0)
								model->tagModel()->addWorkTagAssociation(selectedWorkId, tagId);
						} else {
							std::cerr << "Skipping ambiguous tag definition: " <<  *it << endl;
							continue;
						}
					} else {
						tagId = model->tagModel()->find(*it);
						//if we cannot find the tag then we skip it
						//otherwise we apply it to the selected work
						if(tagId < 0)
							std::cerr << "Cannot find tag: " << *it << " skipping." << std::endl;
						else 
							model->tagModel()->addWorkTagAssociation(selectedWorkId, tagId);
					}
				}
			}
			//XXX deal with a rating
			if(rating != UNDEFINED_RATING){
			}
		}
	}

	if(runGui){
		QMainWindow * mainWindow = new QMainWindow();
		AnnotatorView * view = new AnnotatorView(model, mainWindow);
		app.setStyle(new QCleanlooksStyle);

		bool styled = false;
		if(styleFile != std::string()){
			QFile file(styleFile.c_str());
			if(file.open(QFile::ReadOnly)){
				QString styleSheet = QLatin1String(file.readAll());
				app.setStyleSheet(styleSheet);
				styled = true;
			} else {
				qWarning("Cannot open style sheet: %s\n"
						"Application will use default style.", styleFile.c_str());
			}
		}
		if(!styled){
			QFile file(":/style.qss");
			if(file.open(QFile::ReadOnly)){
				QString styleSheet = QLatin1String(file.readAll());
				app.setStyleSheet(styleSheet);
			}
		}

		view->selectWork(selectedWorkId);
		//XXX I think this is a hack.. 
		//There should be some nicely set up dependency graph
		app.connect( &app, SIGNAL( aboutToQuit() ), view, SLOT( deleteModel() ) );
		mainWindow->setCentralWidget(view);
		mainWindow->show();
		return app.exec();
	} else 
		delete model;
	return 0;
}
