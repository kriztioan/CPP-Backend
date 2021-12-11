#include "SpectralFitter.h"

SpectralFitter::SpectralFitter(std::vector<double> &spectrum,
                               std::vector<std::vector<double>> &pool)
    : _spectrum(spectrum), _pool(pool) {}

void SpectralFitter::fitSpectrum() {

  _weights.clear();

  _indices.clear();

  unsigned int npool = _pool.size();
  unsigned int nspectrum = _spectrum.size();

  double **nnlsMatrix = new double *[npool];

  double yArray[nspectrum];

  if (_uncertainties.size() == nspectrum) { // NNLC
    for (size_t i = 0; i < npool; i++) {

      nnlsMatrix[i] = new double[nspectrum];

      for (size_t j = 0; j < nspectrum; j++) {

        nnlsMatrix[i][j] = _pool[i][j] / _uncertainties[j];
      }
    }

    for (size_t i = 0; i < nspectrum; i++) {

      yArray[i] = _spectrum[i] / _uncertainties[i];
    }
  } else { // NNLS
    for (size_t i = 0; i < npool; i++) {

      nnlsMatrix[i] = new double[nspectrum];

      for (size_t j = 0; j < nspectrum; j++) {

        nnlsMatrix[i][j] = _pool[i][j];
      }
    }

    for (size_t i = 0; i < nspectrum; i++) {

      yArray[i] = _spectrum[i];
    }
  }

  double X[npool], ZZ[nspectrum], W[npool];

  int idx[npool];

  nnls(nnlsMatrix, nspectrum, npool, yArray, X, &_norm, W, ZZ, idx);

  for (const auto &value : X) {

    _weights.push_back(value);
  }

  for (size_t i = 0; i < npool; i++) {

    delete[] nnlsMatrix[i];
  }

  delete[] nnlsMatrix;
}

const std::vector<unsigned int> &SpectralFitter::getNonZeroIndices() {

  if (_indices.size() > 0) {

    return _indices;
  }

  size_t i = 0;
  for (auto value : _weights) {

    if (value > 0) {

      _indices.push_back(i);
    }

    ++i;
  }

  return _indices;
}