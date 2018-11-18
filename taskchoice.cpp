#include "taskchoice.h"
#include "ui_taskchoice.h"

TaskChoice::TaskChoice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskChoice)
{
    ui->setupUi(this);
}

TaskChoice::~TaskChoice()
{
    delete ui;
}

void TaskChoice::on_trainButton_clicked()
{
    task_type = 1;
    this->accept();
}

void TaskChoice::on_acquiButton_clicked()
{
    task_type = 2;
    this->accept();
}

int TaskChoice::get() const{
    return task_type;
}

void TaskChoice::on_backButton_clicked()
{
    this->close();
}
