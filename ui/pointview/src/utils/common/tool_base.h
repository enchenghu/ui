/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 11/29/22.
//

#ifndef POINTVIEW_TOOL_BASE_H
#define POINTVIEW_TOOL_BASE_H

#include <QObject>
#include <QTreeWidgetItem>
#include <memory>
#include <vector>

#include "utils/common/display_context.h"
#include "utils/visualization/pcl_visualizer.h"

namespace autox {
namespace pointview {

class ToolBase : public QObject {
  Q_OBJECT
 public:
  explicit ToolBase(const std::shared_ptr<DisplayContext>& context,
                    const QString& title)
      : QObject(context->getParent()), viewer_(context->getViewerPtr()) {
    tool_property_tree_ = context->createToolPropertyTree(title);
  }
  ~ToolBase() override = default;

 signals:
  void refreshView();

 protected:
  void addProperty(const QString& property,
                   const std::shared_ptr<QWidget>& value) {
    tool_property_tree_->addProperty(property, value);
  };

 protected:
  autox::visualization::PCLVisualizer::Ptr viewer_;
  // property_tree_
  std::shared_ptr<PropertyTree> tool_property_tree_;
};

}  // namespace pointview
}  // namespace autox
#endif  // POINTVIEW_TOOL_BASE_H
