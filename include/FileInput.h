#ifndef FILEINPUT_H_
#define FILEINPUT_H_

#include "Exception.h"
#include <CCfits/CCfits>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <tinyxml2.h>
#include <tuple>
#include <vector>

class FileInput {

public:
  enum class Filetype : char { ASCII, YAAAR, VOTABLE, GUESS };

  FileInput(std::string_view filename);

  void setFilename(std::string_view filename);

  std::string_view getFilename() const;

  std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
  readFileFromDisk(Filetype filetype = Filetype::GUESS);

  std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
  readASCIIFileFromDisk();

  std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
  readYAAARFileFromDisk();

  std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
  readVOTableFileFromDisk();

  std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
  readGuessedFileFromDisk();

private:
  std::string _filename;
};

inline void FileInput::setFilename(const std::string_view filename) {
  _filename = filename;
}

inline std::string_view FileInput::getFilename() const { return (_filename); }

#endif /* FILEINPUT_H_ */
