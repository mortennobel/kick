//
// Created by Morten Nobel-Jørgensen on 18/04/15.
//

#include "nativedialog.h"
#include "tinyfiledialogs.h"
#include <sstream>

// http://stackoverflow.com/a/236803/420250
namespace {
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }
    
    
    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }
}

bool kick::NativeDialog::message(std::string aTitle , /* "" */
        std::string aMessage , /* "" may contain \n and \t */
        DialogType dialogType  ,
        IconType infoIcon ,
        int const aDefaultButton ) {
    char const* aDialogType;
    switch (dialogType){
        case DialogType::ok:
            aDialogType = "ok";
            break;
        case DialogType::okcancel:
            aDialogType = "okcancel";
            break;
        default:
            aDialogType = "yesno";
            break;
    }
    char const * aIconType;
    switch (infoIcon){
        case IconType::info:
            aIconType = "info";
            break;
        case IconType::error:
            aIconType = "error";
            break;
        case IconType::question:
            aIconType = "question";
            break;
        default:
            //case IconType::warning:
            aIconType = "warning";
            break;
    }
    return tinyfd_messageBox(aTitle.c_str() ,
            aMessage.c_str() ,
            aDialogType ,
            aIconType ,
            aDefaultButton );
}

std::unique_ptr<std::string> kick::NativeDialog::input(std::string title, std::string message, std::string defaultInput) {
    assert(message.find('\n')==std::string::npos);
    assert(message.find('\t')==std::string::npos);
    char const * res = tinyfd_inputBox (title.c_str(), message.c_str(), defaultInput.c_str());
    return std::unique_ptr<std::string>(res?new std::string(res): nullptr);
}

std::unique_ptr<std::string> kick::NativeDialog::inputPassword(std::string title, std::string message) {
    assert(message.find('\n')==std::string::npos);
    assert(message.find('\t')==std::string::npos);
    char const * res = tinyfd_inputBox (title.c_str(), message.c_str(), nullptr);
    return std::unique_ptr<std::string>(res?new std::string(res): nullptr);
}

std::unique_ptr<std::string> kick::NativeDialog::saveFile(std::string title, std::string aDefaultPathAndFile, std::vector<std::string> filters) {
    std::vector<char const *> filtersConverted;
    for (auto & f : filters){
        filtersConverted.push_back(f.c_str());
    }
    char const * res = tinyfd_saveFileDialog(title.c_str(), aDefaultPathAndFile.c_str(), filters.size(), filtersConverted.data());
    return std::unique_ptr<std::string>(res?new std::string(res): nullptr);
}

std::vector<std::string> kick::NativeDialog::openFile(std::string title, std::string aDefaultPathAndFile, std::vector<std::string> filters, bool allowMultipleSelects) {
    std::vector<char const *> filtersConverted;
    for (auto & f : filters){
        filtersConverted.push_back(f.c_str());
    }
    char const * resStr = tinyfd_openFileDialog(title.c_str(), aDefaultPathAndFile.c_str(), filters.size(), filtersConverted.data(), allowMultipleSelects);
    if (resStr){
        return split(resStr, '|');
    }
    return std::vector<std::string>();
}

std::unique_ptr<std::string> kick::NativeDialog::selectFolder(std::string title, std::string aDefaultPathAndFile) {
    char const * res = tinyfd_selectFolderDialog(title.c_str(), aDefaultPathAndFile.c_str());
    return std::unique_ptr<std::string>(res?new std::string(res): nullptr);
}

std::unique_ptr<glm::vec3> kick::NativeDialog::colorChooser(std::string title, glm::vec3 defaultRGB) {
    unsigned char aDefaultRGB[3];
    unsigned char resRGB[3];
    for (int i=0;i<3;i++){
        aDefaultRGB[i] = (unsigned char)round(defaultRGB[i]*255);
    }
    char const * res = tinyfd_colorChooser(title.c_str(), nullptr, aDefaultRGB, resRGB);
    if (res){
        glm::vec3* c = new glm::vec3();
        for (int i=0;i<3;i++){
            (*c)[i] = resRGB[i]/255.0f;
        }
        return std::unique_ptr<glm::vec3>(c);
    }
    return std::unique_ptr<glm::vec3>();
}