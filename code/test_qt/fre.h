#ifndef FRE_H
#define FRE_H
#include <QtWidgets/QDialog>
#include "ui_fre.h"

class fre : public QDialog
{
	Q_OBJECT

public:
	fre(QWidget *parent = 0);
	~fre();

private:
	Ui::Dialog ui;
};

#endif // !FRE_H
