#ifndef PAHEMISSIONMODEL_H_
#define PAHEMISSIONMODEL_H_

#include "Exception.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

class PAHEmissionModel {

public:
  static double _energy;

  static double _frequency;

  static double _nc;

  static bool _approximate;

  static double solveInitialTemperatureFunc(double temperature,
                                            void *transitions);

  static double solveApproximateInitialTemperatureFunc(double temperature,
                                                       void *nc);

  static double integralOverHeatCapacity(double temperature, void *transitions);

  static double heatCapacity(double temperature, void *transitions);

  static double featureStrength(double temperature, void *transitions);

  static double approximateFeatureStrength(double temperature, void *charge);

  static void convertFromFrequencyToWavelength(std::vector<double> &grid);

  static void convertFromFrequencyToWavelength(std::array<double, 2> &grid);

  static void convertFromWavelengthToFrequency(std::vector<double> &grid);

  static void convertFromFrequencyToWavelength(
      std::vector<std::vector<std::pair<double, double>>> &transitions);

  static void convertFromWavelengthToFrequency(
      std::vector<std::vector<std::pair<double, double>>> &transitions);

  void useApproximate(std::vector<int> &charges, std::vector<int> &carbons);

  PAHEmissionModel();

  PAHEmissionModel(
      const std::vector<std::vector<std::pair<double, double>>> &transitions);

  void setTransitions(
      const std::vector<std::vector<std::pair<double, double>>> &transitions);

  void getTransitions(
      std::vector<std::vector<std::pair<double, double>>> &transitions);

  void shiftTransitions(double shift);

  void setGrid(const std::vector<double> &grid);

  void makeGrid(const std::vector<double> &frange, double step);

  void makeGrid(double fmin, double fmax, double step);

  std::vector<double> const &getGrid() const;

  void printTransitions();

  void applyCascadeWithEnergy(double energy, std::vector<double> &temperatures);

  void applyTemperatureWithEnergy(double energy,
                                  std::vector<double> &temperatures);

  void applyBlackbodyWithTemperature(double temperature);

  void
  applyBlackbodyWithTemperatureForEach(const std::vector<double> &temperatures);

  void getSpectraAndConvolveWithLorentianOfFHWM(
      std::vector<std::vector<double>> &vector, double fwhm = 15);

  void getSpectraAndConvolveWithGaussianOfFHWM(
      std::vector<std::vector<double>> &vector, double fwhm = 15);

  void
  getSpectraAndConvolveWithDrudeOfFHWM(std::vector<std::vector<double>> &vector,
                                       double fwhm = 15);

private:
  static constexpr double TemperatureMin = 2.73;

  static constexpr double TemperatureMax = 5000.0;

  static constexpr double RootAccuracy = 1e-4;

  static constexpr double IntegrationAccuracy = 1e-4;

  static constexpr int MaxIterations = 150;

  static constexpr int MaxSteps = 100;

  static constexpr double PlanckConstant = 6.6260693000000018e-27;

  static constexpr double SpeedOfLight = 29979245800.0;

  static constexpr double BoltzmannConstant = 1.3806504e-16;

  static constexpr double AvogadrosNumber = 6.0221415e+23;

  std::vector<int> _charges;

  std::vector<int> _carbons;

  std::vector<std::vector<std::pair<double, double>>> _transitions;

  std::vector<double> _grid;

  double _fmin;

  double _fmax;

  double
  solveInitialTemperature(double energy,
                          std::vector<std::pair<double, double>> &transitions);

  double Lorentzian(double frequency, double centroid, double hwhm);

  double Gaussian(double frequency, double centroid, double sigma);

  double Drude(double frequency, double centroid, double fwhm);

  double Blackbody(double frequency, double temperature);
};

inline void PAHEmissionModel::makeGrid(const std::vector<double> &frange,
                                       double step) {
  makeGrid(frange[0], frange[1], step);
}

inline std::vector<double> const &PAHEmissionModel::getGrid() const {
  return (_grid);
}

inline double PAHEmissionModel::Lorentzian(double frequency, double centroid,
                                           double hwhm) {
  return ((1.0 / M_PI) * hwhm / (pow(frequency - centroid, 2) + pow(hwhm, 2)));
}

inline double PAHEmissionModel::Gaussian(double frequency, double centroid,
                                         double sigma) {
  return ((1.0 / (sigma * sqrt(2.0 * M_PI))) *
          exp(-pow(frequency - centroid, 2) / (2.0 * pow(sigma, 2))));
}

inline double PAHEmissionModel::Drude(double frequency, double centroid,
                                      double fwhm) {
  return ((2.0 / (fwhm * M_PI)) * pow(fwhm / centroid, 2) /
          (pow(centroid / frequency - frequency / centroid, 2) +
           pow(fwhm / centroid, 2)));
}

inline double PAHEmissionModel::Blackbody(double frequency,
                                          double temperature) {
  return (
      (2.0 * PlanckConstant * SpeedOfLight * SpeedOfLight * pow(frequency, 3)) /
      (exp(PlanckConstant * SpeedOfLight * frequency /
           (BoltzmannConstant * temperature)) -
       1.0));
}

inline void PAHEmissionModel::useApproximate(std::vector<int> &charges,
                                             std::vector<int> &carbons) {
  _approximate = true;
  _charges = charges;
  _carbons = carbons;
}

#endif /* PAHEMISSIONMODEL_H_ */
