#include "SmallDeformationAnisotropicElasticity.h"
#include "RaccoonUtils.h"

registerMooseObject("raccoonApp", SmallDeformationAnisotropicElasticity);

InputParameters
SmallDeformationAnisotropicElasticity::validParams()
{
  InputParameters params = SmallDeformationElasticityModel::validParams();
  params.addClassDescription(
      "General anisotropic linear elasticity under small-strain assumptions, with full-energy or "
      "principal-stress spectral degradation by the phase field.");

  params.addParam<MaterialPropertyName>(
      "elasticity_tensor", "elasticity_tensor", "Name of the rank-four elasticity tensor");
  params.addParam<MaterialPropertyName>(
      "bulk_modulus",
      "Compatibility parameter for input files derived from SmallDeformationIsotropicElasticity; "
      "the anisotropic model obtains all moduli from elasticity_tensor");
  params.addParam<MaterialPropertyName>(
      "shear_modulus",
      "Compatibility parameter for input files derived from SmallDeformationIsotropicElasticity; "
      "the anisotropic model obtains all moduli from elasticity_tensor");

  params.addRequiredCoupledVar("phase_field", "Name of the phase-field (damage) variable");
  params.addParam<MaterialPropertyName>(
      "strain_energy_density",
      "psie",
      "Name of the strain energy density computed by this material model");
  params.addParam<MaterialPropertyName>("degradation_function", "g", "The degradation function");
  params.addParam<MooseEnum>(
      "decomposition",
      MooseEnum("NONE STRESS_SPECTRAL", "NONE"),
      "Energy decomposition. STRESS_SPECTRAL degrades the contribution from positive principal "
      "stresses and preserves the negative principal-stress contribution");

  return params;
}

SmallDeformationAnisotropicElasticity::SmallDeformationAnisotropicElasticity(
    const InputParameters & parameters)
  : SmallDeformationElasticityModel(parameters),
    DerivativeMaterialPropertyNameInterface(),
    _elasticity_tensor(getADMaterialProperty<RankFourTensor>("elasticity_tensor")),
    _d_name(getVar("phase_field", 0)->name()),
    _psie_name(prependBaseName("strain_energy_density", true)),
    _psie(declareADProperty<Real>(_psie_name)),
    _psie_active(declareADProperty<Real>(_psie_name + "_active")),
    _dpsie_dd(declareADProperty<Real>(derivativePropertyName(_psie_name, {_d_name}))),
    _g_name(prependBaseName("degradation_function", true)),
    _g(getADMaterialProperty<Real>(_g_name)),
    _dg_dd(getADMaterialProperty<Real>(derivativePropertyName(_g_name, {_d_name}))),
    _decomposition(getParam<MooseEnum>("decomposition").getEnum<Decomposition>())
{
}

ADRankTwoTensor
SmallDeformationAnisotropicElasticity::computeStress(const ADRankTwoTensor & strain)
{
  const ADRankTwoTensor stress_intact = _elasticity_tensor[_qp] * strain;

  if (_decomposition == Decomposition::none)
    return computeNoDecomposition(strain, stress_intact);
  if (_decomposition == Decomposition::stress_spectral)
    return computeStressSpectralDecomposition(strain, stress_intact);

  paramError("decomposition", "Unsupported decomposition type");
}

ADRankTwoTensor
SmallDeformationAnisotropicElasticity::computeNoDecomposition(
    const ADRankTwoTensor & strain, const ADRankTwoTensor & stress_intact)
{
  const ADReal energy_intact = 0.5 * stress_intact.doubleContraction(strain);

  _psie_active[_qp] = energy_intact;
  _psie[_qp] = _g[_qp] * energy_intact;
  _dpsie_dd[_qp] = _dg_dd[_qp] * energy_intact;

  return _g[_qp] * stress_intact;
}

ADRankTwoTensor
SmallDeformationAnisotropicElasticity::computeStressSpectralDecomposition(
    const ADRankTwoTensor & strain, const ADRankTwoTensor & stress_intact)
{
  const ADRankTwoTensor stress_active = RaccoonUtils::spectralDecomposition(stress_intact);
  const ADRankTwoTensor stress_inactive = stress_intact - stress_active;

  _psie_active[_qp] = 0.5 * stress_active.doubleContraction(strain);
  const ADReal psie_inactive = 0.5 * stress_inactive.doubleContraction(strain);

  _psie[_qp] = _g[_qp] * _psie_active[_qp] + psie_inactive;
  _dpsie_dd[_qp] = _dg_dd[_qp] * _psie_active[_qp];

  return _g[_qp] * stress_active + stress_inactive;
}
