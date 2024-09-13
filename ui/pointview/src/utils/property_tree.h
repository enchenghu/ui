/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/

#ifndef POINTVIEW_PROPERTY_TREE_H
#define POINTVIEW_PROPERTY_TREE_H

#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <memory>

namespace autox {
namespace pointview {

using PropertyPairs = std::vector<std::pair<QString, std::shared_ptr<QWidget>>>;

class PropertyTree {
 public:
  PropertyTree(QTreeWidget* root, std::shared_ptr<QTreeWidgetItem> top_item,
               std::shared_ptr<QFont> font);
  void addProperty(const QString& property, std::shared_ptr<QWidget> value);
  void addProperties(const PropertyPairs& properties);
  std::shared_ptr<PropertyTree> createPropertySubTree(const QString& title);
  std::shared_ptr<QTreeWidgetItem> getTopItem();

 private:
  std::shared_ptr<QTreeWidgetItem> top_item_;
  std::vector<std::shared_ptr<QTreeWidgetItem>> sub_items_;
  std::vector<std::shared_ptr<PropertyTree>> sub_trees_;
  QTreeWidget* root_;
  std::shared_ptr<QFont> font_;
};

}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_PROPERTY_TREE_H
