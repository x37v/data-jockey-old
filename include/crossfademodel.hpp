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

#ifndef CROSS_FADE_MODEL_HPP
#define CROSS_FADE_MODEL_HPP

#include <QObject>

class CrossFadeModel : public QObject {
	Q_OBJECT
	public:
		CrossFadeModel(unsigned int numMixers, QObject * parent = NULL);
		CrossFadeModel(QObject * parent = NULL);
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(CrossFadeModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);
		float position() const;
		float valueLeft() const;
		float valueRight() const;
		bool enabled() const;
		unsigned int mixerLeft() const;
		unsigned int mixerRight() const;
		float scaleVolume(unsigned int index, float mixer_volume);
	signals:
		void mixersChanged(unsigned int left, unsigned int right);
		void disabled();
		void positionChanged(float val);
      void leftMixerChanged(unsigned int);
      void rightMixerChanged(unsigned int);
      void enabled(bool);
	public slots:
      void setLeftMixer(unsigned int left);
      void setRightMixer(unsigned int right);
		void setMixers(unsigned int left, unsigned int right);
		void disable();
		void setPosition(float val);
      void enable(bool value = true);
	protected slots:
		void updatePosition(float val);
	private:
		bool mRecursing;
		float mPosition;
		unsigned int mNumMixers;
		unsigned int mLeft;
		unsigned int mRight;
		bool mEnabled;
};

#endif

