#ifndef INTEPRETER_MODEL_HPP
#define INTEPRETER_MODEL_HPP

#include <QObject>
#include <QList>
#include <QString>
#include <QFile>

class InterpreterModel : public QObject {
	Q_OBJECT
	public:
		InterpreterModel(QObject * parent = NULL);
		//returns the number of input lines that we have in our history
		unsigned int inputHistoryLines();
		QString line(unsigned int lineno);
	signals:
		void newOutput(QString output);
		void newInput(QString output);
		void cancelingInput();
	public slots:
		void addToInput(QString input);
		void cancelInput();
		void addToOutput(QString output);
		void setHistoryFile(QString filename);
		void writeHistory();
	private:
		QList<QString> mInput;
		QList<QString> mInputHistory;
		QList<QString> mOutputHistory;
		QFile mHistoryFile;
};

#endif

