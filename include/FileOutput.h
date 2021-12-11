#ifndef FILEOUTPUT_H_
#define FILEOUTPUT_H_

#include "Exception.h"

#include <fstream>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>

class FileOutput {

public:
  FileOutput(const std::string_view filename);

  ~FileOutput();

  void setFilename(const std::string_view filename);

  std::string_view getFilename() const;

  void writeTableHeaderToDisk(const std::vector<std::string> &header);
  void writeTableToDisk(const std::vector<std::vector<double>> &table);

private:
  std::string _filename;

  std::ofstream _ofstr;
};

inline void FileOutput::setFilename(std::string_view filename) {
  _filename = filename;
}

inline std::string_view FileOutput::getFilename() const { return (_filename); }

#endif /* FILEOUTPUT_H_ */
