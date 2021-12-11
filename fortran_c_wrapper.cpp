#include "PAHEmissionModel.h"

extern "C" {

void fortran_c_wrapper_(double *_frequencies, double *_intensities,
                        double *energy, double *_new, size_t *size, double *T);
}

void fortran_c_wrapper_(double *_frequencies, double *_intensities,
                        double *energy, double *_new, size_t *size, double *T) {

  PAHEmissionModel::PAHEmissionModel pahemissionmodel;

  std::vector<std::vector<std::pair<double, double>>> transitions;

  std::vector<std::pair<double, double>> single;

  for (size_t i = 0; i < *size; i++)
    single.push_back(
        std::pair<double, double>(_frequencies[i], _intensities[i]));

  transitions.push_back(single);

  pahemissionmodel.setTransitions(transitions);

  std::vector<double> temperatures;

  pahemissionmodel.applyCascadeWithEnergy(*energy, temperatures);

  pahemissionmodel.getTransitions(transitions);

  *T = temperatures.at(0);

  size_t i = 0;
  for (std::vector<std::pair<double, double>>::iterator it =
           transitions.at(0).begin();
       it != transitions.at(0).end(); ++it, i++)
    _new[i] = it->second;
}
