/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 12/13/22.
//

#ifndef POINTVIEW_CONFIG_H
#define POINTVIEW_CONFIG_H

#include <fstream>
#include <iostream>
#include <queue>
#include <string>

#include <boost/algorithm/string/classification.hpp>  // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp>  // Include for boost::split
#include <glog/logging.h>
#include <yaml-cpp/yaml.h>

namespace autox {
namespace pointview {

class Config {
 public:
  Config() {}
  explicit Config(YAML::Node node);
  bool openConfig(std::string path) {
    node_ = YAML::LoadFile(path);
    return true;
  };
  bool saveConfig(std::string path) {
    std::ofstream out(path);
    out << node_;
    out.close();

    return true;
  };
  std::shared_ptr<Config> getSubConfig(std::string name);
  bool saveSubConfig(std::string name, std::shared_ptr<Config> sub_config);
  YAML::const_iterator begin();
  YAML::const_iterator end();
  // Separator .
  template <class T>
  bool getParameter(std::string name, T& value) {
    std::vector<std::string> prefix;
    boost::split(prefix, name, boost::is_any_of("."), boost::token_compress_on);
    // find node
    // node iterator
    YAML::Node currNode;
    currNode = node_;
    // string iterator
    for (unsigned int i = 0; i < prefix.size(); i++) {
      // verify the key
      if (currNode[prefix[i]].IsDefined()) {
        currNode.reset(currNode[prefix[i]]);
      } else {
        return false;
      }
    }
    try {
      value = currNode.as<T>();
    } catch (...) {
      LOG(INFO) << "type convert failed";
      return false;
    }
    return true;
  };
  template <class T>
  bool setParameter(std::string name, const T& value) {
    std::vector<std::string> prefix;
    boost::split(prefix, name, boost::is_any_of("."), boost::token_compress_on);
    // find node
    // node iterator
    YAML::Node currNode;
    currNode = node_;
    // string iterator
    unsigned int i = 0;
    for (; i < prefix.size() - 1; i++) {
      // verify the key
      if (!currNode[prefix[i]].IsDefined()) {
        currNode[prefix[i]] = YAML::Node();
      }
      currNode.reset(currNode[prefix[i]]);
    }
    if (currNode[prefix[i]].IsDefined() && currNode[prefix[i]].IsMap()) {
      return false;
    }
    currNode[prefix[i]] = value;

    return true;
  };
  bool setNamespace(std::string name);
  // for debuge
  void printConfig();

 private:
  YAML::Node node_;
  std::string namespace_{""};
};

std::string getFullName(std::string ns, std::string key);
}  // namespace pointview
}  // namespace autox

#endif  // POINTVIEW_CONFIG_H
