#include "PAHEmissionModel.h"

double PAHEmissionModel::_energy;

double PAHEmissionModel::_frequency;

double PAHEmissionModel::_nc;

bool PAHEmissionModel::_approximate = false;

PAHEmissionModel::PAHEmissionModel()
    : _transitions(std::vector<std::vector<std::pair<double, double>>>(0)),
      _grid(std::vector<double>(0)) {}

PAHEmissionModel::PAHEmissionModel(
    const std::vector<std::vector<std::pair<double, double>>> &transitions)
    : _transitions(transitions), _grid(std::vector<double>(0)) {}

void PAHEmissionModel::setGrid(const std::vector<double> &grid) {

  _grid = grid;

  _fmin = *min_element(_grid.begin(), _grid.end());

  _fmax = *max_element(_grid.begin(), _grid.end());
}

void PAHEmissionModel::makeGrid(double fmin, double fmax, double step) {

  if (fmin > fmax) {

    double tmp = fmin;

    fmin = fmax;

    fmax = tmp;
  }

  _fmin = fmin;

  _fmax = fmax;

  _grid.clear();

  while (fmin < fmax) {

    _grid.push_back(fmin);

    fmin += step;
  }

  if (_grid.back() != fmax) {

    _grid.push_back(fmax);
  }
}

void PAHEmissionModel::setTransitions(
    const std::vector<std::vector<std::pair<double, double>>> &transitions) {

  _transitions = transitions;
}

void PAHEmissionModel::getTransitions(
    std::vector<std::vector<std::pair<double, double>>> &transitions) {

  transitions = _transitions;
}

void PAHEmissionModel::printTransitions() {

  std::cout.setf(std::ios::right);

  std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(5)
            << std::showpoint;

  for (const auto &transition : _transitions) {

    for (const auto &t : transition) {

      std::cout << std::setw(10) << t.first << '\t' << std::setw(10) << t.second
                << '\n';
    }

    std::cout << std::endl;
  }
}

void PAHEmissionModel::getSpectraAndConvolveWithLorentianOfFHWM(
    std::vector<std::vector<double>> &vector, double fwhm) {

  vector.clear();

  double hwhm = fwhm / 2.0;

  for (auto &transition : _transitions) {

    std::vector<double> intensities(_grid.size());

    for (auto &t : transition) {

      if (t.second == 0 || t.first < (_fmin - 22.0 * hwhm) ||
          t.first > (_fmax + 22.0 * hwhm)) {
        continue;
      }

      int i = 0;

      for (const auto &g : _grid) {

        intensities.at(i++) += t.second * Lorentzian(g, t.first, hwhm);
      }
    }

    vector.push_back(intensities);
  }
}

void PAHEmissionModel::getSpectraAndConvolveWithGaussianOfFHWM(
    std::vector<std::vector<double>> &vector, double fwhm) {

  vector.clear();

  vector.reserve(_transitions.size());

  double sigma = fwhm / 2.0 / sqrt(2.0 * log(2.0));

  for (auto &transition : _transitions) {

    std::vector<double> intensities(_grid.size());

    for (auto &t : transition) {

      if (t.second == 0 || t.first < (_fmin - 3.0 * sigma) ||
          t.first > (_fmax + 3.0 * sigma)) {
        continue;
      }

      int i = 0;

      for (const auto &g : _grid) {

        intensities.at(i++) += t.second * Gaussian(g, t.first, sigma);
      }
    }

    vector.push_back(intensities);
  }
}

void PAHEmissionModel::getSpectraAndConvolveWithDrudeOfFHWM(
    std::vector<std::vector<double>> &vector, double fwhm) {

  vector.clear();

  vector.reserve(_transitions.size());

  for (auto &transition : _transitions) {

    std::vector<double> intensities(_grid.size());

    for (auto &t : transition) {

      if (t.second == 0 || t.first < (_fmin - 8.0 * fwhm) ||
          t.first > (_fmax + 8.0 * fwhm)) {
        continue;
      }

      int i = 0;

      for (const auto &g : _grid) {

        intensities.at(i++) += t.second * Drude(g, t.first, fwhm);
      }
    }

    vector.push_back(intensities);
  }
}

void PAHEmissionModel::applyBlackbodyWithTemperature(double temperature) {

  for (auto &transition : _transitions) {

    for (auto &t : transition) {

      t.second *=
          1e5 * 4.0 * M_PI * Blackbody(t.first, temperature) / AvogadrosNumber;
    }
  }
}

void PAHEmissionModel::applyBlackbodyWithTemperatureForEach(
    const std::vector<double> &temperatures) {

  size_t i = 0;

  for (auto &transition : _transitions) {

    for (auto &t : transition) {

      t.second *= 1e5 * 4.0 * M_PI * Blackbody(t.first, temperatures[i]) /
                  AvogadrosNumber;
    }
    ++i;
  }
}

void PAHEmissionModel::applyTemperatureWithEnergy(
    double energy, std::vector<double> &temperatures) {

  temperatures.clear();

  temperatures.reserve(_transitions.size());

  int j = 0;

  for (auto &transition : _transitions) {

    if (_approximate) {

      _nc = static_cast<double>(_carbons.at(j++));
    }

    temperatures.push_back(solveInitialTemperature(energy, transition));
  }

  applyBlackbodyWithTemperatureForEach(temperatures);
}

void PAHEmissionModel::applyCascadeWithEnergy(
    double energy, std::vector<double> &temperatures) {

  temperatures.clear();

  temperatures.reserve(_transitions.size());

  double TemperatureMax;

  double error;

  double strength;

  double factor;

  gsl_integration_workspace *w = gsl_integration_workspace_alloc(MaxSteps);

  gsl_function F;

  if (!_approximate) {

    F.function = PAHEmissionModel::featureStrength;
  } else {

    F.function = PAHEmissionModel::approximateFeatureStrength;
  }

  int j = 0;

  for (auto &transition : _transitions) {

    std::vector<std::pair<double, double>> transitions(transition.cbegin(),
                                                       transition.cend());

    if (!_approximate) {

      factor = 1.0;

      F.params = &transitions;
    } else {

      _nc = static_cast<double>(_carbons.at(j));

      factor = 2.48534271218563e-23 * _nc /
               std::accumulate(
                   transitions.begin(), transitions.end(), 0.0,
                   [](const auto &a, const auto &b) { return a + b.second; });

      F.params = &_charges.at(j++);
    }

    TemperatureMax = solveInitialTemperature(energy, transitions);

    temperatures.push_back(TemperatureMax);

    for (auto &t : transition) {

      if (t.second == 0.0) {

        continue;
      }

      PAHEmissionModel::_frequency = t.first;

      gsl_integration_qag(&F, TemperatureMin, TemperatureMax, 0,
                          IntegrationAccuracy, MaxSteps, GSL_INTEG_GAUSS61, w,
                          &strength, &error);

      t.second *= factor * pow(t.first, 3) * strength;
    }
  }

  gsl_integration_workspace_free(w);
}

double PAHEmissionModel::featureStrength(double temperature,
                                         void *transitions_p) {

  std::vector<double> sum;

  std::vector<std::pair<double, double>> *transitions =
      static_cast<std::vector<std::pair<double, double>> *>(transitions_p);

  sum.reserve(transitions->size());

  double val;

  for (auto &t : *transitions) {

    val = PlanckConstant * SpeedOfLight * t.first /
          (BoltzmannConstant * temperature);

    sum.push_back(t.second * pow(t.first, 3) / (exp(val) - 1.0));
  }

  val = PlanckConstant * SpeedOfLight * _frequency /
        (BoltzmannConstant * temperature);

  return ((PAHEmissionModel::heatCapacity(temperature, transitions_p) /
           (exp(val) - 1.0)) *
          (1.0 / (std::accumulate(sum.begin(), sum.end(), 0.0))));
}

double PAHEmissionModel::approximateFeatureStrength(double temperature,
                                                    void *charge) {

  double a = 0.0, b = 0.0;

  if (*static_cast<int *>(charge) != 0) {
    if (temperature > 1000.0) {
      a = 4.8e-4;
      b = 1.6119;
    } else if (temperature > 300.0 && temperature <= 1000.0) {
      a = 6.38e-7;
      b = 2.5556;
    } else if (temperature > 100.0 && temperature <= 300.0) {
      a = 1.69e-12;
      b = 4.7687;
    } else if (temperature > 40.0 && temperature <= 100.0) {
      a = 7.70e-9;
      b = 2.9244;
    } else if (temperature > 20.0 && temperature <= 40.0) {
      a = 3.47e-12;
      b = 5.0428;
    } else if (temperature > 2.7 && temperature <= 20.0) {
      a = 4.47e-19;
      b = 10.3870;
    }
  } else {
    if (temperature > 270.0) {
      a = 5.52e-7;
      b = 2.5270;
    } else if (temperature > 200.0 && temperature <= 270.0) {
      a = 1.70e-9;
      b = 3.5607;
    } else if (temperature > 60.0 && temperature <= 200.0) {
      a = 1.35e-11;
      b = 4.4800;
    } else if (temperature > 30.0 && temperature <= 60.0) {
      a = 4.18e-8;
      b = 2.5217;
    } else if (temperature > 2.7 && temperature <= 30.0) {
      a = 1.88e-16;
      b = 8.1860;
    }
  }

  double val = 1.4387751297850830401 * _frequency / temperature;

  if (a <= 0.0 || val > log(DBL_MAX)) {

    return 0.0;
  }

  return 1.0 / ((exp(val) - 1.0) * a * pow(temperature, b));
}

double PAHEmissionModel::solveInitialTemperature(
    double energy, std::vector<std::pair<double, double>> &transitions) {

  PAHEmissionModel::_energy = energy;

  size_t niterations = 0;

  int status = 0;

  const gsl_root_fsolver_type *T;

  gsl_root_fsolver *s;

  double temperature = 0;

  gsl_function F;

  if (!_approximate) {

    F.function = PAHEmissionModel::solveInitialTemperatureFunc;

    F.params = &transitions;
  } else {

    F.function = PAHEmissionModel::solveApproximateInitialTemperatureFunc;

    F.params = &_nc;
  }

  T = gsl_root_fsolver_brent;

  s = gsl_root_fsolver_alloc(T);

  double min = TemperatureMin;

  double max = TemperatureMax;

  gsl_root_fsolver_set(s, &F, min, max);

  do {

    ++niterations;

    status = gsl_root_fsolver_iterate(s);

    temperature = gsl_root_fsolver_root(s);

    min = gsl_root_fsolver_x_lower(s);

    max = gsl_root_fsolver_x_upper(s);

    status = gsl_root_test_interval(min, max, 0, RootAccuracy);
  } while (status == GSL_CONTINUE && niterations < MaxIterations);

  gsl_root_fsolver_free(s);

  return (temperature);
}

inline double PAHEmissionModel::solveInitialTemperatureFunc(double temperature,
                                                            void *transitions) {
  return integralOverHeatCapacity(temperature, transitions) - _energy;
}

inline double
PAHEmissionModel::solveApproximateInitialTemperatureFunc(double temperature,
                                                         void *nc) {

  return *static_cast<double *>(nc) *
             (7.54267e-11 * erf(-4.989231 + 0.41778 * log(temperature)) +
              7.542670e-11) -
         _energy;
}

double PAHEmissionModel::integralOverHeatCapacity(double temperature,
                                                  void *transitions) {

  gsl_integration_workspace *w = gsl_integration_workspace_alloc(MaxSteps);

  double energy;

  double error;

  gsl_function F;

  F.function = PAHEmissionModel::heatCapacity;

  F.params = transitions;

  gsl_integration_qag(&F, TemperatureMin, temperature, 0, IntegrationAccuracy,
                      MaxSteps, GSL_INTEG_GAUSS61, w, &energy, &error);

  gsl_integration_workspace_free(w);

  return (energy);
}

double PAHEmissionModel::heatCapacity(double temperature, void *transitions_p) {

  double heatcapacity = 0.0;

  double value;

  std::vector<std::pair<double, double>> *transitions =
      static_cast<std::vector<std::pair<double, double>> *>(transitions_p);

  for (const auto &t : *transitions) {

    value = PlanckConstant * SpeedOfLight * t.first /
            (BoltzmannConstant * temperature);

    heatcapacity +=
        BoltzmannConstant * exp(-value) * pow((value / (1.0 - exp(-value))), 2);
  }

  return (heatcapacity);
}

void PAHEmissionModel::shiftTransitions(double shift) {

  for (auto &transition : _transitions) {

    for (auto &t : transition) {

      t.first += shift;
    }
  }
}

void PAHEmissionModel::convertFromFrequencyToWavelength(
    std::vector<double> &grid) {

  for (auto &g : grid) {

    g = 1e4 / g;
  }
}

void PAHEmissionModel::convertFromFrequencyToWavelength(
    std::array<double, 2> &grid) {

  for (auto &g : grid) {

    g = 1e4 / g;
  }
}

inline void
PAHEmissionModel::convertFromWavelengthToFrequency(std::vector<double> &grid) {

  convertFromFrequencyToWavelength(grid);
}

void PAHEmissionModel::convertFromFrequencyToWavelength(
    std::vector<std::vector<std::pair<double, double>>> &transitions_p) {

  for (auto &transition : transitions_p) {

    for (auto &t : transition) {

      t.first = 1e4 / t.first;
    }
  }
}

inline void PAHEmissionModel::convertFromWavelengthToFrequency(
    std::vector<std::vector<std::pair<double, double>>> &transitions) {
  convertFromFrequencyToWavelength(transitions);
}
