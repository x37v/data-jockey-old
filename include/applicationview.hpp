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

#ifndef APPLICATION_VIEW_HPP
#define APPLICATION_VIEW_HPP

#include <QWidget>

class ApplicationModel;
class WorkDetailView;
class WorkDBView;
class TagEditor;
class MixerPanelView;
class WorkFilterListView;

class InterpreterView;

class ApplicationView : public QWidget {
	Q_OBJECT
	public:
		ApplicationView(ApplicationModel * model);
		MixerPanelView * mixerPanel() const;
		WorkDBView * workDB() const;
		TagEditor * tagEditor() const;
		WorkDetailView * workDetail() const ;
	private:
		ApplicationModel * mModel;
		WorkDetailView * mWorkDetail;
		WorkDBView * mWorkDB;
		TagEditor * mTagEditor;
		MixerPanelView * mMixerPanel;
		WorkFilterListView * mWorkFilterList;

		InterpreterView * mInterpView;

		void connectToModel();
};

#endif

