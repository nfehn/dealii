// ---------------------------------------------------------------------
//
// Copyright (C) 2019 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------



// check serialization for FESeries::Fourier

#include <deal.II/base/quadrature_lib.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_series.h>

#include <deal.II/hp/fe_collection.h>
#include <deal.II/hp/q_collection.h>

#include "serialization.h"

template <int dim>
void
test()
{
  // setup
  hp::FECollection<dim> hp_fe;
  hp::QCollection<dim>  hp_q;

  const unsigned int   min_degree = 1, max_degree = 2;
  const QGauss<1>      base_quadrature(4);
  const QIterated<dim> quadrature(base_quadrature, max_degree);
  const QSorted<dim>   quadrature_sorted(quadrature);
  for (unsigned int p = min_degree; p <= max_degree; ++p)
    {
      hp_fe.push_back(FE_Q<dim>(p));
      hp_q.push_back(quadrature_sorted);
    }

  FESeries::Fourier<dim> fourier_save(max_degree + 1, hp_fe, hp_q);
  FESeries::Fourier<dim> fourier_load(max_degree + 1, hp_fe, hp_q);

  // create transformation matrices
  fourier_save.precalculate_all_transformation_matrices();

  // save series expansion object
  std::ostringstream            oss;
  boost::archive::text_oarchive oa(oss, boost::archive::no_header);
  fourier_save.save_transformation_matrices(oa, 0);
  deallog << oss.str() << std::endl;

  // load expansion object
  std::istringstream            iss(oss.str());
  boost::archive::text_iarchive ia(iss, boost::archive::no_header);
  fourier_load.load_transformation_matrices(ia, 0);
  AssertThrow(compare(fourier_save, fourier_load), ExcInternalError());

  deallog << "OK" << std::endl;
}


int
main()
{
  initlog();

  deallog.push("1d");
  test<1>();
  deallog.pop();
  deallog.push("2d");
  test<2>();
  deallog.pop();
  deallog.push("3d");
  test<3>();
  deallog.pop();
}
