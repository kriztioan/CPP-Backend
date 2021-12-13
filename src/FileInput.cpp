#include "FileInput.h"

FileInput::FileInput(const std::string_view filename) : _filename(filename) {}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
FileInput::readFileFromDisk(FileInput::Filetype filetype) {

  switch (filetype) {

  case FileInput::Filetype::ASCII:

    return readASCIIFileFromDisk();
    break;

  case FileInput::Filetype::YAAAR:

    return readYAAARFileFromDisk();
    break;

  case FileInput::Filetype::VOTABLE:

    return readVOTableFileFromDisk();
    break;

  case FileInput::Filetype::GUESS:

    return readGuessedFileFromDisk();
    break;

  default:

    throw(Exception("Unable to resolve filetype or filetype not supported"));
  }
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
FileInput::readASCIIFileFromDisk() {

  std::vector<double> xdata;
  xdata.reserve(64);

  std::vector<double> ydata;
  ydata.reserve(64);

  std::vector<double> yerrdata;
  yerrdata.reserve(64);

  int column = 1;

  char c;

  std::string value;

  double d;

  char *endptr;

  std::ifstream ifstr;

  ifstr.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {

    ifstr.open(_filename, std::ios::in);

    while (ifstr.get(c).good()) {

      if (isdigit(c) || c == '.' || c == '-' || c == '+') {

        value += c;
      } else if (c == '#') {

        while (ifstr.get(c).good()) {

          if (c == '\n') {
            break;
          }
        }
      } else if (isspace(c)) {

        if (!value.empty()) {

          d = strtod(value.c_str(), &endptr);

          if (*endptr != '\0') {

            throw Exception("Unable to convert string to double");
          }

          if (column > 3) {

            throw Exception("Extraneous number of columns");
          } else if (column == 1) {

            xdata.push_back(1e4 / d);
          } else if (column == 2) {

            ydata.push_back(d);
          } else if (column == 3) {

            yerrdata.push_back(d);
          }

          if (c == '\r' || c == '\n') {
            column = 1;
          } else {

            ++column;
          }

          value.erase();
        }
      } else {

        throw Exception("Extraneous data");
      }
    }
  } catch (const std::ifstream::failure &e) {

    if (!ifstr.eof()) {

      throw Exception(e.what());
    }
  } catch (const Exception &e) {

    throw;
  }

  return std::make_tuple(xdata, ydata, yerrdata);
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
FileInput::readYAAARFileFromDisk() {

  std::vector<double> xdata;
  xdata.reserve(64);

  std::vector<double> ydata;
  ydata.reserve(64);

  std::vector<double> yerrdata;
  yerrdata.reserve(64);

  CCfits::FITS::setVerboseMode(true);

  std::unique_ptr<CCfits::FITS> fits;

  try {

    fits = std::make_unique<CCfits::FITS>(_filename, CCfits::Read);

    CCfits::ExtHDU &table = fits->extension(1);

    table.column(1).read(xdata, 1, table.column(1).rows());

    table.column(2).read(ydata, 1, table.column(2).rows());

    table.column(3).read(yerrdata, 1, table.column(3).rows());

  } catch (CCfits::FitsException &e) {

    throw Exception(e.message());
  }

  for (auto &x : xdata) {

    x = 1e4 / x;
  }

  return std::make_tuple(xdata, ydata, yerrdata);
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
FileInput::readVOTableFileFromDisk() {

  std::vector<double> xdata;
  xdata.reserve(64);

  std::vector<double> ydata;
  ydata.reserve(64);

  std::vector<double> yerrdata;
  yerrdata.reserve(64);

  double d;

  char *endptr;

  tinyxml2::XMLDocument doc;

  if (doc.LoadFile(_filename.c_str()) != tinyxml2::XML_SUCCESS) {

    throw Exception(doc.ErrorStr());
  }

  tinyxml2::XMLElement *root = doc.RootElement();

  if (strcmp(root->Name(), "VOTABLE") != 0) {

    throw Exception("Incorrect document root");
  }

  tinyxml2::XMLElement *resource = root->FirstChildElement("RESOURCE");

  if (nullptr == resource) {

    throw Exception("VOTABLE has no resource");
  }

  tinyxml2::XMLElement *table = resource->FirstChildElement("TABLE");

  if (nullptr == table) {

    throw Exception("VOTABLE resource has no table");
  }

  tinyxml2::XMLElement *data = table->FirstChildElement("DATA");

  if (nullptr == data) {

    throw Exception("VOTABLE resource table has no data");
  }

  tinyxml2::XMLElement *tabledata = data->FirstChildElement("TABLEDATA");

  if (nullptr == tabledata) {

    throw Exception("VOTABLE resource table data has no tabledata");
  }

  tinyxml2::XMLElement *tr = tabledata->FirstChildElement();

  while (true) {

    if (strcmp(tr->Name(), "TR") != 0) {

      throw Exception("VOTABLE resource table data tabledata expects row");
    }

    tinyxml2::XMLElement *td = tr->FirstChildElement();

    if (strcmp(td->Name(), "TD") != 0) {

      throw Exception(
          "VOTABLE resource table data tabledata row expects table data");
    }

    d = strtod(td->GetText(), &endptr);

    if (*endptr != '\0') {

      throw Exception("Unable to convert string to double");
    }

    xdata.push_back(1e4 / d);

    td = td->NextSiblingElement();

    if (strcmp(td->Name(), "TD") != 0) {

      throw Exception(
          "VOTABLE resource table data tabledata row expects table data");
    }

    d = strtod(td->GetText(), &endptr);

    if (*endptr != '\0') {

      throw Exception("Unable to convert string to double");
    }

    ydata.push_back(d);

    tr = tr->NextSiblingElement();

    if (nullptr == tr) {

      break;
    }
  }

  return std::make_tuple(xdata, ydata, yerrdata);
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
FileInput::readGuessedFileFromDisk() {

  std::unique_ptr<CCfits::FITS> fits;

  try {

    fits = std::make_unique<CCfits::FITS>(_filename);
  } catch (CCfits::FitsError &e) {

    std::ifstream ifstr(_filename);

    std::string line;

    getline(ifstr, line);

    if (line.compare(0, 5, "<?xml") == 0) {

      return FileInput::readVOTableFileFromDisk();
    } else {

      return FileInput::readASCIIFileFromDisk();
    }
  }

  return FileInput::readYAAARFileFromDisk();
}
