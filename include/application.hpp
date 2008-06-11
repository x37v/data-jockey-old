#ifndef APPLICATION_HPP
#define APPLICATION_HPP

class MixerPanelView;
class MixerPanelModel;
class AudioDriver;

#include <QObject>

class DataJockeyApplication {
	public:
		static int run(int argc, char *argv[]);
		static void connectMixerPanelModelView(MixerPanelModel * model, MixerPanelView * view);
		static void connectMixerPanelModelDriver(MixerPanelModel * model, AudioDriver * driver);
};

#endif

