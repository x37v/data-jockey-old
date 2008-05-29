#ifndef MIXER_PANEL_VIEW_HPP
#define MIXER_PANEL_VIEW_HPP

#include <QWidget>
#include "masterview.hpp"

class DJMixerChannelView;
class EQView;
class QGridLayout;

class MixerPanelView : public QWidget {
	Q_OBJECT
	public:
		MixerPanelView(unsigned int numMixers = 4, QWidget *parent = NULL);
		//XXX destructor?
	private:
		QGridLayout * mLayout;
		std::vector<DJMixerChannelView *> mDJMixerChannels;
		MasterView * mMaster;
};

#endif

