/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 12/15/22.
//

#ifndef POINTVIEW_DISPLAY_CONTEXT_H
#define POINTVIEW_DISPLAY_CONTEXT_H

#include "utils/property_tree.h"
#include "utils/visualization/pcl_visualizer.h"

namespace autox {
namespace pointview {

class DisplayContext {
  using Viewer = autox::visualization::PCLVisualizer;

 public:
  DisplayContext(QWidget* parent, Viewer::Ptr viewer_ptr,
                 QTreeWidget* device_tree, QTreeWidget* tool_tree);
  QWidget* getParent();
  Viewer::Ptr getViewerPtr();
  std::shared_ptr<PropertyTree> createDevicePropertyTree(const QString& title);
  std::shared_ptr<PropertyTree> createToolPropertyTree(const QString& title);
  std::shared_ptr<QFont> getFont();
  void setFont(std::shared_ptr<QFont> font);

 private:
  std::shared_ptr<PropertyTree> createPropertyTree(QTreeWidget* root,
                                                   const QString& title);

 private:
  QWidget* parent_;
  Viewer::Ptr viewer_ptr_;
  QTreeWidget* device_tree_;
  QTreeWidget* tool_tree_;
  std::shared_ptr<QFont> property_tree_top_font_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_DISPLAY_CONTEXT_H
