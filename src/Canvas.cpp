#include "Canvas.h"

Canvas::Canvas()
    : _color("000000"), _size({2, 300}), _defaultcharacterheight(2.25),
      _defaultticklength(3.0) {
  _wrappers.reserve(4);
}

void Canvas::addPlot(Plot &plot) {

  _wrappers.emplace_back(Wrapper::Type::W_Plot,
                         static_cast<Wrapper::Data>(&plot));
}

void Canvas::addPlots(std::vector<Plot> &plots) {

  for (auto &p : plots) {

    _wrappers.emplace_back(Wrapper::Type::W_Plot,
                           static_cast<Wrapper::Data>(&p));
  }
}

void Canvas::addText(Text &text) {

  _wrappers.emplace_back(Wrapper::Type::W_Text,
                         static_cast<Wrapper::Data>(&text));
}

void Canvas::addPanels(Panels &panels) {

  _wrappers.emplace_back(Wrapper::Type::W_Panels,
                         static_cast<Wrapper::Data>(&panels));
}

PLINT Canvas::lookUpColor(std::string_view hex) {

  auto index = std::find(_colormap.begin(), _colormap.end(), hex);

  PLINT entry = std::distance(_colormap.begin(), index);

  if (index == _colormap.end()) {

    _colormap.emplace_back(hex);

    std::stringstream ss;

    ss << std::hex << hex.substr(0, 6);

    int rgb;

    ss >> rgb;

    int a = 255;

    if (hex.size() == 8) {

      ss.str(std::string());

      ss.clear();

      ss << std::hex << hex.substr(6);

      ss >> a;
    }

    _plstream->scol0a(entry, (rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff,
                      static_cast<float>(a) / 255.0f);
  }

  return (entry);
}

void Canvas::paintOnScreen() {

  _plstream = std::make_unique<plstream>();

  _plstream->sdev("xwin");

  _plstream->spause(true);

  _plstream->spage(72.0, 72.0, _size.at(0), _size.at(1), 0.0, 0.0);

  paint();
}

void Canvas::paintOnPostscript(std::string_view filename) {

  _plstream = std::make_unique<plstream>();

  _plstream->sdev("epscairo");

  std::string f(filename);

  _plstream->sfnam(f.append(".ps").c_str());

  _plstream->spause(false);

  _plstream->spage(0.0, 0.0, _size.at(0), _size.at(1), 0.0, 0.0);

  _plstream->sdiori(1);

  _plstream->setopt("aspect", "1");

  paint();
}

void Canvas::paintOnPNG(std::string_view filename) {

  _plstream = std::make_unique<plstream>();

  _plstream->sdev("pngcairo");

  std::string f(filename);

  _plstream->sfnam(f.append(".png").c_str());

  _plstream->spause(false);

  _plstream->spage(0.0, 0.0, _size.at(0), _size.at(1), 0.0, 0.0);

  _defaultcharacterheight *= 1.5;

  paint();
}

void Canvas::paintOnJPEG(std::string_view filename) {

  _plstream = std::make_unique<plstream>();

  _plstream->sdev("cairojpeg");

  std::string f(filename);

  _plstream->sfnam(f.append(".jpeg").c_str());

  _plstream->spause(false);

  _plstream->spage(0.0, 0.0, _size.at(0), _size.at(1), 0.0, 0.0);

  _defaultcharacterheight *= 1.5;
  
  paint();
}

void Canvas::paint() {

  bool hadfirstpage = false;

  _plstream->scmap0n(64);

  _colormap.clear();

  lookUpColor(_color);

  _plstream->init();

  _plstream->fontld(1);

  _plstream->font(2);

  _plstream->schr(_defaultcharacterheight, 1.0);

  Panels *panels;

  Plot *plot;

  Text *text;

  std::array<double, 2> margins;

  for (auto &w : _wrappers) {

    switch (w.getType()) {

    case Wrapper::Type::W_Plot: {

      plot = static_cast<Plot *>(w.getData());

      if (plot->isAdvance() || !hadfirstpage) {

        _plstream->adv(0);

        hadfirstpage = true;
      }

      drawPlot(plot);
    } break;

    case Wrapper::Type::W_Panels: {
      if (!hadfirstpage) {

        _plstream->adv(0);

        hadfirstpage = true;
      }

      panels = static_cast<Panels *>(w.getData());

      margins[0] = panels->getYMargins().at(0);

      margins[1] = panels->getYMargins().at(0) +
                   (panels->getYMargins().at(1) - panels->getYMargins().at(0)) /
                       static_cast<double>(panels->getRows());

      for (auto &panel : *panels) {

        plot = static_cast<Plot *>(panel.getData());

        plot->setYMargins(margins);

        if (plot->isAdvance()) {

          margins[0] = margins[1];

          margins[1] = margins[1] + (panels->getYMargins().at(1) -
                                     panels->getYMargins().at(0)) /
                                        static_cast<double>(panels->getRows());
        }

        drawPlot(plot);
      }

    } break;
    case Wrapper::Type::W_Text: {

      text = static_cast<Text *>(w.getData());

      _plstream->vpor(0.0, 1.0, 0.0, 1.0);

      _plstream->wind(0.0, 1.0, 0.0, 1.0);

      _plstream->col0(lookUpColor(text->getColor()));

      _plstream->schr(_defaultcharacterheight, text->getSize());

      _plstream->ptex(text->getCoordinates().at(0),
                      text->getCoordinates().at(1),
                      cosf(M_PI * text->getAngle() / 180.0),
                      sinf(M_PI * text->getAngle() / 180.0),
                      text->getJustification(), text->getText().data());
    } break;
    default: {

      throw(Exception("Unsupported graphics type"));
    } break;
    }
  }
}

void Canvas::drawPlot(Plot *plot) {

  Point *point;

  Line *line;

  Curve *curve;

  Text *text;

  _plstream->vpor(plot->getXMargins().at(0), plot->getXMargins().at(1),
                  plot->getYMargins().at(0), plot->getYMargins().at(1));

  float xmin, xmax, ymin, ymax;

  xmin = plot->getXLimits().at(0);

  xmax = plot->getXLimits().at(1);

  ymin = plot->getYLimits().at(0);

  ymax = plot->getYLimits().at(1);

  _plstream->wind(xmin, xmax, ymin, ymax);

  _plstream->width(plot->getLineWidth());

  _plstream->lsty(static_cast<PLINT>(plot->getLineStyle()));

  if (plot->isDrawHorizontalGrid() || plot->isDrawHorizontalFineGrid()) {

    _plstream->col0(lookUpColor(plot->getHorizontalGrid().getColor()));

    _plstream->lsty(
        static_cast<PLINT>(plot->getHorizontalGrid().getLineStyle()));

    _plstream->width(plot->getHorizontalGrid().getLineWidth());

    if (plot->isDrawHorizontalFineGrid()) {

      _plstream->box("gh", 0.0, 0.0, "", 0.0, 0.0);
    } else {

      _plstream->box("g", 0.0, 0.0, "", 0.0, 0.0);
    }
  }

  if (plot->isDrawVerticalGrid() || plot->isDrawVerticalFineGrid()) {

    _plstream->col0(lookUpColor(plot->getVerticalGrid().getColor()));

    _plstream->lsty(static_cast<PLINT>(plot->getVerticalGrid().getLineStyle()));

    _plstream->width(plot->getVerticalGrid().getLineWidth());

    if (plot->isDrawVerticalFineGrid()) {

      _plstream->box("", 0, 0, "gh", 0, 0);
    } else {

      _plstream->box("", 0, 0, "g", 0, 0);
    }
  }

  for (auto &p : *plot) {

    switch (p.getType()) {

    case Wrapper::Type::W_Point: {

      point = static_cast<Point *>(p.getData());

      _plstream->col0(lookUpColor(point->getColor()));

      _plstream->width(plot->getLineWidth());

      _plstream->lsty(static_cast<PLINT>(plot->getLineStyle()));

      _plstream->ssym(_defaultcharacterheight, point->getSize());

      PLFLT px = point->getCoordinates().at(0);

      PLFLT py = point->getCoordinates().at(1);

      _plstream->sym(1, &px, &py, point->getSymbol());
    } break;
    case Wrapper::Type::W_Line: {

      line = static_cast<Line *>(p.getData());

      _plstream->col0(lookUpColor(line->getColor()));

      _plstream->lsty(static_cast<PLINT>(line->getLineStyle()));

      _plstream->width(line->getLineWidth());

      _plstream->join(
          line->getStartCoordinates().at(0), line->getStartCoordinates().at(1),
          line->getEndCoordinates().at(0), line->getEndCoordinates().at(1));
    } break;
    case Wrapper::Type::W_Curve: {

      curve = static_cast<Curve *>(p.getData());

      if (curve->getX().size() == 0) {

        continue;
      }

      std::unique_ptr<PLFLT[]> x =
          std::make_unique<PLFLT[]>(curve->getX().size());

      std::unique_ptr<PLFLT[]> y =
          std::make_unique<PLFLT[]>(curve->getY().size());

      std::copy(curve->getX().begin(), curve->getX().end(), x.get());

      std::copy(curve->getY().begin(), curve->getY().end(), y.get());

      if (curve->isFill() && curve->getSymbol() == 0) {

        _plstream->col0(lookUpColor(curve->getFillColor()));

        _plstream->fill(curve->getX().size(), x.get(), y.get());
      }

      _plstream->col0(lookUpColor(curve->getColor()));

      if (curve->getSymbol() == 0) {

        _plstream->lsty(static_cast<PLINT>(curve->getLineStyle()));

        _plstream->width(curve->getLineWidth());

        _plstream->line(curve->getX().size(), x.get(), y.get());
      } else {

        _plstream->ssym(_defaultcharacterheight, curve->getSymbolSize());

        _plstream->poin(curve->getX().size(), x.get(), y.get(),
                        curve->getSymbol());
      }
    } break;
    case Wrapper::Type::W_Text: {

      text = static_cast<Text *>(p.getData());

      _plstream->col0(lookUpColor(text->getColor()));

      _plstream->width(plot->getLineWidth());

      _plstream->lsty(static_cast<PLINT>(plot->getLineStyle()));

      _plstream->schr(_defaultcharacterheight, text->getSize());

      if (text->getSystem() == Text::CoordinateSystem::DATA) {

        _plstream->ptex(text->getCoordinates().at(0),
                        text->getCoordinates().at(1),
                        (plot->getXLimits().at(1) - plot->getXLimits().at(0)) *
                            cosf(M_PI * text->getAngle() / 180.0),
                        (plot->getYLimits().at(1) - plot->getYLimits().at(0)) *
                            sinf(M_PI * text->getAngle() / 180.0),
                        text->getJustification(), text->getText().data());
      } else if (text->getSystem() == Text::CoordinateSystem::NORMAL) {

        _plstream->ptex(
            plot->getXLimits().at(0) +
                (plot->getXLimits().at(1) - plot->getXLimits().at(0)) *
                    text->getCoordinates().at(0),
            plot->getYLimits().at(0) +
                (plot->getYLimits().at(1) - plot->getYLimits().at(0)) *
                    text->getCoordinates().at(1),
            (plot->getXLimits().at(1) - plot->getXLimits().at(0)) *
                cosf(M_PI * text->getAngle() / 180.0),
            (plot->getYLimits().at(1) - plot->getYLimits().at(0)) *
                sinf(M_PI * text->getAngle() / 180.0),
            text->getJustification(), text->getText().data());
      }
    } break;
    case Wrapper::Type::W_Panels:
    default:

      throw(Exception("Unsupported graphics type"));
      break;
    }
  }

  _plstream->col0(lookUpColor(plot->getColor()));

  _plstream->lsty(static_cast<PLINT>(plot->getLineStyle()));

  _plstream->width(plot->getLineWidth());

  _plstream->schr(_defaultcharacterheight, plot->getFontSize());

  _plstream->smaj(_defaultticklength, plot->getMajorTickLength());

  _plstream->smin(_defaultticklength, plot->getMinorTickLength());

  _plstream->sxax(plot->getXAxis().getMaxDigits(),
                  plot->getXAxis().getPrecision());

  _plstream->syax(plot->getYAxis().getMaxDigits(),
                  plot->getYAxis().getPrecision());

  if (plot->getXAxis().getAxisOptString().find('n') != std::string::npos &&
      plot->getXAxis().getStyle() == Axis::Style::WritePowerInFrame) {

    float order;

    order = floor(log10(xmax));

    if (std::abs(order) > 1.0) {

      std::ostringstream ostr;

      ostr << "(x10#u" << order << "#d)";

      _plstream->ptex(0.9 * (xmax - xmin) + xmin, 0.05 * (ymax - ymin) + ymin,
                      xmax, 0.0, 0.0, ostr.str().c_str());

      xmin /= pow(10, order);

      xmax /= pow(10, order);
    }
  }

  if (plot->getYAxis().getAxisOptString().find('n') != std::string::npos &&
      plot->getYAxis().getStyle() == Axis::Style::WritePowerInFrame) {

    float order;

    order = floor(log10(ymax));

    if (std::abs(order) > 1.0) {

      std::ostringstream ostr;

      ostr << "(x10#u" << order << "#d)";

      _plstream->ptex(0.05 * (xmax - xmin) + xmin, 0.9 * (ymax - ymin) + ymin,
                      (xmax - xmin), 0.0, 0.0, ostr.str().c_str());

      ymin /= pow(10, order);

      ymax /= pow(10, order);
    }
  }

  if (plot->getXAxis().getStyle() != Axis::Style::Default ||
      plot->getYAxis().getStyle() != Axis::Style::Default) {

    _plstream->wind(xmin, xmax, ymin, ymax);
  }

  _plstream->box(plot->getXAxis().getAxisOptString().data(), 0, 0,
                 plot->getYAxis().getAxisOptString().data(), 0, 0);

  _plstream->lab(plot->getXAxis().getTitle().data(),
                 plot->getYAxis().getTitle().data(), plot->getTitle().data());
}
