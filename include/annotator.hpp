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
