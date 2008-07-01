#ifndef INTEPRETER_IO_PROXY_HPP
#define INTEPRETER_IO_PROXY_HPP

#include <QObject>

#ifndef DATAJOCKEY_INTERP_PIPE_LOC
#define DATAJOCKEY_INTERP_PIPE_LOC "/tmp/datajockey_interp_pipe"
#endif

//this class simply acts as a way to get text to and from an
//interpreter that is running in another thread [and using another language]
//it connects itself to the application model's intepreter model

class InterpreterIOProxy : public QObject {
	Q_OBJECT
	public:
		InterpreterIOProxy();
		//bool newInput();
		//std::string getInput();
		static std::string pipeLocation();
		static void processEvents();
	signals:
		void newOutput(QString output);
	public slots:
		void addToInput(QString input);
		void addToOutput(std::string output);
	protected:
		QList<QString> mInputList;
};

#endif

