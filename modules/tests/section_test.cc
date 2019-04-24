/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "linkerconfig/configwriter.h"
#include "linkerconfig/section.h"
#include "modules_testbase.h"

using namespace android::linkerconfig::modules;

constexpr const char* kSectionWithNamespacesExpectedResult =
    R"([test_section]
additional.namespaces = namespace1,namespace2
namespace.default.isolated = true
namespace.default.visible = true
namespace.default.search.paths = /search_path1
namespace.default.search.paths += /search_path2
namespace.default.search.paths += /search_path3
namespace.default.permitted.paths = /permitted_path1
namespace.default.permitted.paths += /permitted_path2
namespace.default.permitted.paths += /permitted_path3
namespace.default.asan.search.paths = /search_path1
namespace.default.asan.search.paths += /data/asan/search_path1
namespace.default.asan.search.paths += /search_path2
namespace.default.asan.permitted.paths = /permitted_path1
namespace.default.asan.permitted.paths += /data/asan/permitted_path1
namespace.default.asan.permitted.paths += /permitted_path2
namespace.default.links = namespace1,namespace2
namespace.default.link.namespace1.shared_libs = lib1.so
namespace.default.link.namespace1.shared_libs += lib2.so
namespace.default.link.namespace1.shared_libs += lib3.so
namespace.default.link.namespace2.allow_all_shared_libs = true
namespace.namespace1.isolated = false
namespace.namespace1.search.paths = /search_path1
namespace.namespace1.search.paths += /search_path2
namespace.namespace1.search.paths += /search_path3
namespace.namespace1.permitted.paths = /permitted_path1
namespace.namespace1.permitted.paths += /permitted_path2
namespace.namespace1.permitted.paths += /permitted_path3
namespace.namespace1.asan.search.paths = /search_path1
namespace.namespace1.asan.search.paths += /data/asan/search_path1
namespace.namespace1.asan.search.paths += /search_path2
namespace.namespace1.asan.permitted.paths = /permitted_path1
namespace.namespace1.asan.permitted.paths += /data/asan/permitted_path1
namespace.namespace1.asan.permitted.paths += /permitted_path2
namespace.namespace1.links = default,namespace2
namespace.namespace1.link.default.shared_libs = lib1.so
namespace.namespace1.link.default.shared_libs += lib2.so
namespace.namespace1.link.default.shared_libs += lib3.so
namespace.namespace1.link.namespace2.allow_all_shared_libs = true
namespace.namespace2.isolated = false
namespace.namespace2.search.paths = /search_path1
namespace.namespace2.search.paths += /search_path2
namespace.namespace2.search.paths += /search_path3
namespace.namespace2.permitted.paths = /permitted_path1
namespace.namespace2.permitted.paths += /permitted_path2
namespace.namespace2.permitted.paths += /permitted_path3
namespace.namespace2.asan.search.paths = /search_path1
namespace.namespace2.asan.search.paths += /data/asan/search_path1
namespace.namespace2.asan.search.paths += /search_path2
namespace.namespace2.asan.permitted.paths = /permitted_path1
namespace.namespace2.asan.permitted.paths += /data/asan/permitted_path1
namespace.namespace2.asan.permitted.paths += /permitted_path2
)";

constexpr const char* kSectionWithOneNamespaceExpectedResult =
    R"([test_section]
namespace.default.isolated = false
namespace.default.search.paths = /search_path1
namespace.default.search.paths += /search_path2
namespace.default.search.paths += /search_path3
namespace.default.permitted.paths = /permitted_path1
namespace.default.permitted.paths += /permitted_path2
namespace.default.permitted.paths += /permitted_path3
namespace.default.asan.search.paths = /search_path1
namespace.default.asan.search.paths += /data/asan/search_path1
namespace.default.asan.search.paths += /search_path2
namespace.default.asan.permitted.paths = /permitted_path1
namespace.default.asan.permitted.paths += /data/asan/permitted_path1
namespace.default.asan.permitted.paths += /permitted_path2
)";

constexpr const char* kSectionBinaryPathExpectedResult =
    R"(dir.test_section = binary_path1
dir.test_section = binary_path2
dir.test_section = binary_path3
)";

TEST(linkerconfig_section, section_with_namespaces) {
  ConfigWriter writer;

  std::vector<std::shared_ptr<Namespace>> namespaces;

  namespaces.push_back(CreateNamespaceWithLinks("default", true, true,
                                                "namespace1", "namespace2"));
  namespaces.push_back(CreateNamespaceWithLinks("namespace1", false, false,
                                                "default", "namespace2"));
  namespaces.push_back(CreateNamespaceWithPaths("namespace2", false, false));

  BinaryPathList empty_list;

  Section section("test_section", empty_list, namespaces);

  section.WriteConfig(writer);
  auto config = writer.ToString();
  ASSERT_EQ(config, kSectionWithNamespacesExpectedResult);
}

TEST(linkerconfig_section, section_with_one_namespace) {
  android::linkerconfig::modules::ConfigWriter writer;

  std::vector<std::shared_ptr<Namespace>> namespaces;
  namespaces.push_back(CreateNamespaceWithPaths("default", false, false));

  BinaryPathList empty_list;

  Section section("test_section", empty_list, namespaces);
  section.WriteConfig(writer);
  auto config = writer.ToString();
  ASSERT_EQ(config, kSectionWithOneNamespaceExpectedResult);
}

TEST(linkerconfig_section, binary_paths) {
  BinaryPathList binary_paths = {{"binary_path2", kLowPriority},
                                 {"binary_path3", kLowPriority + 10},
                                 {"binary_path1", kDefaultPriority}};
  std::vector<std::shared_ptr<Namespace>> empty_namespace;
  Section section("test_section", binary_paths, empty_namespace);

  android::linkerconfig::modules::BinaryPathMap paths;
  section.CollectBinaryPaths(paths);

  std::string binary_path_output = "";
  for (auto& item : paths) {
    binary_path_output += item.second + "\n";
  }

  ASSERT_EQ(binary_path_output, kSectionBinaryPathExpectedResult);
}