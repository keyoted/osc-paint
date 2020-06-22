#include "ResourcePath.hpp"
#include <iostream>

// HACK: This was hacked together, originally it served to get the resources
// from mac's executable package thing.

const std::string rsrfolder{"resources"};
std::filesystem::path _resourcePath{rsrfolder};

void setResourcePathBeguin(std::filesystem::path pth) {
    _resourcePath = pth.remove_filename();
    _resourcePath /= rsrfolder;
}

std::string resourcePath(void) {
    return _resourcePath.string() + '/';
}
