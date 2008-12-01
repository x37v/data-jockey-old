/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REMOTE_WORK_FILTER_MODEL_HPP
#define REMOTE_WORK_FILTER_MODEL_HPP

#include "workfiltermodel.hpp"
#include <vector>
#include <set>
#include <string>
#include <QObject>
#include <QString>
#include <QList>

class RemoteWorkFilterModelProxy;

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
		RemoteWorkFilterModelProxy * proxy() const;
	signals:
		void workListing(QList<int> works);
		void nameChanged(QString name);
		void descriptionChanged(QString description);
		void deleteingSelf();
	public slots:
		virtual void reportWorks();
	private:
		RemoteWorkFilterModelProxy * mProxy;
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
		void removeSelf();
	private:
		//this indicates if we have been updated since the filter was executed
		bool mUpdated;
		std::set<int> mSelectedWorks;
		QString mName;
		QString mDescription;
};

#endif

