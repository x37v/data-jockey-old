#include "djmixerchannelview.hpp"
#include "djmixercontrolview.hpp"
#include "mixerchannelview.hpp"

DJMixerChannelView::DJMixerChannelView(QWidget * parent)
	: QObject(parent) 
{
	mDJMixerControl = new DJMixerControlView(parent);
	mMixerChannel = new MixerChannelView(parent);
}

DJMixerControlView * DJMixerChannelView::DJMixerControl(){
	return mDJMixerControl;
}

MixerChannelView * DJMixerChannelView::mixerChannel(){
	return mMixerChannel;
}

