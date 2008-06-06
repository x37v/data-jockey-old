#include "djmixerchannelview.hpp"
#include "djmixercontrolview.hpp"
#include "djmixerworkinfoview.hpp"
#include "mixerchannelview.hpp"

DJMixerChannelView::DJMixerChannelView(QWidget * parent)
	: QObject(parent) 
{
	mDJMixerControl = new DJMixerControlView(parent);
	mMixerChannel = new MixerChannelView(parent);
	mDJMixerWorkInfo = new DJMixerWorkInfoView(parent);
}

DJMixerControlView * DJMixerChannelView::DJMixerControl(){
	return mDJMixerControl;
}

DJMixerWorkInfoView * DJMixerChannelView::DJMixerWorkInfo(){
	return mDJMixerWorkInfo;
}

MixerChannelView * DJMixerChannelView::mixerChannel(){
	return mMixerChannel;
}

