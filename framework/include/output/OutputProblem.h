/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef OUTPUTPROBLEM_H
#define OUTPUTPROBLEM_H

#include "FEProblem.h"
#include "Output.h"
// libMesh
#include "libmesh/equation_systems.h"
#include "libmesh/vector_value.h"
#include "libmesh/mesh_function.h"

class MooseMesh;
class OutputProblem;

template<>
InputParameters validParams<OutputProblem>();

class OutputProblem : public Problem
{
public:
  OutputProblem(const std::string & name, InputParameters parameters);
  virtual ~OutputProblem();

  // Solve /////
  virtual void init();

  // Setup /////
  virtual void timestepSetup();

  // Output system /////

  virtual Output & out() { return _out; }       // NOTE: don't like this -> remove and replace with better design
  virtual void output(bool /*force*/ = false) { _out.output(); }

  virtual void outputPps(const FormattedTable & table);
  virtual void outputInput();

  /**
   * Set a position offset for the output.
   */
  virtual void setPosition(const Point & p);

protected:
  /**
   * Move the mesh into position based on _position.
   */
  void moveMesh();

  FEProblem & _mproblem;
  MooseMesh & _mesh;
  EquationSystems _eq;
  Output _out;
  std::vector<std::vector<MeshFunction *> > _mesh_functions;
  NumericVector<Number> * _serialized_solution;

  Point _position;

//  bool _mesh_might_change;

private:
  static MooseMesh * meshInitHelper(FEProblem & fe_problem, InputParameters & params);
};

#endif
