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

#ifndef DJ_MIXER_WORK_INFO_VIEW_HPP
#define DJ_MIXER_WORK_INFO_VIEW_HPP

#include <QWidget>
#include <QString>

class QLineEdit;
class QVBoxLayout;

class DJMixerWorkInfoView : public QWidget {
	Q_OBJECT
	public:
		DJMixerWorkInfoView(QWidget * parent);
	public slots:
		void setArtistText(QString text);
		void setTitleText(QString text);
		void reset();
	private:
		QVBoxLayout * mLayout;
		QLineEdit * mArtistLabel;
		QLineEdit * mWorkLabel;
};

#endif
