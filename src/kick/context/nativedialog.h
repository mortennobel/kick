//
// Created by Morten Nobel-Jørgensen on 18/04/15.
//


#pragma once

#include <string>
#include <vector>
#include <cassert>
#include "glm/glm.hpp"

namespace kick {
    // Cpp interface to tinyfiledialogs
    class NativeDialog {
    public:
        enum class DialogType {
            ok, okcancel, yesno
        };
        enum class IconType {
            info, warning, error, question
        };

        static bool message(std::string aTitle , /* "" */
                        std::string aMessage , /* "" may contain \n and \t */
                        DialogType dialogType = DialogType::ok ,
                        IconType infoIcon = IconType::info,
                        int const aDefaultButton = 0);

        static std::unique_ptr<std::string> input(std::string title, std::string message, std::string defaultInput = "");

        static std::unique_ptr<std::string> inputPassword(std::string title, std::string message);

        static std::unique_ptr<std::string> saveFile(std::string title, std::string aDefaultPathAndFile, std::vector<std::string> filters);

        static std::vector<std::string> openFile(std::string title, std::string aDefaultPathAndFile, std::vector<std::string> filters, bool allowMultipleSelects = false);

        static std::unique_ptr<std::string> selectFolder(std::string title, std::string aDefaultPathAndFile);

        static std::unique_ptr<glm::vec3> colorChooser(std::string title, glm::vec3 defaultRGB);
    };
}


