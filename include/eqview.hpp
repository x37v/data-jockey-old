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

#ifndef EQVIEW_HPP
#define EQVIEW_HPP

#include <QWidget>

class QDial;
class QVBoxLayout;
class QWidget;

class EQView : public QWidget {
	Q_OBJECT
	public:
		EQView(QWidget *parent = NULL);
		QDial * low();
		QDial * mid();
		QDial * high();
		float highValue() const;
		float midValue() const;
		float lowValue() const;
	signals:
		void highChanged(float high);
		void midChanged(float mid);
		void lowChanged(float low);
	public slots:
		void setHigh(float val);
		void setHigh(int val);
		void setMid(float val);
		void setMid(int val);
		void setLow(float val);
		void setLow(int val);
	private:
		QDial * mLow;
		QDial * mMid;
		QDial * mHigh;
		QVBoxLayout * mLayout;
		//to prevent infinite loops
		bool mRecursing;
};

#endif
