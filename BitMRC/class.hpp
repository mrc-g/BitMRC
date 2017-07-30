#pragma once
#ifdef LINUX
#include <QWidget>
#else
#include <QtWidgets\qwidget.h>
#endif

#include "ui_class.h"

class Class : public QWidget {
	Q_OBJECT

public:
	Class(QWidget * parent = Q_NULLPTR);
	~Class();

private:
	Ui::Class ui;
};
