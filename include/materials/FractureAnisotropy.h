#pragma once

#include "Material.h"
#include "BaseNameInterface.h"
#include "RankTwoTensor.h"

/**
 * Constant transverse fracture-anisotropy tensor with an optional passive Bunge Euler rotation.
 */
class FractureAnisotropy : public Material, public BaseNameInterface
{
public:
  static InputParameters validParams();

  FractureAnisotropy(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  /// Constant global tensor A = R [I + (r - 1) m (x) m] R^T
  RankTwoTensor _tensor;

  /// Fracture-anisotropy tensor material property
  ADMaterialProperty<RankTwoTensor> & _anisotropy_tensor;
};
