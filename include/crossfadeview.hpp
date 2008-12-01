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

#ifndef CROSS_FADE_VIEW_HPP
#define CROSS_FADE_VIEW_HPP

#include <QWidget>
#include <vector>

class QVBoxLayout;
class QSlider;
class QComboBox;

class CrossFadeView : public QWidget {
	Q_OBJECT
	public:
		CrossFadeView(unsigned int numMixers, QWidget *parent = NULL);
		~CrossFadeView();
	signals:
		void mixersChanged(unsigned int left, unsigned int right);
		void disabled();
		void positionChanged(float val);
	public slots:
		void setMixers(unsigned int left, unsigned int right);
		void disable();
		void setPosition(float val);
	protected slots:
		void setIndex(int index);
		void setPosition(int pos);
	private:
		QVBoxLayout * mLayout;
		QSlider * mSlider;
		QComboBox * mSelection;
		std::vector<unsigned int *> mPairList;
};

#endif

