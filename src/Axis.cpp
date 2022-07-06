#include "Axis.h"

Axis::Axis()
    : _title(""), _axisoptstr("bcnst"), _precision(1), _maxdigits(6),
      _style(Axis::Style::Default), _label_callback(nullptr),
      _tick_callback(nullptr) {}

void Axis::_set(char option, bool on = true) {

  std::string::size_type i = _axisoptstr.find(option);

  if (on && i == std::string::npos) {

    _axisoptstr.append(1, option);
  }

  if (!on && i != std::string::npos) {

    _axisoptstr.erase(i, 1);
  }
}

void Axis::_reciprocal_labeller(PLINT axis, PLFLT value, PLCHAR_NC_VECTOR label,
                                PLINT length, PLPointer data) {

  if (value == 0.0) {

    *label = '\0';

    return;
  }

  snprintf(label, length, "%g", 1e4 / value);
}

PLINT Axis::_reciprocal_tickfinder(PLINT axis, PLINT nticks, PLFLT *ticks,
                                   PLFLT min, PLFLT max, PLPointer data) {

  PLFLT density = 0.02;

  if (min > max) {

    std::swap(min, max);
  }

  PLFLT w_min = 1e4 / max;
  PLFLT w_max = 1e4 / min;

  PLFLT o_min = log10(w_min);
  PLFLT o_max = log10(w_max);
  PLFLT o_rng = log10(w_max - w_min);

  PLFLT order = (o_max - o_min) < 1.0 ? round(o_rng) - 1.0 : floor(o_min);

  PLFLT delta = pow(10.0, order);

  PLINT n = 0;

  PLFLT tick = pow(10, order) * ceil(w_min * pow(10, -order));

  if (tick != w_min) {
    PLFLT t = w_min * pow(10.0, -floor(o_min));
    if (fmod(t, 0.1) == 0.0 || fmod(t, 0.25) == 0.0) {
      if (n < nticks) {
        ticks[n++] = w_min;
      }
    }
  }

  while (tick < w_max) {
    if (n < nticks) {
      ticks[n++] = tick;
    }
    tick += delta;
    if ((1e4 / ticks[n - 1] - 1e4 / tick) / (max - min) < density) {
      break;
    }
  }

  if (ticks[n - 1] != w_max) {
    PLFLT t = w_max * pow(10.0, -floor(o_max));
    if (fmod(t, 0.1) == 0.0 || fmod(t, 0.25) == 0.0) {
      if (n < nticks) {
        ticks[n++] = w_max;
      }
    }
  }

  for (PLINT i = 0; i < n; i++) {
    ticks[i] = 1e4 / ticks[i];
  }

  return n;
}