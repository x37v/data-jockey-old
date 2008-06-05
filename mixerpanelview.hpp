#ifndef MIXER_PANEL_VIEW_HPP
#define MIXER_PANEL_VIEW_HPP

#include <QWidget>
class MasterView;
class DJMixerChannelView;
class DJMixerControlView;
class EQView;
class QGridLayout;
class CrossFadeView;

class MixerPanelView : public QWidget {
	Q_OBJECT
	public:
		MixerPanelView(unsigned int numMixers = 4, QWidget *parent = NULL);
		CrossFadeView * crossFade();
		MasterView * master();
		std::vector<DJMixerChannelView *> * mixerChannels();
	private:
		QGridLayout * mLayout;
		std::vector<DJMixerChannelView *> mDJMixerChannels;
		MasterView * mMaster;
		CrossFadeView * mXFade;
};

#endif

