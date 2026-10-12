#include "FractureAnisotropy.h"
#include "RotationTensor.h"

registerMooseObject("raccoonApp", FractureAnisotropy);

InputParameters
FractureAnisotropy::validParams()
{
  InputParameters params = Material::validParams();
  params += BaseNameInterface::validParams();
  params.addClassDescription("Defines the constant transverse fracture-anisotropy tensor "
                             "$\\mathbf{A}=\\mathbf{I}+(r-1)\\mathbf{m}\\otimes\\mathbf{m}$.");
  params.addParam<RealVectorValue>(
      "direction", RealVectorValue(1, 0, 0), "The material direction before Euler rotation");
  params.addParam<Real>("euler_angle_1", 0.0, "First passive Bunge Euler angle in degrees");
  params.addParam<Real>("euler_angle_2", 0.0, "Second passive Bunge Euler angle in degrees");
  params.addParam<Real>("euler_angle_3", 0.0, "Third passive Bunge Euler angle in degrees");
  params.addRangeCheckedParam<Real>(
      "anisotropy_ratio", 1.0, "anisotropy_ratio > 0", "The gradient-energy ratio $r$");
  params.addParam<MaterialPropertyName>("anisotropy_tensor",
                                        "fracture_anisotropy_tensor",
                                        "Name of the fracture-anisotropy tensor property");
  return params;
}

FractureAnisotropy::FractureAnisotropy(const InputParameters & parameters)
  : Material(parameters),
    BaseNameInterface(parameters),
    _tensor(RankTwoTensor::initIdentity),
    _anisotropy_tensor(declareADProperty<RankTwoTensor>(prependBaseName("anisotropy_tensor", true)))
{
  RealVectorValue direction = getParam<RealVectorValue>("direction");
  const Real norm = direction.norm();
  if (norm == 0.0)
    paramError("direction", "The fracture-anisotropy direction must be nonzero.");

  direction /= norm;
  const Real ratio = getParam<Real>("anisotropy_ratio");
  _tensor += (ratio - 1.0) * RankTwoTensor::selfOuterProduct(direction);
  if (ratio != 1.0)
  {
    const RotationTensor rotation(RealVectorValue(getParam<Real>("euler_angle_1"),
                                                getParam<Real>("euler_angle_2"),
                                                getParam<Real>("euler_angle_3")));
    _tensor.rotate(rotation);
  }
}

void
FractureAnisotropy::computeQpProperties()
{
  _anisotropy_tensor[_qp] = _tensor;
}
