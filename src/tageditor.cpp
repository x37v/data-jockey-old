#include "tageditor.hpp"
#include "tagmodel.hpp"
#include "tagview.hpp"
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QRegExp>
#include <QHeaderView>

TagEditor::TagEditor(TagModel * model, QWidget * parent) :
	QWidget(parent) 
{
	QVBoxLayout * layout = new QVBoxLayout(this);
	QHBoxLayout * inputLayout = new QHBoxLayout;
	mTagView = new TagView(model, this);
	mAddTagBtn = new QPushButton(tr("create tag"), this);
	mTagClassSelect = new QComboBox(this);
	mTagNameInput = new QLineEdit(this);

	//allow for dragging from the tagview
	mTagView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	mTagView->setDragEnabled(true);

	//resize the headers to fit the data
	mTagView->header()->resizeSections(QHeaderView::ResizeToContents);

	inputLayout->addWidget(mTagClassSelect, 0);
	inputLayout->addWidget(mTagNameInput, 0);
	inputLayout->setContentsMargins(0,0,0,0);

	mTagClassSelect->setEditable(true);
	mTagClassSelect->setInsertPolicy(QComboBox::NoInsert);
	//populate the mTagClassSelect
	for (int i = 0; i < model->classList()->size(); ++i){
		mTagClassSelect->addItem(model->classList()->at(i)->second,
				model->classList()->at(i)->first);
	}

	layout->addWidget(mTagView, 10);
	layout->addLayout(inputLayout, 0);
	layout->addWidget(mAddTagBtn, 0, Qt::AlignHCenter);

	layout->setContentsMargins(0,0,0,0);
	setLayout(layout);

	//connect our internal signals/slots
	QObject::connect(mAddTagBtn,
			SIGNAL(clicked(bool)),
			this,
			SLOT(addTagButtonPushed()));
	QObject::connect(model,
			SIGNAL(classAdded(QPair<int, QString>)),
			this,
			SLOT(addToClassList(QPair<int, QString>)));
	QObject::connect(mTagView,
		SIGNAL(tagSelectionChanged(QList<int>)),
		this,
		SIGNAL(tagSelectionChanged(QList<int>)));
}

void TagEditor::addTagButtonPushed(){
	QRegExp rx("^\\s*$");
	QString text = mTagNameInput->text();
	//make sure it isn't just white space
	if(!rx.exactMatch(text)){
		int index = mTagClassSelect->currentIndex();
		QString className = mTagClassSelect->currentText();
		//make sure the class name isn't just white space
		if(!rx.exactMatch(className)){
			//is this a new class?
			if(className == mTagClassSelect->itemText(index)){
				int classId = mTagClassSelect->itemData(index).toInt();
				emit(tagAdded(classId, text)); 
			} else
				emit(tagAdded(className, text)); 
		}
	}
	//clear out the tag name input
	mTagNameInput->clear();
}

void TagEditor::addToClassList(QPair<int, QString> item){
	mTagClassSelect->addItem(item.second, item.first);
}

