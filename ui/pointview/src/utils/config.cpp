/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 12/13/22.
//
#include "utils/config.h"

namespace autox {
namespace pointview {

bool Config::setNamespace(std::string name) {
  namespace_ = name;
  return true;
}

void Config::printConfig() { std::cout << node_ << std::endl; }

std::shared_ptr<Config> Config::getSubConfig(std::string name) {
  std::vector<std::string> prefix;
  boost::split(prefix, name, boost::is_any_of("."), boost::token_compress_on);
  // find node
  // node iterator
  std::queue<YAML::Node> node_queue;
  node_queue.push(node_);
  // string iterator
  for (unsigned int i = 0; i < prefix.size() && !node_queue.empty(); i++) {
    YAML::Node tmp_node = node_queue.front();
    node_queue.pop();
    // verify the key
    if (tmp_node[prefix[i]].IsDefined()) {
      node_queue.push(tmp_node[prefix[i]]);
    } else {
      return nullptr;
    }
  }
  YAML::Node tmp_node = node_queue.front();
  node_queue.pop();
  return std::make_shared<Config>(tmp_node);
}

Config::Config(YAML::Node node) : node_(node) {}

YAML::const_iterator Config::begin() { return node_.begin(); }

YAML::const_iterator Config::end() { return node_.end(); }

bool Config::saveSubConfig(std::string name,
                           std::shared_ptr<Config> sub_config) {
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
  currNode[prefix[i]] = sub_config->node_;

  return true;
}

std::string getFullName(std::string ns, std::string key) {
  return ns + "." + key;
}
}  // namespace pointview
}  // namespace autox
