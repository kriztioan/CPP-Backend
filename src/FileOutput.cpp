#include "FileOutput.h"

FileOutput::FileOutput(std::string_view filename) : _filename(filename) {}

FileOutput::~FileOutput() {

  if (_ofstr.is_open()) {

    _ofstr.close();
  }
}

void FileOutput::writeTableHeaderToDisk(
    const std::vector<std::string> &header) {

  try {

    if (!_ofstr.is_open()) {

      _ofstr.open(_filename.c_str(), std::ios::out);
    }

    for (const auto &record : header) {

      _ofstr << record << '\n';
    }

    _ofstr.flush();
  } catch (const std::exception &e) {

    _ofstr.close();

    throw(Exception(e.what()));
  }
}

void FileOutput::writeTableToDisk(
    const std::vector<std::vector<double>> &table) {

  try {

    if (!_ofstr.is_open()) {

      _ofstr.open(_filename.c_str(), std::ios::out);
    }

    _ofstr.setf(std::ios::right);

    _ofstr << std::setprecision(5) << std::showpoint << std::scientific;

    size_t nj = table.size();

    size_t ni = table.begin()->size();

    try {

      for (size_t i = 0; i < ni; i++) {

        for (size_t j = 0; j < nj - 1; j++) {

          _ofstr << std::setw(12) << table.at(j).at(i) << '\t';
        }

        _ofstr << std::setw(12) << table.at(nj - 1).at(i) << '\n';
      }

      _ofstr.flush();
    } catch (const std::exception &e) {

      _ofstr.close();

      throw(Exception(e.what()));
    }
  } catch (const std::ofstream::failure &e) {

    throw(Exception(e.what()));
  }
}
