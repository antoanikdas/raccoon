//* This file is part of the RACCOON application
//* being developed at Dolbow lab at Duke University
//* http://dolbow.pratt.duke.edu

#include "CrackSurfaceDensity.h"

registerMooseObject("raccoonApp", CrackSurfaceDensity);

InputParameters
CrackSurfaceDensity::validParams()
{
  InputParameters params = Material::validParams();
  params += BaseNameInterface::validParams();
  params.addClassDescription(
      "This class computes the crack surface density $\\gamma = \\dfrac{1}{c_0 l} (\\alpha + l^2 "
      "\\grad d \\cdot \\mathbf{A} \\grad d)$, where $\\alpha$ is the crack geometric function, "
      "$c_0$ is the normalization constant, $l$ is the phase-field regularization length, and "
      "$\\mathbf{A}$ defaults to the identity tensor.");

  params.addRequiredCoupledVar("phase_field", "The phase-field variable");
  params.addParam<MaterialPropertyName>(
      "normalization_constant", "c0", "The normalization constant");
  params.addParam<MaterialPropertyName>(
      "regularization_length", "l", "The phase-field regularization length");
  params.addParam<MaterialPropertyName>(
      "crack_geometric_function", "alpha", "The crack geometric function");
  params.addParam<MaterialPropertyName>("anisotropy_tensor",
                                        "Optional fracture-anisotropy tensor $\\mathbf{A}$");
  params.addParam<MaterialPropertyName>(
      "crack_surface_density", "gamma", "Name of the crack surface density");

  return params;
}

CrackSurfaceDensity::CrackSurfaceDensity(const InputParameters & parameters)
  : Material(parameters),
    BaseNameInterface(parameters),
    _grad_d(adCoupledGradient("phase_field")),
    _c0(getADMaterialProperty<Real>(prependBaseName("normalization_constant", true))),
    _l(getADMaterialProperty<Real>(prependBaseName("regularization_length", true))),
    _alpha(getADMaterialProperty<Real>(prependBaseName("crack_geometric_function", true))),
    _anisotropy_tensor(
        isParamValid("anisotropy_tensor")
            ? &getADMaterialProperty<RankTwoTensor>(prependBaseName("anisotropy_tensor", true))
            : nullptr),
    _gamma(declareADProperty<Real>(prependBaseName("crack_surface_density", true)))
{
}

void
CrackSurfaceDensity::computeQpProperties()
{
  const ADReal gradient_density = _anisotropy_tensor
                                      ? _grad_d[_qp] * ((*_anisotropy_tensor)[_qp] * _grad_d[_qp])
                                      : _grad_d[_qp] * _grad_d[_qp];
  _gamma[_qp] = (_alpha[_qp] + _l[_qp] * _l[_qp] * gradient_density) / _c0[_qp] / _l[_qp];
}
