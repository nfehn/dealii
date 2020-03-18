// ---------------------------------------------------------------------
//
// Copyright (C) 2000 - 2018 by the deal.II authors
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



// test internal::extract_dofs_by_component for some corner cases that
// I was unsure about when refactoring some code in there
//
// this particular test checks the call path to
// internal::extract_dofs_by_component from DoFTools::extract_dofs via
// the block_select flag


#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_nedelec.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_raviart_thomas.h>
#include <deal.II/fe/fe_system.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/hp/dof_handler.h>

#include "../tests.h"



template <int dim>
void
check()
{
  Triangulation<dim> tr;
  GridGenerator::hyper_cube(tr, -1, 1);
  tr.refine_global(1);

  // create an FECollection and set
  // one cell to use the second
  // element of this collection
  hp::FECollection<dim> element;
  for (unsigned int i = 0; i < 2; ++i)
    element.push_back(
      FESystem<dim>(FE_Q<dim>(1 + i), 1, FE_Nedelec<dim>(0), 1));
  hp::DoFHandler<dim> dof(tr);
  dof.begin_active()->set_active_fe_index(1);
  dof.distribute_dofs(element);

  // try all possible block
  // masks, which we encode as bit
  // strings
  for (unsigned int int_mask = 0; int_mask < (1U << element[0].n_blocks());
       ++int_mask)
    {
      std::vector<bool> component_mask(element[0].n_blocks());
      for (unsigned int c = 0; c < element[0].n_blocks(); ++c)
        component_mask[c] = (int_mask & (1 << c));

      const IndexSet dofs =
        DoFTools::extract_dofs(dof, BlockMask(component_mask));

      for (unsigned int d = 0; d < dof.n_dofs(); ++d)
        deallog << dofs.is_element(d);
      deallog << std::endl;
    }
}


int
main()
{
  initlog();
  deallog << std::setprecision(2) << std::fixed;

  deallog.push("2d");
  check<2>();
  deallog.pop();
  deallog.push("3d");
  check<3>();
  deallog.pop();
}
