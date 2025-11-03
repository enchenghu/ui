#ifndef DEIR_H
#define DEIR_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include "DataGrid.h"
#include "imageWidget.h"
#include <utility>
#include "ManualWBProcessor.h"
#include "ColorTransformCalculator.h"
#include <QDialog>
#include <map>
namespace Ui {
class Deir;
}
class OriginalSizeViewer;
class Deir : public QWidget
{
    Q_OBJECT

public:
    explicit Deir(QWidget *parent = nullptr);
    ~Deir();

private slots:
    void deirProcess();
    void deirMeanProcess();
    void setInputFolder(); 
    void hideRoi();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    Ui::Deir *ui;
    std::shared_ptr<MicroDataGrid> deirMrx[3];
    void init();
    void initConnect();
    void saveInputFolder(const QString& path);
    void setAlgParams();
    QString getLastInputFolder();
    ImageWidget* pImage_;
    QSlider *m_widthSlider;
    QSlider *m_heightSlider;
    QLabel *m_widthLabel;
    QLabel * Label_name;
    void loadImageLablelListInit();
    void findIRStrings(const QStringList & );
    void processImage(const QString& filename_coIR, const QString& filename_noIR, int modeIndex);
    void resetOtherLabels(QLabel* cur_label);
    QVector<std::pair<int, QLabel*>> loadImageLablelList;
    std::map<QString, QVector<QVector<double>>> loadImagesMap;
    QString input_folder_;
    QStringList fileList_;
    QVector<std::pair<QString, QString>> filePairList_noir_;
    QVector<std::pair<QString, QString>> filePairList_irp0_;
    QVector<std::pair<QString, QString>> filePairList_irp1_;
    std::unique_ptr<ManualWBProcessor> wb_rgbir_processor_; 
    OriginalSizeViewer *viewer_;
    QString originalStyleSheet;
    QString greenStyle;
    QString oriStyle;
    QStringList fileList_noir_;
    QStringList fileList_irp0_;
    QStringList fileList_irp1_;
    QVector<QVector<ColorTransformCalculator::TransformResult>> traMat_all_;

};

// 原始尺寸显示窗口
class OriginalSizeViewer : public QDialog
{
    Q_OBJECT

public:
    OriginalSizeViewer(QWidget *parent = nullptr);
    ImageWidget* getImage() const {return m_imageWidget;}
private slots:
    void onWidthSliderChanged(int value);
    void onHeightSliderChanged(int value);    
private:
    ImageWidget* m_imageWidget;
    QSlider * width_Slider;
    QSlider * height_Slider;
    QLabel *label_width;
    QLabel *label_height;
};

#endif // DEIR_H
