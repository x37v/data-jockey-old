#ifndef INTEPRETER_MODEL_HPP
#define INTEPRETER_MODEL_HPP

#include <QObject>
#include <QList>
#include <QString>

class InterpreterModel : public QObject {
	Q_OBJECT
	public:
		InterpreterModel(QObject * parent = NULL);
	signals:
		void newOutput(QString output);
		void newInput(QString output);
	public slots:
		void addToInput(QString input);
		void addToOutput(QString output);
	private:
		QList<QString> mInput;
		QList<QString> mInputHistory;
		QList<QString> mOutputHistory;
};

#endif

