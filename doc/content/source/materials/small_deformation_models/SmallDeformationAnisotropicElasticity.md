# SmallDeformationAnisotropicElasticity

!syntax description /Materials/SmallDeformationAnisotropicElasticity

This material supplies small-strain linear elasticity for a general rank-four stiffness tensor.
With `decomposition = NONE`,

\[
  \boldsymbol{\sigma} = g(d)\,\mathsf{C}:\boldsymbol{\varepsilon}, \qquad
  \psi_e = g(d)\,\frac{1}{2}\boldsymbol{\varepsilon}:\mathsf{C}:\boldsymbol{\varepsilon}.
\]

The `elasticity_tensor` may be produced by MOOSE's `ADComputeElasticityTensor`.  Its `orthotropic`
fill method accepts engineering constants in the order

```
E11 E22 E33 G12 G23 G31 nu21 nu31 nu32 nu12 nu13 nu23
```

and supports material-axis rotation with `euler_angle_1`, `euler_angle_2`, and `euler_angle_3`.
A transversely isotropic solid with axis 1 is obtained by imposing

\[
E_{22}=E_{33},\quad \nu_{12}=\nu_{13},\quad G_{12}=G_{31},\quad
G_{23}=\frac{E_{22}}{2(1+\nu_{23})},
\]

with reciprocal Poisson ratios \(\nu_{ji}/E_j=\nu_{ij}/E_i\).

For an isotropic material defined by bulk modulus \(K\) and shear modulus \(G\), use
`fill_method = symmetric_isotropic` and `C_ijkl = '${fparse K-2*G/3} ${G}'`.  This gives exactly the
same constitutive response as `SmallDeformationIsotropicElasticity` with `decomposition = NONE`.

With `decomposition = STRESS_SPECTRAL`, the intact stress is spectrally decomposed as

\[
\boldsymbol{\sigma}_0=\mathsf{C}:\boldsymbol{\varepsilon}
=\boldsymbol{\sigma}_0^+ + \boldsymbol{\sigma}_0^-,
\qquad
\boldsymbol{\sigma}_0^+=\sum_a\langle\sigma_a\rangle_+
\boldsymbol{n}_a\otimes\boldsymbol{n}_a.
\]

The phase field degrades only the positive principal-stress contribution,

\[
\boldsymbol{\sigma}=g(d)\boldsymbol{\sigma}_0^+ + \boldsymbol{\sigma}_0^-,
\qquad
\psi_e^+=\frac12\boldsymbol{\sigma}_0^+:\boldsymbol{\varepsilon},
\qquad
\psi_e=g(d)\psi_e^+ + \frac12\boldsymbol{\sigma}_0^-:\boldsymbol{\varepsilon}.
\]

`psie_active` is therefore the tensile crack-driving energy transferred to the phase-field
sub-application, and `dpsie_dd` contains only its degradation derivative. This follows MOOSE's
anisotropic `stress_spectral` convention. A strain-spectral or volumetric/deviatoric split is not
applied to a general anisotropic tensor because those isotropic formulas do not extend uniquely.
For an undamaged material, both decomposition choices recover
\(\boldsymbol{\sigma}=\mathsf{C}:\boldsymbol{\varepsilon}\) exactly. Under partial damage and a
mixed tension/compression state, `STRESS_SPECTRAL` is intentionally not identical to the
strain-based `SPECTRAL` split in `SmallDeformationIsotropicElasticity`.

!syntax parameters /Materials/SmallDeformationAnisotropicElasticity

!syntax inputs /Materials/SmallDeformationAnisotropicElasticity

!syntax children /Materials/SmallDeformationAnisotropicElasticity
