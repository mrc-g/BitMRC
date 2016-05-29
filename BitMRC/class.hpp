#pragma once
#include <QWidget>
#include "ui_class.h"

class Class : public QWidget {
	Q_OBJECT

public:
	Class(QWidget * parent = Q_NULLPTR);
	~Class();

private:
	Ui::Class ui;
};
