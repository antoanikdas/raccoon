# FractureAnisotropy

!syntax description /Materials/FractureAnisotropy

This material defines the constant transverse fracture-anisotropy tensor

\begin{equation}
  \mathbf{A} = \mathbf{I} + (r - 1)\mathbf{m}\otimes\mathbf{m},
\end{equation}

where $\mathbf{m}$ is the normalized material [!param](/Materials/FractureAnisotropy/direction)
and $r>0$ is the [!param](/Materials/FractureAnisotropy/anisotropy_ratio). The eigenvalue along
$\mathbf{m}$ is $r$, and both transverse eigenvalues are one. Setting $r=1$ recovers isotropic
fracture resistance.

Optional `euler_angle_1`, `euler_angle_2`, and `euler_angle_3` rotate this tensor
using the same passive Bunge convention and degrees as `ADComputeElasticityTensor`.
The global tensor is $\mathbf A_{global}=\mathbf R\mathbf A\mathbf R^T$.
All three angles default to zero, preserving direction-based inputs. When angles
are supplied, `direction` specifies the material direction before rotation;
the angles rotate that direction rather than replace it. For $r=1$, the tensor
remains the identity regardless of the angles.

For example, both of the following give the same $+45^\circ$ fracture direction:

```
[Materials]
  [fracture_anisotropy]
    type = FractureAnisotropy
    direction = '1 1 0'
    anisotropy_ratio = 4
  []
[]
```

```
[Materials]
  [fracture_anisotropy]
    type = FractureAnisotropy
    direction = '1 0 0'
    anisotropy_ratio = 4
    euler_angle_1 = -45
    euler_angle_2 = 0
    euler_angle_3 = 0
  []
[]
```

!syntax parameters /Materials/FractureAnisotropy

!syntax inputs /Materials/FractureAnisotropy

!syntax children /Materials/FractureAnisotropy
