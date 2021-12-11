#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "../config.h"
#include "Exception.h"
#include <tclap/CmdLine.h>
#define MYSQLPP_SSQLS_NO_STATICS
#include "PAHEmissionModel.h"
#include "PAHdb.h"
#include <cstdlib>

class Parameters {

public:
  enum class Arg : char {
    ZeroKelvin,
    FixedTemperature,
    Temperature,
    Cascade,
    CoAdd,
    Stack,
    TemperatureStack,
    CompareExperimentWithTheory,
    SpectralFit,
    Wavelength,
    Frequency,
    Lorentzian,
    Gaussian,
    Drude
  };

  Parameters();
  Parameters(const int argc, const char *argv[]);

  void parse(const int argc, const char *argv[]);

  std::string_view getDatabase() const;
  std::string_view getHost() const;
  std::string_view getUsername() const;
  std::string_view getPassword() const;
  std::string_view getSocket() const;
  std::string_view getOutputFilename() const;
  std::string_view getInputFilename() const;

  const double &getFWHM() const;
  const double &getShift() const;
  double getEnergyInCGS() const;
  const double &getTemperatureInKelvin() const;

  const PAHdb::Database &getTable() const;
  const int &getPort() const;
  const int &getTimeout() const;
  const Arg &getModel() const;
  const Arg &getProfile() const;
  const Arg &getTool() const;
  const Arg &getUnits() const;
  const int &getDatabaseVersion() const;

  std::vector<int> const &getIds() const;

  std::array<int, 2> const &getPlotSize() const;

  void setIds(const std::vector<int> &ids);

  void setPlotLimits(const std::array<double, 2> &plotlimits);

  std::array<double, 2> const &getPlotLimits() const;

  std::array<double, 2> const &getPlotLimitsAsFrequencies() const;

  std::array<double, 2> const &getPlotLimitsAsWavelenghts() const;

  std::vector<double> const &getTemperaturesInKelvin() const;

  std::vector<double> const &getWeights() const;

  bool isCompress();
  bool isX11();
  bool isPNG();
  bool isJPEG();
  bool isPostscript();

private:
  TCLAP::CmdLine _cmdline;

  std::string _database;
  std::string _host;
  std::string _username;
  std::string _password;
  std::string _socket;
  std::string _output;
  std::string _input;

  double _fwhm;
  double _shift;
  double _energy;
  double _temperature;

  PAHdb::Database _table;
  int _port;
  int _timeout;
  Arg _model;
  Arg _profile;
  Arg _tool;
  Arg _units;
  int _databaseversion;

  bool _compress;
  bool _x11;
  bool _png;
  bool _jpeg;
  bool _postscript;

  std::vector<double> _temperatures;

  std::vector<double> _weights;

  std::array<double, 2> _plotlimits;

  std::array<double, 2> _invplotlimits;

  std::vector<int> _ids;

  std::array<int, 2> _plotsize;
};

inline std::string_view Parameters::getDatabase() const { return (_database); }

inline std::string_view Parameters::getHost() const { return (_host); }

inline std::string_view Parameters::getUsername() const { return (_username); }

inline std::string_view Parameters::getPassword() const { return (_password); }

inline std::string_view Parameters::getSocket() const { return (_socket); }

inline std::string_view Parameters::getOutputFilename() const {
  return (_output);
}

inline std::string_view Parameters::getInputFilename() const {
  return (_input);
}

inline const Parameters::Arg &Parameters::getProfile() const {
  return (_profile);
}

inline const double &Parameters::getFWHM() const { return (_fwhm); }

inline const double &Parameters::getShift() const { return (_shift); }

inline double Parameters::getEnergyInCGS() const {
  return (1.6021765e-12 * _energy);
}

inline const double &Parameters::getTemperatureInKelvin() const {
  return (_temperature);
}

inline const PAHdb::Database &Parameters::getTable() const { return (_table); }

inline const int &Parameters::getPort() const { return (_port); }

inline const int &Parameters::getTimeout() const { return (_timeout); }

inline bool Parameters::isCompress() { return (_compress); }

inline bool Parameters::isX11() { return (_x11); }

inline bool Parameters::isPNG() { return (_png); }

inline bool Parameters::isJPEG() { return (_jpeg); }

inline bool Parameters::isPostscript() { return (_postscript); }

inline std::vector<int> const &Parameters::getIds() const { return (_ids); }

inline std::array<int, 2> const &Parameters::getPlotSize() const {
  return (_plotsize);
}

inline void Parameters::setIds(const std::vector<int> &ids) { _ids = ids; }

inline void Parameters::setPlotLimits(const std::array<double, 2> &plotlimits) {
  _plotlimits = plotlimits;
}

inline std::array<double, 2> const &Parameters::getPlotLimits() const {
  return (_plotlimits);
}

inline std::array<double, 2> const &
Parameters::getPlotLimitsAsFrequencies() const {
  return (_units == Arg::Frequency ? _plotlimits : _invplotlimits);
}

inline std::array<double, 2> const &
Parameters::getPlotLimitsAsWavelenghts() const {
  return (_units == Arg::Wavelength ? _plotlimits : _invplotlimits);
}

inline std::vector<double> const &Parameters::getTemperaturesInKelvin() const {
  return (_temperatures);
}

inline std::vector<double> const &Parameters::getWeights() const {
  return (_weights);
}

inline const Parameters::Arg &Parameters::getModel() const { return (_model); }

inline const Parameters::Arg &Parameters::getTool() const { return (_tool); }

inline const Parameters::Arg &Parameters::getUnits() const { return (_units); }

inline const int &Parameters::getDatabaseVersion() const {
  return (_databaseversion);
}

#endif /* PARAMETERS_H_ */
