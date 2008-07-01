#ifndef INTEPRETER_IO_PROXY_HPP
#define INTEPRETER_IO_PROXY_HPP

#include <QObject>

//this class simply acts as a way to get text to and from an
//interpreter that is running in another thread [and using another language]
//it connects itself to the application model's intepreter model

class InterpreterIOProxy : public QObject {
	Q_OBJECT
	protected:
		InterpreterIOProxy();
		InterpreterIOProxy(const InterpreterIOProxy&);
		InterpreterIOProxy& operator= (const InterpreterIOProxy&);
		static QList<QString> mInputList;
		static InterpreterIOProxy * cInstance;
	public:
		static InterpreterIOProxy * instance();
		static bool newInput();
		static std::string getInput();
		static void processEvents();
	signals:
		void newOutput(QString output);
	public slots:
		static void addToInput(QString input);
		static void addToOutput(std::string output);
};

#endif

