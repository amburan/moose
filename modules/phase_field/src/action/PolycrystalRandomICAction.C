#include "PolycrystalRandomICAction.h"
#include "Factory.h"
#include "Parser.h"
#include "FEProblem.h"

#include <sstream>
#include <stdexcept>

// libMesh includes
#include "libmesh/libmesh.h"
#include "libmesh/exodusII_io.h"
#include "libmesh/equation_systems.h"
#include "libmesh/nonlinear_implicit_system.h"
#include "libmesh/explicit_system.h"
#include "libmesh/string_to_enum.h"

const Real PolycrystalRandomICAction::_abs_zero_tol = 1e-12;

template<>
InputParameters validParams<PolycrystalRandomICAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<unsigned int>("crys_num", "number of order parameters to create");
  params.addRequiredParam<std::string>("var_name_base","specifies the base name of the variables");
  MooseEnum typ_options("continuous, discrete");
  params.addParam<MooseEnum>("random_type",typ_options,"The type of random polycrystal initial condition. Whether one order parameter is chosen to be 1 at each node or if each order parameter continuously varies from 0 to 1");

  return params;
}

PolycrystalRandomICAction::PolycrystalRandomICAction(const std::string & name, InputParameters params)
  :Action(name, params),
   _crys_num(getParam<unsigned int>("crys_num")),
   _var_name_base(getParam<std::string>("var_name_base")),
   _random_type(getParam<MooseEnum>("random_type"))
{}

void
PolycrystalRandomICAction::act()
{
#ifdef DEBUG
  Moose::err << "Inside the PolycrystalRandomICAction Object\n";
#endif

// Loop through the number of order parameters


  for (unsigned int crys = 0; crys<_crys_num; crys++)
  {
    //Create variable names
    std::string var_name = _var_name_base;
    std::stringstream out;
    out << crys;
    var_name.append(out.str());

    //Set parameters for BoundingBoxIC
    InputParameters poly_params = _factory.getValidParams("PolycrystalRandomIC");
    poly_params.set<VariableName>("variable") = var_name;
    poly_params.set<unsigned int>("crys_num") = _crys_num;
    poly_params.set<unsigned int>("crys_index") = crys;
    poly_params.set<unsigned int>("typ") = _random_type;


    //Add initial condition
    _problem->addInitialCondition("PolycrystalRandomIC", "InitialCondition", poly_params);
  }

}
