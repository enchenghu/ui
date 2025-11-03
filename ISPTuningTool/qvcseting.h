#ifndef QVCSETING_H
#define QVCSETING_H

#include <QMap>
#include <QWidget>

namespace Ui {
class QVCSeting;
}

class QVCSeting : public QWidget
{
    Q_OBJECT

public:
    explicit QVCSeting(QWidget *parent = nullptr);
    ~QVCSeting();
private:
    int GetBitWidthbyIndex(int index);
    int GetImageTypebyIndex(int index);

private slots:
    void on_SetButton_clicked();

private:
    Ui::QVCSeting *ui;
    QMap<int,int> VCImageType;
    QMap<int,uint32_t> VCImageWidth;
    QMap<int,uint32_t> VCImageHeight;
    QStringList bitWidthList;
    void *handle;
    int VCcount=1;
    bool VC0_isRaw;
    bool VC1_isRaw;

};

#endif // QVCSETING_H
