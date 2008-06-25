#ifndef APPLICATION_HPP
#define APPLICATION_HPP

class MixerPanelView;
class MixerPanelModel;
class AudioDriver;

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include "buffer.hpp"

class DataJockeyApplication {
	public:
		static int run(int argc, char *argv[]);
};

#endif

