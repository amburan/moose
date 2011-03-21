#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Object.h"
#include "MaterialProperty.h"
#include "ParallelUniqueId.h"

// libMesh includes
#include "quadrature_gauss.h"
#include "elem.h"

// forward declarations
namespace Moose {
class SubProblem;
}

/**
 * Holds material properties that are assigned to blocks.
 */
class Material : public Object
{
public:
  Material(const std::string & name, InputParameters parameters);
  
  virtual ~Material();

  /** 
   * Block ID the Material is active on.
   * 
   * @return The block ID.
   */
  unsigned int blockID();

  /**
   * Causes the material to recompute all of it's values
   * at the quadrature points.  This is a helper in the base
   * class that does a bunch of common setup first then calls
   * computeProperties().
   */
  void reinit();

  /**
   * Reinit version for side material
   */
  void reinit(unsigned int side);

  bool hasStatefulProperties();

  /**
   * Retrieve the Constant Real valued property named "name"
   */
  template<typename T>
  MaterialProperty<T> & getProperty(const std::string & prop_name);

  /**
   * Retrieve the Constant Real valued property named "name"
   */
  template<typename T>
  MaterialProperty<T> & getPropertyOld(const std::string & prop_name);

  /**
   * Retrieve the Constant Real valued property named "name"
   */
  template<typename T>
  MaterialProperty<T> & getPropertyOlder(const std::string & prop_name);

#if 0
  /**
   * Updates the old (first) material properties to the current/new material properies (second)
   */
  void updateDataState();

  /**
   * This function is called at the beginning of each timestep
   * for each active material block
   */
  virtual void timeStepSetup();
#endif

protected:
  Moose::SubProblem & _problem;
  THREAD_ID _tid;

  unsigned int _qp; 

  QBase * & _qrule;
  const std::vector< Point > & _q_point;

  const Elem * & _current_elem;

  /**
   * Whether or not this material has stateful properties.  This will get automatically
   * set to true if a stateful property is declared.
   */
  bool _has_stateful_props;

// struct DeleteFunctor 
//   {
//     void operator()(const std::pair<const unsigned int, MooseArray<QpData *> > & p) const
//     {
//       //for(MooseArray<QpData *>::iterator i = p.second.begin(); i != p.second.end(); ++i)
//       //  delete *i;
//       std::cerr << p.first << " ";
//       //MooseArray<const QpData *>::iterator i = p.second;
//       std::cerr << p.second[0] << std::endl;
//     }
//  };
  
  enum QP_Data_Type { CURR, PREV };

  /**
   * All materials must override this virtual.
   * This is where they fill up the vectors with values.
   */
  virtual void computeProperties();

  /**
   * Compute material properties in quadrature point
   * Materials has to redefine this.
   */
  virtual void computeQpProperties();

#if 0
  /**
   * This function is called to create the data structure that will be associated
   * with a quadrature point
   */
  virtual QpData * createData();

  /**
   * This function returns a reference to a standard vector of datastructures for all 
   * the quadrature points on the current element
   */
  virtual std::vector<QpData *> & getData(QP_Data_Type qp_data_type); 
#endif

  /**
   * Block ID this material is active on.
   */
  unsigned int _block_id;

  template <typename T>
  bool have_property(const std::string & prop_name) const;

  template <typename T>
  bool have_property_old(const std::string & prop_name) const;

  template <typename T>
  bool have_property_older(const std::string & prop_name) const;

  /**
   * Declare the Real valued property named "name".
   * This must be done _before_ a property of that name is tried
   * to be retrieved using get().
   */
  template<typename T>
  MaterialProperty<T> & declareProperty(const std::string & prop_name);

  /**
   * Declare the Real valued property named "name".
   * This must be done _before_ a property of that name is tried
   * to be retrieved using getOld().
   */
  template<typename T>
  MaterialProperty<T> & declarePropertyOld(const std::string & prop_name);

  /**
   * Declare the Real valued property named "name".
   * This must be done _before_ a property of that name is tried
   * to be retrieved using getOlder().
   */
  template<typename T>
  MaterialProperty<T> & declarePropertyOlder(const std::string & prop_name);

#if 0
  std::map<unsigned int, std::vector<QpData *> > _qp_prev;
  std::map<unsigned int, std::vector<QpData *> > _qp_curr;
#endif

  /**
   * Data structure to map names with values.
   */
  std::set<std::string> _stateful_props;

  Moose::MaterialProperties & _props;
  Moose::MaterialProperties & _props_old;
  Moose::MaterialProperties & _props_older;

  std::map<unsigned int, std::map<unsigned int, Moose::MaterialProperties> > * _props_elem;
  std::map<unsigned int, std::map<unsigned int, Moose::MaterialProperties> > * _props_elem_old;
  std::map<unsigned int, std::map<unsigned int, Moose::MaterialProperties> > * _props_elem_older;
};


template <typename T>
inline bool
Material::have_property (const std::string & prop_name) const
{
  Moose::MaterialProperties::const_iterator it = _props.find(prop_name);

  if (it != _props.end())
    if (dynamic_cast<const MaterialProperty<T>*>(it->second) != NULL)
      return true;

  return false;
}

template <typename T>
inline bool
Material::have_property_old (const std::string & prop_name) const
{
  Moose::MaterialProperties::const_iterator it = _props_old.find(prop_name);

  if (it != _props_old.end())
    if (dynamic_cast<const MaterialProperty<T>*>(it->second) != NULL)
      return true;

  return false;
}

template <typename T>
inline bool
Material::have_property_older (const std::string & prop_name) const
{
  Moose::MaterialProperties::const_iterator it = _props_older.find(prop_name);

  if (it != _props_older.end())
    if (dynamic_cast<const MaterialProperty<T>*>(it->second) != NULL)
      return true;

  return false;
}

template<typename T>
MaterialProperty<T> &
Material::getProperty(const std::string & prop_name)
{
  Moose::MaterialProperties::const_iterator it = _props.find(prop_name);

  if (it != _props.end())
  {
    libmesh_assert (dynamic_cast<const MaterialProperty<T>*>(it->second) != NULL);
    return *dynamic_cast<MaterialProperty<T>*>(it->second);
  }

  mooseError("Material '" + name() + "' has no property named: " + prop_name + "\n\n");
}

template<typename T>
MaterialProperty<T> &
Material::getPropertyOld(const std::string & prop_name)
{
  Moose::MaterialProperties::const_iterator it = _props_old.find(prop_name);

  if (it != _props_old.end())
  {
    libmesh_assert (dynamic_cast<const MaterialProperty<T>*>(it->second) != NULL);
    return *dynamic_cast<MaterialProperty<T>*>(it->second);
  }

  mooseError("Material '" + name() + "' has no property named: " + prop_name + "\n\n");
}

template<typename T>
MaterialProperty<T> &
Material::getPropertyOlder(const std::string & prop_name)
{
  Moose::MaterialProperties::const_iterator it = _props_older.find(prop_name);

  if (it != _props_older.end())
  {
    libmesh_assert (dynamic_cast<const MaterialProperty<T>*>(it->second) != NULL);
    return *dynamic_cast<MaterialProperty<T>*>(it->second);
  }

  mooseError("Material '" + name() + "' has no property named: " + prop_name + "\n\n");
}

template<typename T>
MaterialProperty<T> &
Material::declareProperty(const std::string & prop_name)
{
  if (!this->have_property<T>(prop_name))
    _props[prop_name] = new MaterialProperty<T>;

  MaterialProperty<T> *prop = dynamic_cast<MaterialProperty<T>*>(_props[prop_name]);
  mooseAssert(prop != NULL, "Internal error in declaring material property: " + prop_name);

  return *prop;
}

template<typename T>
MaterialProperty<T> &
Material::declarePropertyOld(const std::string & prop_name)
{
  _has_stateful_props = true;
  _stateful_props.insert(prop_name);

  if (!this->have_property_old<T>(prop_name))
    _props_old[prop_name] = new MaterialProperty<T>;

  MaterialProperty<T> *prop = dynamic_cast<MaterialProperty<T>*>(_props_old[prop_name]);
  mooseAssert(prop != NULL, "Internal error in declaring material property: " + prop_name);

  return *prop;
}

template<typename T>
MaterialProperty<T> &
Material::declarePropertyOlder(const std::string & prop_name)
{
  _has_stateful_props = true;
  _stateful_props.insert(prop_name);

  if (!this->have_property_older<T>(prop_name))
    _props_older[prop_name] = new MaterialProperty<T>;

  MaterialProperty<T> *prop = dynamic_cast<MaterialProperty<T>*>(_props_older[prop_name]);
  mooseAssert(prop != NULL, "Internal error in declaring material property: " + prop_name);

  return *prop;
}


template<>
InputParameters validParams<Material>();

#endif //MATERIAL_H
