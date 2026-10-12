#pragma once

#include "ADKernel.h"
#include "BaseNameInterface.h"
#include "ADRankTwoTensorForward.h"

/**
 * Gradient term for a phase-field fracture model with quadratic fracture anisotropy.
 */
class ADPFFAnisotropicDiffusion : public ADKernel, public BaseNameInterface
{
public:
  static InputParameters validParams();

  ADPFFAnisotropicDiffusion(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

  /// The fracture toughness
  const ADMaterialProperty<Real> & _Gc;

  /// The normalization constant
  const ADMaterialProperty<Real> & _c0;

  /// The phase-field regularization length
  const ADMaterialProperty<Real> & _l;

  /// The symmetric positive-definite fracture-anisotropy tensor
  const ADMaterialProperty<RankTwoTensor> & _anisotropy_tensor;
};
