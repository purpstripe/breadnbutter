#ifndef MORESHELTERINFO_H
#define MORESHELTERINFO_H

#include <QDialog>

namespace Ui {
class MoreShelterInfo;
}

class MoreShelterInfo : public QDialog
{
    Q_OBJECT

public:
    explicit MoreShelterInfo(QWidget *parent = nullptr);
    ~MoreShelterInfo();

private:
    Ui::MoreShelterInfo *ui;
};

#endif // MORESHELTERINFO_H
