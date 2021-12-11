
#include "../config.h"
#include "Canvas.h"
#include "FileInput.h"
#include "FileOutput.h"
#include "MinMax.h"
#include "PAHEmissionModel.h"
#include <exception>
#include <new>
#define MYSQLPP_SSQLS_NO_STATICS
#include "PAHdb.h"
#include "Parameters.h"
#include "SpectralFitter.h"
#include "ctime"
#include "iostream"

/** Define own memory handler */
void new_handler() {

  std::set_new_handler(nullptr);

  time_t t = std::time(nullptr);

  struct tm *tm_s = std::localtime(&t);

  char str_t[32];

  std::strftime(str_t, sizeof(str_t), "%Y%m%d-%X - EXCEPTION: ", tm_s);

  std::cerr << str_t << "Memory allocation failed";

  std::abort();
}

/**
 * Program entry point
 *
 * @param argc  Program argument count
 * @param argv  Program arguments
 *
 */
int main(const int argc, const char *argv[], char ** /* envp */) {

  std::set_new_handler(new_handler);

  Parameters parameters;

  try {

    parameters.parse(argc, argv);
  } catch (const Exception &e) {

    std::cerr << e.what() << std::endl;

    return (1);
  }

  PAHdb pahdb;

  try {

    pahdb.connect(parameters.getDatabase(), parameters.getHost(),
                  parameters.getUsername(), parameters.getPassword(),
                  parameters.getPort(), parameters.isCompress(),
                  parameters.getTimeout(), parameters.getSocket());

    pahdb.setTable(parameters.getTable());

    pahdb.setProgress(10.0);
  } catch (const Exception &e) {

    std::cerr << e.what() << std::endl;

    return (1);
  }

  std::vector<std::vector<std::pair<double, double>>> transitions;

  std::vector<std::vector<double>> spectra;

  std::vector<double> grid;

  std::vector<double> temperatures;

  std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>
      inputdata;

  PAHEmissionModel pahemissionmodel;

  SpectralFitter spectralfitter;

  try {

    if (parameters.getTool() == Parameters::Arg::CompareExperimentWithTheory) {

      transitions = pahdb.getExperimentalAndTheoreticalTransitionsFromId(
          parameters.getIds().at(0));
    } else {

      if (parameters.getTool() == Parameters::Arg::SpectralFit) {

        FileInput fileinput(parameters.getInputFilename());

        inputdata = fileinput.readFileFromDisk();

        pahemissionmodel.setGrid(std::get<0>(inputdata));

        std::array<double, 2> inputlimits;

        inputlimits[0] = *max_element(std::get<0>(inputdata).begin(),
                                      std::get<0>(inputdata).end());

        inputlimits[1] = *min_element(std::get<0>(inputdata).begin(),
                                      std::get<0>(inputdata).end());

        if (parameters.getUnits() == Parameters::Arg::Wavelength) {

          PAHEmissionModel::convertFromFrequencyToWavelength(inputlimits);
        }

        parameters.setPlotLimits(inputlimits);

        spectralfitter.setSpectrum(std::get<1>(inputdata));

        spectralfitter.setUncertainties(std::get<2>(inputdata));

      } else {

        pahemissionmodel.makeGrid(parameters.getPlotLimitsAsFrequencies().at(0),
                                  parameters.getPlotLimitsAsFrequencies().at(1),
                                  2.0);
      }

      grid = pahemissionmodel.getGrid();

      if (parameters.getTool() != Parameters::Arg::TemperatureStack) {

        pahemissionmodel.setTransitions(
            pahdb.getTransitionsFromIds(parameters.getIds()));
      } else {

        transitions = pahdb.getTransitionsFromId(parameters.getIds().at(0));

        transitions = std::vector<std::vector<std::pair<double, double>>>(
            parameters.getTemperaturesInKelvin().size(), transitions.at(0));

        pahemissionmodel.setTransitions(transitions);
      }

      switch (parameters.getModel()) {

      case Parameters::Arg::FixedTemperature:

        if (parameters.getTool() == Parameters::Arg::TemperatureStack) {

          pahemissionmodel.applyBlackbodyWithTemperatureForEach(
              parameters.getTemperaturesInKelvin());
        } else {

          pahemissionmodel.applyBlackbodyWithTemperature(
              parameters.getTemperatureInKelvin());
        }

        break;

      case Parameters::Arg::Temperature:

        pahemissionmodel.applyTemperatureWithEnergy(parameters.getEnergyInCGS(),
                                                    temperatures);

        break;

      case Parameters::Arg::Cascade:

        pahemissionmodel.applyCascadeWithEnergy(parameters.getEnergyInCGS(),
                                                temperatures);

        break;

      case Parameters::Arg::ZeroKelvin:
      default:

        break;
      };

      if (parameters.getShift() != 0.0) {

        pahemissionmodel.shiftTransitions(parameters.getShift());
      }

      switch (parameters.getProfile()) {

      case Parameters::Arg::Gaussian:

        pahemissionmodel.getSpectraAndConvolveWithGaussianOfFHWM(
            spectra, parameters.getFWHM());

        break;
      case Parameters::Arg::Drude:

        pahemissionmodel.getSpectraAndConvolveWithDrudeOfFHWM(
            spectra, parameters.getFWHM());

        break;
      case Parameters::Arg::Lorentzian:
      default:

        pahemissionmodel.getSpectraAndConvolveWithLorentianOfFHWM(
            spectra, parameters.getFWHM());
        break;
      };
    }

  } catch (const Exception &e) {

    std::cerr << e.what() << std::endl;

    try {

      pahdb.setError(e.what());
    } catch (const Exception &e) {

      std::cerr << e.what() << std::endl;
    }

    return (1);
  }

  try {

    pahdb.setProgress(60.0);
  } catch (const Exception &e) {

    std::cerr << e.what() << std::endl;

    return (1);
  }

  std::vector<double> small;

  std::vector<double> large;

  std::vector<double> anion;

  std::vector<double> neutral;

  std::vector<double> cation;

  if (parameters.getTool() == Parameters::Arg::SpectralFit) {

    spectralfitter.setPool(spectra);

    spectralfitter.fitSpectrum();

    std::vector<unsigned int> indices = spectralfitter.getNonZeroIndices();

    std::vector<int> ids;

    for (const auto index : indices) {

      ids.push_back(parameters.getIds().at(index));
    }

    std::vector<sql_properties> prop =
        pahdb.getPropertiesByIDs(ids); // perhaps other name then 'prop'?

    small = std::vector<double>(grid.size(), 0.0);

    large = std::vector<double>(grid.size(), 0.0);

    anion = std::vector<double>(grid.size(), 0.0);

    neutral = std::vector<double>(grid.size(), 0.0);

    cation = std::vector<double>(grid.size(), 0.0);

    unsigned int j = 0;

    for (const auto &properties : prop) {

      unsigned int k = 0;

      if (properties.carbon > 50) {

        for (auto &l : large) {

          l += spectralfitter.getWeights().at(indices.at(j)) *
               spectra.at(indices.at(j)).at(k++);
        }
      } else {

        for (auto &s : small) {

          s += spectralfitter.getWeights().at(indices.at(j)) *
               spectra.at(indices.at(j)).at(k++);
        }
      }

      k = 0;

      if (properties.charge < 0) {

        for (auto &a : anion) {

          a += spectralfitter.getWeights().at(indices.at(j)) *
               spectra.at(indices.at(j)).at(k++);
        }
      } else if (properties.charge == 0) {

        for (auto &n : neutral) {

          n += spectralfitter.getWeights().at(indices.at(j)) *
               spectra.at(indices.at(j)).at(k++);
        }
      } else {

        for (auto &c : cation) {

          c += spectralfitter.getWeights().at(indices.at(j)) *
               spectra.at(indices.at(j)).at(k++);
        }
      }

      ++j;
    }
  }

  try {

    pahdb.setProgress(80.0);
  } catch (const Exception &e) {

    std::cerr << e.what() << std::endl;

    return (1);
  }

  Canvas canvas;

  canvas.setSize(parameters.getPlotSize());

  canvas.setColor("ffffff");

  canvas.setDefaultCharacterHeight(parameters.getPlotSize().at(1) * 2.0 /
                                   325.0);

  Panels panels;

  Plot plot;

  plot.setXLimits(parameters.getPlotLimits());

  plot.setDrawHorizontalFineGrid();

  plot.setDrawVerticalFineGrid();

  plot.getHorizontalGrid().setColor("dedede");

  plot.getHorizontalGrid().setLineStyle(Plot::Style::Dashed);

  plot.getVerticalGrid() = plot.getHorizontalGrid();

  plot.getYAxis().setWriteLabelsParalell();

  plot.getYAxis().setStyle(Axis::Style::WritePowerInFrame);

  std::string xtitle;

  std::string ytitle;

  switch (parameters.getUnits()) {

  case Parameters::Arg::Wavelength:

    xtitle = "#fiwavelength [micron]";

    if (parameters.getTool() != Parameters::Arg::CompareExperimentWithTheory) {

      PAHEmissionModel::convertFromFrequencyToWavelength(grid);
    } else {

      PAHEmissionModel::convertFromFrequencyToWavelength(transitions);
    }

    break;
  case Parameters::Arg::Frequency:
  default:

    xtitle = "#fifrequency [cm#u-1#d]";
  }

  if (parameters.getTool() == Parameters::Arg::CompareExperimentWithTheory) {

    ytitle = "#fiintegrated cross-section [#fnx#fi10#u5#d cm/mol]";
  } else if (parameters.getModel() == Parameters::Arg::ZeroKelvin) {

    ytitle = "#ficross-section [#fnx#fi10#u5#d cm#u2#d/mol]";
  } else if (parameters.getModel() == Parameters::Arg::FixedTemperature ||
             parameters.getModel() == Parameters::Arg::Temperature) {

    ytitle = "#fispectral radiance [#fnx#fi10#u5#d erg/s/cm#u-1#d/mol]";
  } else if (parameters.getModel() == Parameters::Arg::Cascade) {

    ytitle = "#firadiant energy [#fnx#fi10#u5#d erg/cm#u-1#d/mol]";
  }

  std::vector<Plot> plots;

  plots.reserve(2);

  Curve curve;

  curve.setFill();

  Line line;

  Point point;

  point.setSymbol(256);

  Text text;

  text.setJustification(Text::RightJustification);

  text.setColor("0000ff");

  std::vector<std::string> formulae;

  formulae.reserve(2);

  std::vector<PAHGeometry> geometries;

  geometries.reserve(2);

  std::vector<double> sum;

  std::ostringstream ostr;

  std::array<double, 2> margins;

  std::array<double, 2> newmargins;

  int i = 0, j = 0;

  std::array<int, 2> canvassize;

  switch (parameters.getTool()) {

  case Parameters::Arg::Stack:
  case Parameters::Arg::TemperatureStack:

    canvassize[0] = canvas.getSize().at(0);

    canvassize[1] = canvas.getSize().at(1) +
                    (spectra.size() - 1) * parameters.getPlotSize().at(1) *
                        (plot.getYMargins().at(1) - plot.getYMargins().at(0));

    canvas.setSize(canvassize);

    margins[0] = plot.getYMargins().at(0) * parameters.getPlotSize().at(1) /
                 canvas.getSize().at(1);

    margins[1] = 1.0 - ((1.0 - plot.getYMargins().at(1)) *
                        parameters.getPlotSize().at(1)) /
                           canvas.getSize().at(1);

    panels.setYMargins(margins);

    panels.setLayout(1, spectra.size());

    plot.getXAxis().setTitle(xtitle);

    formulae = pahdb.getFormulaeFromIds(parameters.getIds());

    geometries = pahdb.getGeometriesFromIds(parameters.getIds());

    std::for_each(geometries.begin(), geometries.end(),
                  std::mem_fun_ref(&PAHGeometry::diagonalize));

    for (const auto &spectrum : spectra) {

      // draw panel grid

      plot.setAdvance(false);

      plot.setDrawHorizontalFineGrid();

      plot.setDrawVerticalFineGrid();

      plot.getYAxis().setDrawConventionalAxis(false);

      plot.getYAxis().setDrawUnconventionalAxis(false);

      plot.getYAxis().setDrawConventionalLabels(false);

      plot.setYLimits(
          MinMax::min_max(spectrum, MinMax::Nice | MinMax::MaxExtraRoom));

      plots.push_back(plot);

      plot.clear();

      plot.setDrawHorizontalFineGrid(false);

      plot.setDrawVerticalFineGrid(false);

      plot.getXAxis().setDrawConventionalLabels(false);

      plot.getXAxis().setTitle("");

      // Draw panel structure

      if (parameters.getTool() == Parameters::Arg::Stack) {

        line.setColor("cccccc");

        line.setLineWidth(5);

        j = 0;

        for (const auto &bonds : geometries.at(i).getBonds()) {

          for (const auto &bond : bonds) {

            line.setStartCoordinates(
                (plot.getXLimits().at(1) + plot.getXLimits().at(0)) / 2.0 +
                    geometries.at(i)[j].getX() *
                        (plot.getXLimits().at(1) - plot.getXLimits().at(0)) /
                        (28.0 * parameters.getPlotSize().at(0) /
                         parameters.getPlotSize().at(1)),
                (plot.getYLimits().at(1) + plot.getYLimits().at(0)) / 2.0 +
                    geometries.at(i)[j].getY() *
                        (plot.getYLimits().at(1) - plot.getYLimits().at(0)) /
                        28.0,
                geometries.at(i)[j].getZ());

            line.setEndCoordinates(
                (plot.getXLimits().at(1) + plot.getXLimits().at(0)) / 2.0 +
                    geometries.at(i)[bond].getX() *
                        (plot.getXLimits().at(1) - plot.getXLimits().at(0)) /
                        (28.0 * parameters.getPlotSize().at(0) /
                         parameters.getPlotSize().at(1)),
                (plot.getYLimits().at(1) + plot.getYLimits().at(0)) / 2.0 +
                    geometries.at(i)[bond].getY() *
                        (plot.getYLimits().at(1) - plot.getYLimits().at(0)) /
                        28.0,
                geometries.at(i)[j].getZ());

            plot.addLine(line);
          }

          ++j;
        }

        for (const auto &atom : geometries.at(i)) {

          point.setCoordinates(
              (plot.getXLimits().at(1) + plot.getXLimits().at(0)) / 2.0 +
                  atom.getX() *
                      (plot.getXLimits().at(1) - plot.getXLimits().at(0)) /
                      (28.0 * parameters.getPlotSize().at(0) /
                       parameters.getPlotSize().at(1)),
              (plot.getYLimits().at(1) + plot.getYLimits().at(0)) / 2.0 +
                  atom.getY() *
                      (plot.getYLimits().at(1) - plot.getYLimits().at(0)) /
                      28.0,
              atom.getZ());

          point.setColor(atom.getCPKColor());

          point.setSize(atom.getSize() / 70.0);

          plot.addPoint(point);
        }

        plots.push_back(plot);

        plot.clear();
      }

      // Write panel formula

      if (parameters.getTool() != Parameters::Arg::TemperatureStack) {

        text.setText(Text::formatChemicalFormula(formulae.at(i)));

        text.setCoordinates(0.95, 0.85, 0.0, Text::CoordinateSystem::NORMAL);

        plot.addText(text);

        plots.push_back(plot);

        plot.clear();
      }

      // Write panel temperature

      if ((parameters.getModel() != Parameters::Arg::ZeroKelvin &&
           parameters.getModel() != Parameters::Arg::FixedTemperature) ||
          parameters.getTool() == Parameters::Arg::TemperatureStack) {

        ostr.str("");

        ostr << static_cast<int>(
                    parameters.getTool() == Parameters::Arg::TemperatureStack
                        ? parameters.getTemperaturesInKelvin().at(i)
                        : temperatures.at(i))
             << " K";

        text.setText(ostr.str());

        text.setCoordinates(0.95, 0.75, 0.0, Text::CoordinateSystem::NORMAL);

        plot.addText(text);

        plots.push_back(plot);

        plot.clear();
      }

      plot.getYAxis().setDrawConventionalAxis();

      plot.getYAxis().setDrawUnconventionalAxis();

      plot.getYAxis().setDrawConventionalLabels();

      // Draw panel spectrum

      curve.setXAndY(grid, spectra.at(i++));

      plot.addCurve(curve);

      plot.setAdvance(true);

      plots.push_back(plot);

      plot.clear();
    }

    panels.addPlots(plots);

    canvas.addPanels(panels);

    text.setColor("000000");

    text.setText(ytitle);

    text.setJustification(Text::CenterJustification);

    text.setAngle(90.0);

    text.setSize(1.0);

    text.setCoordinates(
        0.0723,
        panels.getYMargins().at(0) +
            (panels.getYMargins().at(1) - panels.getYMargins().at(0)) / 2.0,
        0.0);

    canvas.addText(text);

    break;
  case Parameters::Arg::CompareExperimentWithTheory:

    canvassize[0] = canvas.getSize().at(0);

    canvassize[1] = canvas.getSize().at(1) +
                    parameters.getPlotSize().at(1) *
                        (plot.getYMargins().at(1) - plot.getYMargins().at(0));

    canvas.setSize(canvassize);

    margins[0] = plot.getYMargins().at(0) * parameters.getPlotSize().at(1) /
                 canvas.getSize().at(1);

    margins[1] = 1.0 - ((1.0 - plot.getYMargins().at(1)) *
                        parameters.getPlotSize().at(1)) /
                           canvas.getSize().at(1);

    plot.setAdvance(false);

    plot.getXAxis().setDrawConventionalAxis(false);

    plot.getXAxis().setDrawUnconventionalAxis(false);

    plot.getXAxis().setDrawConventionalLabels(false);

    plot.getYAxis().setDrawConventionalAxis(false);

    plot.getYAxis().setDrawUnconventionalAxis(false);

    plot.getYAxis().setDrawConventionalLabels(false);

    plot.setYLimits(
        MinMax::min_max(transitions, MinMax::Nice | MinMax::MaxExtraRoom));

    newmargins[0] = margins[0];

    newmargins[1] = margins[0] + (margins[1] - margins[0]) / 2.0;

    plot.setYMargins(newmargins);

    plots.push_back(plot);

    plot.clear();

    newmargins[0] = newmargins[1];

    newmargins[1] = newmargins[1] + (margins[1] - margins[0]) / 2.0;

    plot.setYMargins(newmargins);

    plots.push_back(plot);

    plot.clear();

    formulae = pahdb.getFormulaeFromIds(parameters.getIds());

    geometries = pahdb.getGeometriesFromIds(parameters.getIds());

    std::for_each(geometries.begin(), geometries.end(),
                  std::mem_fun_ref(&PAHGeometry::diagonalize));

    plot.setDrawHorizontalFineGrid(false);

    plot.setDrawVerticalFineGrid(false);

    plot.setYMargins(margins);

    line.setColor("cccccc");

    line.setLineWidth(5);

    for (const auto &bonds : geometries.at(0).getBonds()) {

      for (const auto bond : bonds) {

        line.setStartCoordinates(
            (plot.getXLimits().at(1) + plot.getXLimits().at(0)) / 2.0 +
                geometries.at(0)[i].getX() *
                    (plot.getXLimits().at(1) - plot.getXLimits().at(0)) / 28.0,
            (plot.getYLimits().at(1) + plot.getYLimits().at(0)) / 2.0 +
                geometries.at(0)[i].getY() *
                    (plot.getYLimits().at(1) - plot.getYLimits().at(0)) / 28.0,
            geometries.at(0)[i].getZ());

        line.setEndCoordinates(
            (plot.getXLimits().at(1) + plot.getXLimits().at(0)) / 2.0 +
                geometries.at(0)[bond].getX() *
                    (plot.getXLimits().at(1) - plot.getXLimits().at(0)) / 28.0,
            (plot.getYLimits().at(1) + plot.getYLimits().at(0)) / 2.0 +
                geometries.at(0)[bond].getY() *
                    (plot.getYLimits().at(1) - plot.getYLimits().at(0)) / 28.0,
            geometries.at(0)[bond].getZ());

        plot.addLine(line);
      }

      ++i;
    }

    for (const auto &atom : geometries.at(0)) {

      point.setCoordinates(
          (plot.getXLimits().at(1) + plot.getXLimits().at(0)) / 2.0 +
              atom.getX() *
                  (plot.getXLimits().at(1) - plot.getXLimits().at(0)) / 28.0,
          (plot.getYLimits().at(1) + plot.getYLimits().at(0)) / 2.0 +
              atom.getY() *
                  (plot.getYLimits().at(1) - plot.getYLimits().at(0)) / 28.0,
          atom.getZ());

      point.setColor(atom.getCPKColor());

      point.setSize(atom.getSize() / 40.0);

      plot.addPoint(point);
    }

    text.setColor("0000ff");

    text.setJustification(Text::RightJustification);

    text.setText(Text::formatChemicalFormula(formulae.at(0)));

    text.setCoordinates(0.95, 0.95, 0.0, Text::CoordinateSystem::NORMAL);

    plot.addText(text);

    text.setSize(4);

    text.setJustification(Text::CenterJustification);

    text.setText("Theory");

    text.setCoordinates(0.5, 0.75, 0.0, Text::CoordinateSystem::NORMAL);

    plot.addText(text);

    text.setText("Experiment");

    text.setCoordinates(0.5, 0.25, 0.0, Text::CoordinateSystem::NORMAL);

    plot.addText(text);

    plots.push_back(plot);

    plot.clear();

    plot.getXAxis().setTitle(xtitle);

    plot.getXAxis().setDrawConventionalAxis();

    plot.getXAxis().setDrawConventionalLabels();

    plot.getYAxis().setDrawConventionalAxis();

    plot.getYAxis().setDrawUnconventionalAxis();

    plot.getYAxis().setDrawConventionalLabels();

    newmargins[0] = margins[0];

    newmargins[1] = margins[0] + (margins[1] - margins[0]) / 2.0;

    plot.setYMargins(newmargins);

    line.setLineWidth(4);

    line.setColor("ff0000");

    for (const auto &transition : transitions.at(0)) {

      line.setStartCoordinates(transition.first, 0.0, 0.0);

      line.setEndCoordinates(transition.first, transition.second, 0.0);

      plot.addLine(line);
    }

    plots.push_back(plot);

    plot.clear();

    newmargins[0] = newmargins[1];

    newmargins[1] = newmargins[1] + (margins[1] - margins[0]) / 2.0;

    plot.setYMargins(newmargins);

    plot.getXAxis().setTitle("");

    plot.getXAxis().setDrawZeroLine();

    plot.getXAxis().setDrawConventionalLabels(false);

    plot.getXAxis().setDrawUnconventionalAxis();

    for (const auto &transition : transitions.at(1)) {

      line.setStartCoordinates(transition.first, 0.0, 0.0);

      line.setEndCoordinates(transition.first, transition.second, 0.0);

      plot.addLine(line);
    }

    plots.push_back(plot);

    canvas.addPlots(plots);

    text.setColor("000000");

    text.setText(ytitle);

    text.setJustification(Text::CenterJustification);

    text.setAngle(90.0);

    text.setSize(1.0);

    text.setCoordinates(0.0723, margins[0] + (margins[1] - margins[0]) / 2.0,
                        0.0);

    canvas.addText(text);

    break;
  case Parameters::Arg::SpectralFit:

    curve.setFill(false);

    curve.setSymbol(9);

    curve.setXAndY(grid, std::get<1>(inputdata));

    plot.addCurve(curve);

    curve.setSymbol(0);

    curve.setColor("dedede");

    sum = std::vector<double>(spectra.front().size());

    for (auto &spectrum : spectra) {

      int j = 0;

      if (spectralfitter.getWeights().at(i) == 0.0) {

        ++i;

        continue;
      }

      for (auto &s : spectrum) {

        s *= spectralfitter.getWeights().at(i);

        sum.at(j++) += s;
      }

      curve.setXAndY(grid, spectrum);

      plot.addCurve(curve);

      ++i;
    }

    curve.setXAndY(grid, sum);

    curve.setColor("ff0000");

    curve.setLineWidth(2);

    plot.addCurve(curve);

    plot.setYLimits(MinMax::min_max(sum, MinMax::Nice));

    text.setJustification(Text::LeftJustification);

    text.setColor("ff0000");

    text.setText("total fit");

    text.setCoordinates(0.85, 0.90, 0.0, Text::CoordinateSystem::NORMAL);

    plot.addText(text);

    text.setColor("00ff00");

    text.setText("N#dC#u>50");

    text.setCoordinates(0.85, 0.83, 0.0, Text::CoordinateSystem::NORMAL);

    plot.addText(text);

    text.setColor("0000ff");

    text.setText("N#dC#u<50");

    text.setCoordinates(0.85, 0.76, 0.0, Text::CoordinateSystem::NORMAL);

    plot.addText(text);

    text.setColor("fff00f");

    text.setText("anion");

    text.setCoordinates(0.85, 0.69, 0.0, Text::CoordinateSystem::NORMAL);

    plot.addText(text);

    text.setColor("ff00ff");

    text.setText("neutral");

    text.setCoordinates(0.85, 0.62, 0.0, Text::CoordinateSystem::NORMAL);

    plot.addText(text);

    text.setColor("00ffff");

    text.setText("cation");

    text.setCoordinates(0.85, 0.55, 0.0, Text::CoordinateSystem::NORMAL);

    plot.addText(text);

    curve.setXAndY(grid, large);

    curve.setColor("00ff00");

    curve.setLineWidth(1);

    plot.addCurve(curve);

    curve.setXAndY(grid, small);

    curve.setColor("0000ff");

    plot.addCurve(curve);

    curve.setXAndY(grid, anion);

    curve.setColor("fff00f");

    plot.addCurve(curve);

    curve.setXAndY(grid, neutral);

    curve.setColor("ff00ff");

    plot.addCurve(curve);

    curve.setXAndY(grid, cation);

    curve.setColor("00ffff");

    plot.addCurve(curve);

    plot.getXAxis().setTitle(xtitle);

    plot.getYAxis().setTitle(ytitle);

    plots.push_back(plot);

    canvas.addPlots(plots);

    ostr.str("");

    ostr << "Euclidean distance norm: " << spectralfitter.getNorm();

    if (std::get<2>(inputdata).size() == std::get<1>(inputdata).size()) {
      ostr << " (NNLC; used provided uncertainties)";
    } else {
      ostr << " (NNLS)";
    }

    text.setColor("000000");

    text.setText(ostr.str());

    text.setCoordinates(0.15, 0.93, 0.0);

    canvas.addText(text);

    break;
  case Parameters::Arg::CoAdd:
  default:

    sum = std::vector<double>(spectra.front().size());

    for (const auto &spectrum : spectra) {

      int j = 0;

      for (auto s : spectrum) {

        sum.at(j++) += s * parameters.getWeights().at(i);
      }

      ++i;
    }

    curve.setXAndY(grid, sum);

    plot.addCurve(curve);

    plot.setYLimits(MinMax::min_max(sum, MinMax::Nice));

    plot.getXAxis().setTitle(xtitle);

    plot.getYAxis().setTitle(ytitle);

    plots.push_back(plot);

    canvas.addPlots(plots);

    break;
  };

  std::string f(parameters.getOutputFilename());

  FileOutput fileoutput(f.append(".dat"));

  spectra.insert(spectra.begin(), grid);

  time_t t = time(nullptr);

  std::vector<std::string> header{"NASA Ames PAH IR Spectroscopic Database",
                                  std::string("Date: ") + ctime(&t),
                                  "Version : " PROGRAM_VERSION,
                                  "Build: " PROGRAM_BUILD,
                                  std::string("First column: ") + xtitle,
                                  std::string("Other columns: ") + ytitle};

  if (parameters.getTool() == Parameters::Arg::SpectralFit) {

    header.insert(header.end(), {"First row: UIDs", "Second row: weights"});

    std::ostringstream uids;

    uids << std::right << std::setw(12) << 0 << '\t';

    std::vector<sql_properties> prop = pahdb.getPropertiesByIDs(
        parameters.getIds()); // perhaps other name then 'prop'?

    for (const auto &properties : prop) {

      uids << std::right << std::setw(12) << properties.uid << '\t';
    }

    header.insert(header.end(), {uids.str(), "First row: weights"});

    spectra.front().insert(spectra.front().begin(), 0);

    for (size_t i = 1; i < spectra.size(); i++) {

      spectra.at(i).insert(spectra.at(i).begin(),
                           spectralfitter.getWeights().at(i - 1));
    }
  }

  fileoutput.writeTableHeaderToDisk(header);

  fileoutput.writeTableToDisk(spectra);

  if (parameters.isX11()) {

    canvas.paintOnScreen();
  }

  if (parameters.isPNG()) {

    canvas.paintOnPNG(parameters.getOutputFilename());
  }

  if (parameters.isJPEG()) {

    canvas.paintOnJPEG(parameters.getOutputFilename());
  }

  if (parameters.isPostscript()) {
    canvas.paintOnPostscript(parameters.getOutputFilename());
  }

  try {

    pahdb.setProgress(100.0);
  } catch (const Exception &e) {

    std::cerr << e.what() << std::endl;

    return (1);
  }

  return (0);
}
