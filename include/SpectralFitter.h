#ifndef SPECTRALFITTER_H_
#define SPECTRALFITTER_H_

#include "Exception.h"
#include "NNLS.h"
#include <iostream>
#include <vector>

class SpectralFitter {

public:
  SpectralFitter() = default;

  SpectralFitter(std::vector<double> &spectrum,
                 std::vector<std::vector<double>> &pool);

  void setSpectrum(std::vector<double> &spectrum);

  void setUncertainties(std::vector<double> &uncertainties);

  void setPool(std::vector<std::vector<double>> &pool);

  void fitSpectrum();

  const std::vector<double> &getWeights() const;

  const std::vector<unsigned int> &getNonZeroIndices();

  const double &getNorm() const;

private:
  std::vector<double> _spectrum;

  std::vector<double> _uncertainties;

  std::vector<std::vector<double>> _pool;

  std::vector<double> _weights;

  std::vector<unsigned int> _indices;

  double _norm;
};

inline void SpectralFitter::setSpectrum(std::vector<double> &spectrum) {

  _spectrum = spectrum;
}

inline void
SpectralFitter::setUncertainties(std::vector<double> &uncertainties) {

  _uncertainties = uncertainties;
}

inline void SpectralFitter::setPool(std::vector<std::vector<double>> &pool) {

  _pool = pool;
}

inline const std::vector<double> &SpectralFitter::getWeights() const {

  return _weights;
}

inline const double &SpectralFitter::getNorm() const { return _norm; }

#endif /* SPECTRALFITTER_H_ */
