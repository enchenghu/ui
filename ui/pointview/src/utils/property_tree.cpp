/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#include "utils/property_tree.h"

namespace autox {
namespace pointview {

PropertyTree::PropertyTree(QTreeWidget* root,
                           std::shared_ptr<QTreeWidgetItem> top_item,
                           std::shared_ptr<QFont> font) {
  root_ = root;
  top_item_ = top_item;
  font_ = font;
  root->setFont(*font);
}

void PropertyTree::addProperty(const QString& property,
                               std::shared_ptr<QWidget> value) {
  auto tree_item = std::make_shared<QTreeWidgetItem>(QStringList() << property);
  top_item_->addChild(tree_item.get());
  sub_items_.push_back(tree_item);
  root_->setItemWidget(tree_item.get(), 1, value.get());
  tree_item->setFont(0, *font_);
}

void PropertyTree::addProperties(const PropertyPairs& properties) {
  for (auto& kv : properties) {
    addProperty(kv.first, kv.second);
  }
}

std::shared_ptr<PropertyTree> PropertyTree::createPropertySubTree(
    const QString& title) {
  auto tree_item = std::make_shared<QTreeWidgetItem>(QStringList() << title);
  top_item_->addChild(tree_item.get());
  sub_items_.push_back(tree_item);
  tree_item->setFont(0, *font_);
  std::shared_ptr<QFont> font = std::make_shared<QFont>();
  font->setPointSize(font_->pointSize() - 1);
  auto t = std::make_shared<PropertyTree>(root_, tree_item, font);
  sub_trees_.push_back(t);
  return t;
}

std::shared_ptr<QTreeWidgetItem> PropertyTree::getTopItem() {
  return top_item_;
}

}  // namespace pointview
}  // namespace autox
