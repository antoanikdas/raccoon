#include "ADPFFAnisotropicDiffusion.h"

registerMooseObject("raccoonApp", ADPFFAnisotropicDiffusion);

InputParameters
ADPFFAnisotropicDiffusion::validParams()
{
  InputParameters params = ADKernel::validParams();
  params += BaseNameInterface::validParams();
  params.addClassDescription(
      "The anisotropic diffusion term in the phase-field evolution equation. The weak form is "
      "$(\\grad w, \\dfrac{2\\Gc l}{c_0} \\mathbf{A} \\grad d)$.");

  params.addParam<MaterialPropertyName>(
      "fracture_toughness", "Gc", "The fracture toughness $\\Gc$");
  params.addParam<MaterialPropertyName>(
      "normalization_constant", "c0", "The normalization constant $c_0$");
  params.addParam<MaterialPropertyName>(
      "regularization_length", "l", "The phase-field regularization length");
  params.addParam<MaterialPropertyName>("anisotropy_tensor",
                                        "fracture_anisotropy_tensor",
                                        "The fracture-anisotropy tensor $\\mathbf{A}$");
  return params;
}

ADPFFAnisotropicDiffusion::ADPFFAnisotropicDiffusion(const InputParameters & parameters)
  : ADKernel(parameters),
    BaseNameInterface(parameters),
    _Gc(getADMaterialProperty<Real>(prependBaseName("fracture_toughness", true))),
    _c0(getADMaterialProperty<Real>(prependBaseName("normalization_constant", true))),
    _l(getADMaterialProperty<Real>(prependBaseName("regularization_length", true))),
    _anisotropy_tensor(
        getADMaterialProperty<RankTwoTensor>(prependBaseName("anisotropy_tensor", true)))
{
}

ADReal
ADPFFAnisotropicDiffusion::computeQpResidual()
{
  const ADRealVectorValue anisotropic_gradient = _anisotropy_tensor[_qp] * _grad_u[_qp];
  return 2 * _Gc[_qp] * _l[_qp] / _c0[_qp] * (_grad_test[_i][_qp] * anisotropic_gradient);
}
