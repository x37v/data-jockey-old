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

#ifndef EQ_MODEL_HPP
#define EQ_MODEL_HPP

#include <QObject>

class EQModel : public QObject {
	Q_OBJECT
	public:
		EQModel(QObject *parent = NULL);
		enum band {LOW,MID,HIGH};
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(EQModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);
		float high();
		float mid();
		float low();
		float value(band b);
	signals:
		void highChanged(float high);
		void midChanged(float mid);
		void lowChanged(float low);
		void highCutChanged(bool cut);
		void midCutChanged(bool cut);
		void lowCutChanged(bool cut);
		void valuesChanged(float low, float mid, float high, QObject * ob);
	public slots:
		void reset();
		void set(band b, float val);
		void toggleCut(band b);
		void cut(band b, bool cut);
		void cutLow(bool cut);
		void cutMid(bool cut);
		void cutHigh(bool cut);
		void setHigh(float val);
		void setMid(float val);
		void setLow(float val);
		void toggleHighCut();
		void toggleMidCut();
		void toggleLowCut();
	//protected slots, they don't emit signals, just change our internal values
	protected slots:
		void updateHigh(float val);
		void updateMid(float val);
		void updateLow(float val);
	private:
		float mHigh;
		float mMid;
		float mLow;
		float mHighLast;
		float mMidLast;
		float mLowLast;
		bool mCuttingHigh;
		bool mCuttingMid;
		bool mCuttingLow;
};

#endif

