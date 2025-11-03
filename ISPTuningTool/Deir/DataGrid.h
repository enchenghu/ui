#ifndef DataGrid_H
#define DataGrid_H


#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>


class MicroDataGrid : public QWidget {
    Q_OBJECT
public:
    explicit MicroDataGrid(QWidget *parent = nullptr) : QWidget(parent) {
        setupUI();
        updateData({
            {0.851, 0.124, 0.055, 0.923},
            {0.092, 0.874, 0.044, 0.891},
            {0.023, 0.112, 0.953, 0.834}
        });
    }
    
    void updateData(const QVector<QVector<double>>& data) {
        if (data.size() != 3 || data[0].size() != 4) return;
        
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 4; ++col) {
                QString text = QString::number(data[row][col], 'f', 3);
                dataLabels[row][col]->setText(text);
                
                // 根据数值大小设置颜色提示（可选）
                // if (data[row][col] > 0.8) {
                //     dataLabels[row][col]->setStyleSheet(getDataStyleHigh());
                // } else if (data[row][col] < 0.2) {
                //     dataLabels[row][col]->setStyleSheet(getDataStyleLow());
                // } else {
                //     dataLabels[row][col]->setStyleSheet(getDataStyleNormal());
                // }
            }
        }
    }

private:
    QLabel* dataLabels[3][4];
    
    void setupUI() {
        // 设置网格整体样式
        setStyleSheet("QWidget { background-color: white; }");
        
        QGridLayout *gridLayout = new QGridLayout(this);
        gridLayout->setSpacing(3);
        gridLayout->setContentsMargins(5, 5, 5, 5);
        
        // 列标题
        QStringList colTitles = {"R", "G", "B", "IR"};
        for (int col = 0; col < 4; ++col) {
            QLabel *label = createHeaderLabel(colTitles[col], 50, 20);
            gridLayout->addWidget(label, 0, col + 1);
        }
        
        // 行标题和数据
        QStringList rowTitles = {"R", "G", "B"};
        for (int row = 0; row < 3; ++row) {
            // 行标题
            QLabel *rowLabel = createHeaderLabel(rowTitles[row], 25, 30);
            gridLayout->addWidget(rowLabel, row + 1, 0);
            
            // 数据单元格
            for (int col = 0; col < 4; ++col) {
                QLabel *label = createDataLabel("0.000", 50, 30);
                gridLayout->addWidget(label, row + 1, col + 1);
                dataLabels[row][col] = label;
            }
        }
    }
    
    QLabel* createHeaderLabel(const QString& text, int width, int height) {
        QLabel *label = new QLabel(text);
        label->setAlignment(Qt::AlignCenter);
        label->setFixedSize(width, height);
        label->setStyleSheet(
            "QLabel {"
            "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6d6d6d, stop:1 #4a4a4a);"
            "    color: white;"
            "    border: 1px solid #3a3a3a;"
            "    border-radius: 2px;"
            "    font-size: 9pt;"
            "    font-weight: bold;"
            "    padding: 1px;"
            "}"
        );
        return label;
    }
    
    QLabel* createDataLabel(const QString& text, int width, int height) {
        QLabel *label = new QLabel(text);
        label->setAlignment(Qt::AlignCenter);
        label->setFixedSize(width, height);
        label->setStyleSheet(getDataStyleNormal());
        return label;
    }
    
    QString getDataStyleNormal() {
        return "QLabel {"
               "    background-color: #ffffff;"
               "    border: 1px solid #d0d0d0;"
               "    border-radius: 2px;"
               "    font-size: 10pt;"
               "    font-family: 'Consolas', monospace;"
               "    font-weight: 500;"
               "    padding: 3px;"
               "    color: #333;"
               "}"
               "QLabel:hover {"
               "    background-color: #f0f8ff;"
               "    border: 1px solid #a0c8ff;"
               "}";
    }
    
    QString getDataStyleHigh() {
        return getDataStyleNormal() + 
               "QLabel {"
               "    background-color: #fff0f0;"
               "    color: #c00000;"
               "    font-weight: 600;"
               "}";
    }
    
    QString getDataStyleLow() {
        return getDataStyleNormal() + 
               "QLabel {"
               "    background-color: #f0f0ff;"
               "    color: #0000c0;"
               "}";
    }
};


#endif