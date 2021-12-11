#include "Parameters.h"

Parameters::Parameters()
    : _cmdline(PROGRAM_DESCRIPTION, ' ', PROGRAM_VERSION) {}

Parameters::Parameters(const int argc, const char *argv[])
    : _cmdline(PROGRAM_DESCRIPTION, ' ', PROGRAM_VERSION) {

  parse(argc, argv);
}

void Parameters::parse(const int argc, const char *argv[]) {

  try {

    TCLAP::ValueArg<std::string> database("", "database", "MySQL database",
                                          false, "pahdb", "string", _cmdline);

    std::vector<std::string> constraint{"Theory", "Experiment"};

    TCLAP::ValuesConstraint<std::string> tableconstraint(constraint);

    TCLAP::ValueArg<std::string> table("", "table", "MySQL table", false,
                                       "Theory", &tableconstraint, _cmdline);

    TCLAP::ValueArg<std::string> host("", "host", "MySQL host", false,
                                      "localhost", "string", _cmdline);

    TCLAP::ValueArg<std::string> username("", "username", "MySQL username",
                                          false, "root", "string", _cmdline);

    TCLAP::ValueArg<std::string> password("", "password", "MySQL password",
                                          false, "root", "string", _cmdline);

    TCLAP::ValueArg<int> port("", "port", "MySQL port", false, 3306, "integer",
                              _cmdline);

    TCLAP::SwitchArg compress("", "compress", "MySQL compression", _cmdline);

    TCLAP::ValueArg<int> timeout("", "timeout", "MySQL timeout", false, 60,
                                 "integer", _cmdline);

    TCLAP::ValueArg<std::string> socket("", "socket", "MySQL socket", false,
                                        "/var/run/mysqld/mysqld.sock", "string",
                                        _cmdline);

    TCLAP::ValueArg<int> databaseversion("", "databaseversion",
                                         "Database version", false, 99,
                                         "integer", _cmdline);

    TCLAP::ValueArg<std::string> output("", "output", "Output file", false,
                                        "default", "string", _cmdline);

    TCLAP::ValueArg<std::string> input("", "input", "Input file", false, "",
                                       "string", _cmdline);

    TCLAP::ValueArg<std::string> ids("", "ids", "Species identifier(s)", true,
                                     "", "comma separeted list of integers",
                                     _cmdline);

    TCLAP::ValueArg<std::string> plotsize(
        "", "plotsize", "Plot size", false, "650,325",
        "comma separeted list of 2 integers", _cmdline);

    constraint.clear();

    constraint.clear();
    constraint.insert(constraint.begin(), {"ZeroKelvin", "FixedTemperature",
                                           "Temperature", "Cascade"});

    TCLAP::ValuesConstraint<std::string> modelconstraint(constraint);

    TCLAP::ValueArg<std::string> model("", "model", "Emission model", false,
                                       "ZeroKelvin", &modelconstraint,
                                       _cmdline);

    constraint.clear();

    constraint.insert(constraint.begin(),
                      {"CoAdd", "Stack", "TemperatureStack",
                       "CompareExperimentWithTheory", "SpectralFit"});

    TCLAP::ValuesConstraint<std::string> toolconstraint(constraint);

    TCLAP::ValueArg<std::string> tool("", "tool", "Tool", false, "CoAdd",
                                      &toolconstraint, _cmdline);

    constraint.clear();

    constraint.insert(constraint.begin(), {"Lorentzian", "Gaussian", "Drude"});

    TCLAP::ValuesConstraint<std::string> profileconstraint(constraint);

    TCLAP::ValueArg<std::string> profile("", "profile", "Emission profile",
                                         false, "Lorentzian",
                                         &profileconstraint, _cmdline);

    TCLAP::ValueArg<double> fwhm(
        "", "fwhm", "Full-width-half-maximum for applied emission profile",
        false, 15.0, "double", _cmdline);

    TCLAP::ValueArg<double> shift("", "shift", "Spectral shift", false, 0.0,
                                  "double", _cmdline);

    TCLAP::ValueArg<double> energy("", "energy", "Energy for model [eV]", false,
                                   5, "double", _cmdline);

    TCLAP::ValueArg<double> temperature("", "temperature",
                                        "Temperature for Blackbody [Kelvin]",
                                        false, 550, "double", _cmdline);

    TCLAP::ValueArg<std::string> temperatures(
        "", "temperatures", "Temperatures for Blackbodies [Kelvin]", false,
        "10,100,1000", "comma separeted list of doubles", _cmdline);

    TCLAP::ValueArg<std::string> weights(
        "", "weights", "Weights to apply for co-add", false, "",
        "comma separated list of doubles", _cmdline);

    TCLAP::ValueArg<std::string> plotlimits(
        "", "plotlimits", "Plot limits", false, "4000,1",
        "Comma separated list of two doubles", _cmdline);

    constraint.clear();

    constraint.insert(constraint.begin(), {"Frequency", "Wavelength"});

    TCLAP::ValuesConstraint<std::string> unitsconstraint(constraint);

    TCLAP::ValueArg<std::string> units("", "units", "Plot units", false,
                                       "Frequency", &unitsconstraint, _cmdline);

    TCLAP::SwitchArg x11("", "x11", "Output to screen", _cmdline, false);

    TCLAP::SwitchArg nopng("", "nopng", "Disable output to PNG file", _cmdline,
                           false);

    TCLAP::SwitchArg jpeg("", "jpeg", "Output to a JPEG file", _cmdline, false);

    TCLAP::SwitchArg postscript("", "postscript", "Output to a Postscript file",
                                _cmdline, false);

    _cmdline.parse(argc, argv);

    _database = database.getValue();

    if (table.getValue() == "Theory") {

      _table = PAHdb::Database::Theory;
    } else if (table.getValue() == "Experiment") {

      _table = PAHdb::Database::Experiment;
    }

    _host = host.getValue();

    _username = username.getValue();

    _password = password.getValue();

    _port = port.getValue();

    _compress = compress.getValue();

    _timeout = timeout.getValue();

    _socket = socket.getValue();

    _databaseversion = databaseversion.getValue();

    _output = output.getValue();

    _input = input.getValue();

    std::string_view tmp = ids.getValue();

    std::string strvalue;

    auto it = tmp.begin();

    int ivalue;

    char *endp;

    while (true) {

      if (*it == ',' || it == tmp.end()) {

        ivalue = strtol(strvalue.c_str(), &endp, 10);

        if (strvalue.size() == 0 || *endp != '\0') {

          throw(Exception(std::string(
              "PARSE ERROR: Argument (--ids)\n             Value '" + strvalue +
              "' does not meet constraint: integer")));
        }

        _ids.push_back(ivalue);

        strvalue.clear();
      } else {

        strvalue += *it;
      }

      if (it == tmp.end()) {

        break;
      }
      ++it;
    };

    tmp = plotsize.getValue();

    it = tmp.begin();

    size_t i = 0;

    while (i < 2) {

      if (*it == ',' || it == tmp.end()) {

        ivalue = strtol(strvalue.c_str(), &endp, 10);

        if (strvalue.size() == 0 || *endp != '\0') {

          throw(Exception(std::string(
              "PARSE ERROR: Argument (--plotsize)\n             Value '" +
              strvalue + "' does not meet constraint: int")));
        }

        _plotsize[i++] = ivalue;

        strvalue.clear();
      } else {

        strvalue += *it;
      }

      if (it == tmp.end()) {

        break;
      }
      ++it;
    };

    if (_plotsize.size() != 2) {

      throw(Exception(
          (std::string(
               "PARSE ERROR: Argument (--plotsize)\n             Value '") +
           tmp.data() + "' does not meet constraint: two integers")));
    }

    if (model.getValue() == "ZeroKelvin") {

      _model = Parameters::Arg::ZeroKelvin;
    } else if (model.getValue() == "FixedTemperature") {

      _model = Parameters::Arg::FixedTemperature;
    } else if (model.getValue() == "Temperature") {

      _model = Parameters::Arg::Temperature;
    }

    else if (model.getValue() == "Cascade") {

      _model = Parameters::Arg::Cascade;
    }

    if (tool.getValue() == "CoAdd") {

      _tool = Parameters::Arg::CoAdd;
    } else if (tool.getValue() == "Stack") {

      _tool = Parameters::Arg::Stack;
    } else if (tool.getValue() == "TemperatureStack") {

      _tool = Parameters::Arg::TemperatureStack;
    }

    else if (tool.getValue() == "CompareExperimentWithTheory") {

      _tool = Parameters::Arg::CompareExperimentWithTheory;
    } else if (tool.getValue() == "SpectralFit") {
      _tool = Parameters::Arg::SpectralFit;
    }

    if (profile.getValue() == "Lorentzian") {

      _profile = Parameters::Arg::Lorentzian;
    } else if (profile.getValue() == "Gaussian") {

      _profile = Parameters::Arg::Gaussian;
    } else if (profile.getValue() == "Drude") {

      _profile = Parameters::Arg::Drude;
    }

    _fwhm = fwhm.getValue();

    _shift = shift.getValue();

    _energy = energy.getValue();

    _temperature = temperature.getValue();

    double dvalue;

    tmp = temperatures.getValue();

    if (tmp.size() > 0) {

      it = tmp.begin();

      while (true) {

        if (*it == ',' || it == tmp.end()) {

          dvalue = strtod(strvalue.c_str(), &endp);

          if (strvalue.size() == 0 || *endp != '\0') {

            throw(Exception(
                std::string("PARSE ERROR: Argument (--temperatures)\n          "
                            "   Value '" +
                            strvalue + "' does not meet constraint: double")));
          }

          _temperatures.push_back(dvalue);

          strvalue.clear();
        } else {

          strvalue += *it;
        }

        if (it == tmp.end()) {

          break;
        }
        ++it;
      };
    }

    tmp = weights.getValue();

    if (tmp.size() > 0) {

      it = tmp.begin();

      while (true) {

        if (*it == ',' || it == tmp.end()) {

          dvalue = strtod(strvalue.c_str(), &endp);

          if (strvalue.size() == 0 || *endp != '\0') {

            throw(Exception(std::string(
                "PARSE ERROR: Argument (--weights)\n             Value '" +
                strvalue + "' does not meet constraint: double")));
          }

          _weights.push_back(dvalue);

          strvalue.clear();
        } else {

          strvalue += *it;
        }

        if (it == tmp.end()) {

          break;
        }
        ++it;
      };
    } else {

      _weights = std::vector<double>(_ids.size(), 1.0);
    }

    tmp = plotlimits.getValue();

    it = tmp.begin();

    i = 0;

    while (i < 2) {

      if (*it == ',' || it == tmp.end()) {

        dvalue = strtod(strvalue.c_str(), &endp);

        if (strvalue.size() == 0 || *endp != '\0') {

          throw(Exception(std::string(
              "PARSE ERROR: Argument (--plotlimits)\n             Value '" +
              strvalue + "' does not meet constraint: double")));
        }

        _plotlimits[i++] = dvalue;

        strvalue.clear();
      } else {

        strvalue += *it;
      }

      if (it == tmp.end()) {

        break;
      }
      ++it;
    };

    if (_plotlimits.size() != 2) {

      throw(Exception(
          std::string(
              "PARSE ERROR: Argument (--plotlimits)\n             Value '") +
          tmp.data() + "' does not meet constraint: two doubles"));
    }

    if (units.getValue() == "Frequency")
      _units = Parameters::Arg::Frequency;
    else if (units.getValue() == "Wavelength")
      _units = Parameters::Arg::Wavelength;

    _invplotlimits = _plotlimits;

    _invplotlimits.at(0) = 1e4 / _plotlimits.at(0);

    _invplotlimits.at(1) = 1e4 / _plotlimits.at(1);

    _x11 = x11.getValue();

    _png = !nopng.getValue();

    _jpeg = jpeg.getValue();

    _postscript = postscript.getValue();
  } catch (const TCLAP::ArgException &e) {

    throw(Exception(e.what()));
  }
}
