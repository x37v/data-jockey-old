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

#ifndef APPLICATION_MODEL_HPP
#define APPLICATION_MODEL_HPP

#include <QSqlDatabase>
#include <QObject>

class WorkTableModel;
class MixerPanelModel;
class TagModel;
class WorkFilterModelProxy;
class WorkFilterList;
class InterpreterModel;
class RemoteWorkFilterModel;
class OscReceiver;
//XXX hack for now
class OscSender;

class ApplicationModel : public QObject {
	Q_OBJECT
	public:
		static void setNumberOfMixers(unsigned int num);
		static void setDataBase(std::string type, 
				std::string name, 
				std::string username = "",
				std::string password = "", 
				int port = -1, 
				std::string host = std::string("localhost"));
		static ApplicationModel * instance();

		QSqlDatabase db() const;
		WorkTableModel * WorkTable() const;
		MixerPanelModel * mixerPanel() const;
		TagModel * tagModel() const;
		WorkFilterModelProxy * filteredWorkTable() const;
		WorkFilterList * workFilterList() const;
		InterpreterModel * interpreter() const;
		OscReceiver * oscReceiver() const;
		OscSender * oscSender() const;
	protected:
		ApplicationModel();
		ApplicationModel(const ApplicationModel&);
		ApplicationModel& operator= (const ApplicationModel&);
		virtual ~ApplicationModel();
	private:
		static ApplicationModel * cInstance;
		static unsigned int cNumMixers;
		static QSqlDatabase cDB;
		WorkTableModel * mWorkTable;
		MixerPanelModel * mMixerPanel;
		TagModel * mTagModel;
		WorkFilterModelProxy * mFilterProxy;
		WorkFilterList * mWorkFilterList;
		InterpreterModel * mInterp;
		OscReceiver * mOscReceiver;
		OscSender * mOscSender;
};

class ApplicationModelProxy : public QObject {
	Q_OBJECT
	public:
		ApplicationModelProxy(Qt::ConnectionType type = Qt::AutoCompatConnection, 
				QObject * parent = NULL);
		MixerPanelModel * mixerPanel() const;
		void addFilter(RemoteWorkFilterModel * filter);
		void removeFilter(RemoteWorkFilterModel * filter);
		void removeFilter(unsigned int i);
		unsigned int numFilters();
		RemoteWorkFilterModel * filter(unsigned int i);
	private:
		QList<RemoteWorkFilterModel *> mFilterList;
		MixerPanelModel * mMixerPanel;
};

#endif

