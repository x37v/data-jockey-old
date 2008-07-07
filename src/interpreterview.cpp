#include "interpreterview.hpp"
#include "interpretermodel.hpp"
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QKeyEvent>

InterpreterView::InterpreterView(InterpreterModel * model, QWidget * parent) :
	QWidget(parent)
{
	mModel = model;
	mHistoryIndex = 0;
	mWaitingForOutput = false;
	mTextEntry = new QLineEdit(this);
	mOutputDisplay = new QTextEdit(this);
	QVBoxLayout * layout = new QVBoxLayout(this);

	mOutputDisplay->setReadOnly(true);

	layout->addWidget(mOutputDisplay, 10);
	layout->addWidget(mTextEntry, 0);

	setLayout(layout);

	//connect internal sigs and slots
	QObject::connect(
			mTextEntry, SIGNAL(returnPressed()),
			this, SLOT(acceptNewInput()));
}

void InterpreterView::keyPressEvent ( QKeyEvent * event ){
	//if(event->matches(QKeySequence::Copy)){
		//emit(newInput(""));
	if(event->matches(QKeySequence::MoveToPreviousLine)){
		if (mHistoryIndex == 0){
			mCurrentInput = mTextEntry->text();
			mHistoryIndex = 1;
			mTextEntry->setText(mModel->line(0));
		} else if (mHistoryIndex < mModel->inputHistoryLines()) {
			mTextEntry->setText(mModel->line(mHistoryIndex));
			mHistoryIndex += 1;
			if (mHistoryIndex > mModel->inputHistoryLines())
				mHistoryIndex = mModel->inputHistoryLines();
		}
	} else if(event->matches(QKeySequence::MoveToNextLine)){
		if (mHistoryIndex == 0){
			//do nothing
		} else if (mHistoryIndex == 1){
			mTextEntry->setText(mCurrentInput);
			mHistoryIndex = 0;
		} else {
			mHistoryIndex -= 1;
			mTextEntry->setText(mModel->line(mHistoryIndex - 1));
		}
	} else {
		QWidget::keyPressEvent(event);
	}
}

void InterpreterView::setInput(QString line){
	mTextEntry->setText(line);
}

void InterpreterView::addToOutput(QString line){
	mWaitingForOutput = false;
	mOutputDisplay->moveCursor(QTextCursor::End);
	mOutputDisplay->insertPlainText(line);
	mOutputDisplay->moveCursor(QTextCursor::End);
}

void InterpreterView::acceptNewInput(){
	QString newText = mTextEntry->text();
	mHistoryIndex = 0;
	mTextEntry->clear();

	//move the cursor to the end because the user may have moved it
	mOutputDisplay->moveCursor(QTextCursor::End);
	if(mWaitingForOutput){
		QString html = QString("<strong>?> %1</strong><br>\n").arg(newText);
		mOutputDisplay->insertHtml(html);
	} else {
		QString html = QString("<strong>>> %1</strong><br>\n").arg(newText);
		mOutputDisplay->insertHtml(html);
	}
	mWaitingForOutput = true;
	emit(newInput(newText));
	mOutputDisplay->moveCursor(QTextCursor::End);
}

