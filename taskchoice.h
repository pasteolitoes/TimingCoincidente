#ifndef TASKCHOICE_H
#define TASKCHOICE_H

#include <QDialog>

namespace Ui {
class TaskChoice;
}

class TaskChoice : public QDialog
{
    Q_OBJECT

public:
    explicit TaskChoice(QWidget *parent = 0);
    ~TaskChoice();

    int get() const;

    int task_type;

private slots:

    void on_trainButton_clicked();

    void on_acquiButton_clicked();

    void on_backButton_clicked();

private:
    Ui::TaskChoice *ui;
};

#endif // TASKCHOICE_H
