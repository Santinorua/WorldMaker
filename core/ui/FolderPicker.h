#pragma once

#include <string>

struct GLFWwindow;

namespace WorldMaker::ui {

// Cancellation leaves folder and error unchanged.
bool PickExportFolder(GLFWwindow* owner, std::string& folder, std::string& error);

}
