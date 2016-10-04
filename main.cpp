#include <fstream>
#include <iostream>

#include <QApplication>

#include "histogram.h"
#include "histogram-qt.h"
#include "main_window.h"
#include "parser.h"
#include "run.h"
#include "run_config.h"

double get_mass(const event &e) {
  return (e.tracks[0].p + e.tracks[1].p).norm();
}

bool cut_pt(const event &e) {
  double pt2_1 = e.tracks[0].p.x() * e.tracks[0].p.x() + e.tracks[0].p.y() * e.tracks[0].p.y();
  double pt2_2 = e.tracks[1].p.x() * e.tracks[1].p.x() + e.tracks[1].p.y() * e.tracks[1].p.y();
  return pt2_1 > .3 * .3 && pt2_2 > .3 * .3;
}

bool cut_m_pi_pi(const event &e) {
  return (e.tracks[0].p + e.tracks[1].p).norm() > .5;
}

bool cut_eta(const event &e) {
  lorentz::vec rho = e.tracks[0].p + e.tracks[1].p;
  lorentz::vec fake = lorentz::vec::mxyz(0, rho.x(), rho.y(), rho.z());
  double eta = std::atanh(fake.z() / fake.t());
  return std::abs(eta) < 2.5;
}

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  // For strtod
  std::setlocale(LC_ALL, "C");

  // Histograms
  run r;
  hist::linear_axis<double> mass_axis = hist::linear_axis<double>(0, 1.5, 75);
  r.add_fill("mass", mass_axis, &get_mass);

  hist::linear_axis<double> pt_axis = hist::linear_axis<double>(0, 1, 200);
  r.add_fill("pt^2 ~= -t", pt_axis, [](const event &e) {
    lorentz::vec rho = e.tracks[0].p + e.tracks[1].p;
    return rho.x() * rho.x() + rho.y() * rho.y();
  });

  hist::linear_axis<double> y_axis = hist::linear_axis<double>(-10, 10, 100);
  r.add_fill("y", y_axis, [](const event &e) {
    lorentz::vec rho = e.tracks[0].p + e.tracks[1].p;
    return 0.5 * std::log((rho.t() + rho.z()) / (rho.t() - rho.z()));
  });

  hist::linear_axis<double> eta_axis = hist::linear_axis<double>(-5, 5, 100);
  r.add_fill("eta", eta_axis, [](const event &e) {
    lorentz::vec rho = e.tracks[0].p + e.tracks[1].p;
    lorentz::vec fake = lorentz::vec::mxyz(0, rho.x(), rho.y(), rho.z());
    return std::atanh(fake.z() / fake.t());
  });

  // Cuts
  run_config *rc = new run_config;
  rc->add_cut("pt(pi) > 0.3", &cut_pt);
  rc->add_cut("M(pi pi) > 0.5", &cut_m_pi_pi);
  rc->add_cut("|eta(rho)| < 2.5", &cut_eta);

  // Loop over events
  starlight_parser parser("/home/louis/Documents/ULB/MA1/Mémoire/starlight/data/slight.rho.out");
  main_window *win = new main_window(r, rc, &parser);
  win->show();

  return app.exec();
}
