# CrackSurfaceDensity

!syntax description /Materials/CrackSurfaceDensity

The crack-surface density is

\begin{equation}
  \gamma = \frac{1}{c_0 l}\left[\alpha(d) + l^2 \grad d \cdot \mathbf{A} \grad d\right].
\end{equation}

If [!param](/Materials/CrackSurfaceDensity/anisotropy_tensor) is omitted, $\mathbf{A}$ is the
identity tensor and the original isotropic formulation is recovered.

## Example Input File Syntax

!syntax parameters /Materials/CrackSurfaceDensity

!syntax inputs /Materials/CrackSurfaceDensity

!syntax children /Materials/CrackSurfaceDensity
