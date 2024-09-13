/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "display_context.h"

namespace autox {
namespace pointview {

DisplayContext::DisplayContext(QWidget* parent, Viewer::Ptr viewer_ptr,
                               QTreeWidget* device_tree, QTreeWidget* tool_tree)
    : parent_(parent),
      viewer_ptr_(viewer_ptr),
      device_tree_(device_tree),
      tool_tree_(tool_tree) {}

QWidget* DisplayContext::getParent() { return parent_; }

DisplayContext::Viewer::Ptr DisplayContext::getViewerPtr() {
  return viewer_ptr_;
}

std::shared_ptr<PropertyTree> DisplayContext::createDevicePropertyTree(
    const QString& title) {
  return createPropertyTree(device_tree_, title);
}

std::shared_ptr<PropertyTree> DisplayContext::createToolPropertyTree(
    const QString& title) {
  return createPropertyTree(tool_tree_, title);
}

std::shared_ptr<PropertyTree> DisplayContext::createPropertyTree(
    QTreeWidget* root, const QString& title) {
  auto tree_item = std::make_shared<QTreeWidgetItem>(QStringList() << title);
  root->addTopLevelItem(tree_item.get());
  tree_item->setExpanded(true);
  auto t =
      std::make_shared<PropertyTree>(root, tree_item, property_tree_top_font_);
  return t;
}
std::shared_ptr<QFont> DisplayContext::getFont() {
  return property_tree_top_font_;
}
void DisplayContext::setFont(std::shared_ptr<QFont> font) {
  property_tree_top_font_ = font;
}

}  // namespace pointview
}  // namespace autox
