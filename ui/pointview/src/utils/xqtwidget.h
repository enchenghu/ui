/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 11/28/22.
//

#ifndef POINT_VIEW_XQTWIDGET_H
#define POINT_VIEW_XQTWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <memory>

namespace autox {
namespace pointview {

template <typename T>
class XQtWidget : public QWidget {
 public:
  explicit XQtWidget(QString label = "Hello world") {
    layout_ = std::make_shared<QHBoxLayout>();
    label_ = std::make_shared<QLabel>();
    widget_ = std::make_shared<T>();
    label_->setText(label);
    layout_->addWidget(label_.get());
    layout_->addWidget(widget_.get());
    this->setLayout(layout_.get());
  }

  std::shared_ptr<QLabel> getLabel() { return label_; };
  std::shared_ptr<T> getWidget() { return widget_; };

 private:
  std::shared_ptr<QHBoxLayout> layout_;
  std::shared_ptr<QLabel> label_;
  std::shared_ptr<T> widget_;
};

using XCombobox = XQtWidget<QComboBox>;
using XTextField = XQtWidget<QLineEdit>;
using XSpinBox = XQtWidget<QSpinBox>;
using XDoubleSpinBox = XQtWidget<QDoubleSpinBox>;
using XCheckBox = XQtWidget<QCheckBox>;

}  // namespace pointview
}  // namespace autox
#endif  // POINT_VIEW_XQTWIDGET_H
