[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 4
  ny = 4
  elem_type = QUAD9
[]

[Variables]
  [d]
    order = SECOND
  []
[]

[Kernels]
  [anisotropic_diffusion]
    type = ADPFFAnisotropicDiffusion
    variable = d
    fracture_toughness = Gc
    regularization_length = l
    normalization_constant = c0
    anisotropy_tensor = fracture_anisotropy_tensor
  []
  [source]
    type = ADCoefMatSource
    variable = d
    coefficient = 10
  []
[]

[BCs]
  [exact]
    type = ADFunctionDirichletBC
    variable = d
    boundary = 'left right bottom top'
    function = 'x^2+2*y^2'
  []
[]

[Materials]
  [fracture_properties]
    type = ADGenericConstantMaterial
    prop_names = 'Gc l c0 alpha'
    prop_values = '0.5 1 1 0'
  []
  [fracture_anisotropy]
    type = FractureAnisotropy
    direction = '1 0 0'
    anisotropy_ratio = 3
  []
  [crack_surface_density]
    type = CrackSurfaceDensity
    phase_field = d
    anisotropy_tensor = fracture_anisotropy_tensor
  []
[]

[Postprocessors]
  [error]
    type = ElementL2Error
    variable = d
    function = 'x^2+2*y^2'
  []
  [average_gamma]
    type = ADElementAverageMaterialProperty
    mat_prop = gamma
  []
[]

[Executioner]
  type = Steady
  solve_type = NEWTON
[]

[Outputs]
  csv = true
[]
