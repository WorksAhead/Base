#include "LibraryEngineWidget.h"
#include "LibraryEngineItemWidget.h"

#include <QScrollArea>
#include <QPainter>
#include <QGridLayout>
#include <QStyleOption>

#include <algorithm>

LibraryEngineWidget::LibraryEngineWidget(ContextPtr context, QWidget* parent) : context_(context), QWidget(parent)
{
	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QWidget* w = new QWidget;

	ui_.setupUi(w);

	enginesLayout_ = new FlowLayout(0, 20, 20);

	ui_.enginesWidget->setLayout(enginesLayout_);

	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(w);

	QGridLayout* layout = new QGridLayout;
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(scrollArea);

	setLayout(layout);

	firstShow_ = true;
}

LibraryEngineWidget::~LibraryEngineWidget()
{
}

void LibraryEngineWidget::addEngine(const QString& engineName, const QString& engineVersion)
{
	LibraryEngineItemWidget* w = new LibraryEngineItemWidget(context_);
	w->setEngineVersion(qMakePair(engineName, engineVersion));

	int idx = 0;
	while (idx < enginesLayout_->count()) {
		LibraryEngineItemWidget* w = (LibraryEngineItemWidget*)enginesLayout_->itemAt(idx)->widget();
		if (qMakePair(engineName, engineVersion) < w->getEngineVersion()) {
			break;
		}
		++idx;
	}

	enginesLayout_->insertWidget(idx, w);

	QString key = engineName.toLower() + "\n" + engineVersion.toLower();
	engineItemWidgets_.insert(key, w);
}

void LibraryEngineWidget::removeEngine(const QString& engineName, const QString& engineVersion)
{
	QString key = engineName.toLower() + "\n" + engineVersion.toLower();

	LibraryEngineItemWidget* w = engineItemWidgets_.value(key, 0);
	if (w) {
		w->deleteLater();
		enginesLayout_->removeWidget(w);
		engineItemWidgets_.remove(key);
	}
}

void LibraryEngineWidget::showEvent(QShowEvent*)
{
	if (firstShow_) {
		refresh();
		firstShow_ = false;
	}
}

void LibraryEngineWidget::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LibraryEngineWidget::refresh()
{
	for (;;) {
		QLayoutItem* li = enginesLayout_->takeAt(0);
		if (!li) {
			break;
		}
		li->widget()->deleteLater();
		delete li;
	}

	engineItemWidgets_.clear();

	std::vector<EngineVersion> installedEngineList;
	context_->getEngineList(installedEngineList);

	std::sort(installedEngineList.begin(), installedEngineList.end(), [](const EngineVersion& lhs, const EngineVersion& rhs){
		return (lhs.first + " " + lhs.second) < (rhs.first + " " + rhs.second);
	});

	for (const EngineVersion& v : installedEngineList)
	{
		LibraryEngineItemWidget* w = new LibraryEngineItemWidget(context_);
		w->setEngineVersion(qMakePair(v.first.c_str(), v.second.c_str()));
		enginesLayout_->addWidget(w);
		QString key = QString(v.first.c_str()).toLower() + "\n" + QString(v.second.c_str()).toLower();
		engineItemWidgets_.insert(key, w);
	}
}

