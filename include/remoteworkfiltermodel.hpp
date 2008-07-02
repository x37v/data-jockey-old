#ifndef REMOTE_WORK_FILTER_MODEL_HPP
#define REMOTE_WORK_FILTER_MODEL_HPP

#include "workfiltermodel.hpp"
#include <vector>
#include <set>
#include <string>
#include <QObject>
#include <QString>
#include <QList>

//this is a filter that is executed in another thread
//for instance, in an interpreter
class RemoteWorkFilterModel : public QObject {
	Q_OBJECT
	public:
		RemoteWorkFilterModel(std::string name = "RemoteWorkFilterModel", 
				std::string description = "RemoteWorkFilterModel Description");
		virtual ~RemoteWorkFilterModel();
		void clearWorks();
		void addWork(int id);
		void setWorks(std::vector<int> idlist);
	signals:
		void workListing(QList<int> works);
		void nameChanged(QString name);
		void descriptionChanged(QString description);
		void deleteingSelf();
	public slots:
		void reportWorks();
	private:
		QList<int> mSelectedWorks;
};

//this is a filter that actually gets its data from a remote filter
class RemoteWorkFilterModelProxy : public WorkFilterModel {
	Q_OBJECT
	public:
		RemoteWorkFilterModelProxy(QObject * parent = NULL);
		virtual ~RemoteWorkFilterModelProxy();
		virtual bool beforeFilter();
		virtual bool acceptsWork(int work_id);
		virtual std::string description();
		virtual std::string name();
	signals:
		void requestingUpdate();
		//this is called to remove us from the filter list
		void deleteing(WorkFilterModel * model);
	public slots:
		void setWorks(QList<int> idlist);
		void setName(QString name);
		void setDescription(QString description);
	protected slots:
		void removeSelf();
	private:
		//this indicates if we have been updated since the filter was executed
		bool mUpdated;
		std::set<int> mSelectedWorks;
		QString mName;
		QString mDescription;
};

#endif
