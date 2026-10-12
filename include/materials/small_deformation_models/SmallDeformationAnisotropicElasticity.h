#pragma once

#include "SmallDeformationElasticityModel.h"
#include "DerivativeMaterialPropertyNameInterface.h"
#include "RankFourTensorForward.h"

/**
 * Small-strain linear elasticity for a general elasticity tensor.
 *
 * The model supports either full-energy degradation or a principal-stress
 * tension/compression split suitable for anisotropic stiffness tensors.
 */
class SmallDeformationAnisotropicElasticity
  : public SmallDeformationElasticityModel,
    public DerivativeMaterialPropertyNameInterface
{
public:
  static InputParameters validParams();

  SmallDeformationAnisotropicElasticity(const InputParameters & parameters);

  virtual ADRankTwoTensor computeStress(const ADRankTwoTensor & strain) override;

protected:
  ADRankTwoTensor computeNoDecomposition(const ADRankTwoTensor & strain,
                                         const ADRankTwoTensor & stress_intact);
  ADRankTwoTensor computeStressSpectralDecomposition(const ADRankTwoTensor & strain,
                                                     const ADRankTwoTensor & stress_intact);

  /// Elasticity tensor, including any material-axis rotation
  const ADMaterialProperty<RankFourTensor> & _elasticity_tensor;

  /// Name of the phase-field variable
  const VariableName _d_name;

  // Strain energy density and its derivative with respect to damage
  const MaterialPropertyName _psie_name;
  ADMaterialProperty<Real> & _psie;
  ADMaterialProperty<Real> & _psie_active;
  ADMaterialProperty<Real> & _dpsie_dd;

  // Degradation function and its derivative with respect to damage
  const MaterialPropertyName _g_name;
  const ADMaterialProperty<Real> & _g;
  const ADMaterialProperty<Real> & _dg_dd;

  enum class Decomposition
  {
    none,
    stress_spectral
  };

  const Decomposition _decomposition;
};
