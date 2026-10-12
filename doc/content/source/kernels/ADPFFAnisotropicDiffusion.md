# ADPFFAnisotropicDiffusion

!syntax description /Kernels/ADPFFAnisotropicDiffusion

This kernel supplies the gradient contribution for the crack-surface density

\begin{equation}
  \gamma(d, \grad d) = \frac{1}{c_0 l}\left[\alpha(d) + l^2 \grad d \cdot \mathbf{A} \grad d\right],
\end{equation}

where $\mathbf{A}$ is a symmetric positive-definite fracture-anisotropy tensor. Its weak-form
contribution is

\begin{equation}
  \left(\grad w, \frac{2 G_c l}{c_0}\mathbf{A}\grad d\right).
\end{equation}

The isotropic formulation is recovered when $\mathbf{A}=\mathbf{I}$.

!syntax parameters /Kernels/ADPFFAnisotropicDiffusion

!syntax inputs /Kernels/ADPFFAnisotropicDiffusion

!syntax children /Kernels/ADPFFAnisotropicDiffusion
