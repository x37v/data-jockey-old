#ifndef DATAJOCKEY_ANNOTATOR_HPP
#define DATAJOCKEY_ANNOTATOR_HPP

#include <QObject>
#include <QWidget>
#include <QSqlDatabase>
#include "config.hpp"
#include <stdexcept>

//forward declarations
class	TagModel;
class	WorkTableModel;
class	WorkFilterModelProxy;
class TagEditor;
class WorkDetailView;
class WorkDBView;

class AnnotatorModel : public QObject {
	Q_OBJECT
	public:
		AnnotatorModel(datajockey::Configuration * config, QObject *parent = NULL) throw (std::runtime_error);
		virtual ~AnnotatorModel();
		TagModel * tagModel();
		WorkTableModel * workTableModel();
		WorkFilterModelProxy * filteredWorkTableModel();
		QSqlDatabase * db();
	private:
		TagModel * mTagModel;
		WorkTableModel * mWorkTableModel;
		WorkFilterModelProxy * mFilteredWorkTableModel;
		QSqlDatabase mDB;
};

class AnnotatorView : public QWidget {
	Q_OBJECT
	public:
		AnnotatorView(AnnotatorModel * model, QWidget * parent = NULL);
		void selectWork(int work_id);
	public slots:
		void deleteModel();
	private:
		AnnotatorModel * mModel;
		//views
		TagEditor * mTagEditor;
		WorkDetailView * mWorkDetailView;
		WorkDBView * mWorkDBView;
};

#endif
