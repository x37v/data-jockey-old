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

#ifndef MASTER_VIEW_HPP
#define MASTER_VIEW_HPP

#include <QWidget>

class QSlider;
class QDoubleSpinBox;
class QRadioButton;

class MasterView : public QObject {
	Q_OBJECT
	public:
		MasterView(unsigned int numMixers, QWidget *parent = NULL);
		QSlider * volume() const;
		QDoubleSpinBox * tempo() const;
		QWidget * tempoWidget() const;
	signals:
		void volumeChanged(float vol);
		void tempoChanged(float tempo);
		void tempoMulChanged(double mul);
		void syncSourceChanged(unsigned int src);
	//public slots will only be called externally
	//so they don't emit signals
	public slots:
		void setVolume(float vol);
		void setTempo(float tempo);
		void setTempoMul(double mul);
		//0 is master, 1 is mixer 0, 2 is mixer 1..
		void setSyncSource(unsigned int src);
	//protected slots are called internally and, do emit signals
	protected slots:
		void setVolume(int vol);
		void setTempoDouble(double tempo);
		void syncSrcClicked(bool clicked);
		void setTempoMulSelf(double tempo);
	private:
		bool mRecursing;
		QSlider * mVolumeSlider;
		QDoubleSpinBox * mTempo;
		QDoubleSpinBox * mTempoMul;
		std::vector<QRadioButton *> mSyncSource;
		QWidget * mTempoWidget;
};

#endif

