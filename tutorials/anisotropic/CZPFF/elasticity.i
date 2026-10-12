# CZ: A Euler angle 1 = -45 degrees; C material axis remains at +45 degrees.
Gc = 2.7
# Critical elastic energy density used by the existing RACCOON CZ examples.
psic = 14.88
l = 0.02
anisotropy_ratio = 4
# Passive Bunge Euler angles for A; C has independent angles below.
fracture_euler_angle_1 = -45
fracture_euler_angle_2 = 0
fracture_euler_angle_3 = 0
nx = 30
ny_half = 15
refine = 2
# Absolute change-of-norm tolerance for this unit-size tutorial specimen.
damage_change_tolerance = 1e-6
# Safety limit; reaching this without convergence rejects the time step.
staggered_max_iterations = 10

# Orthotropic engineering constants in material axes 1, 2, 3.
E1 = 2.1e5
E2 = 1.4e5
E3 = 1.0e5
G12 = 7.0e4
G23 = 4.5e4
G31 = 5.5e4
nu12 = 0.25
nu13 = 0.20
nu23 = 0.30

[MultiApps]
  [fracture]
    type = TransientMultiApp
    input_files = fracture.i
    cli_args = 'Gc=${Gc};psic=${psic};l=${l};anisotropy_ratio=${anisotropy_ratio};nx=${nx};ny_half=${ny_half};refine=${refine};fracture_euler_angle_1=${fracture_euler_angle_1};fracture_euler_angle_2=${fracture_euler_angle_2};fracture_euler_angle_3=${fracture_euler_angle_3}'
    execute_on = TIMESTEP_END
  []
[]

[Transfers]
  [from_d]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    from_multi_app = fracture
    variable = d
    source_variable = d
  []
  [to_psie_active]
    type = MultiAppGeneralFieldShapeEvaluationTransfer
    to_multi_app = fracture
    variable = psie_active
    source_variable = psie_active
  []
[]

[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Mesh]
  [top_half]
    type = GeneratedMeshGenerator
    dim = 2
    nx = ${nx}
    ny = ${ny_half}
    ymin = 0
    ymax = 0.5
    boundary_id_offset = 0
    boundary_name_prefix = top_half
  []
  [top_stitch]
    type = BoundingBoxNodeSetGenerator
    input = top_half
    new_boundary = top_stitch
    bottom_left = '0.5 0 0'
    top_right = '1 0 0'
  []
  [bottom_half]
    type = GeneratedMeshGenerator
    dim = 2
    nx = ${nx}
    ny = ${ny_half}
    ymin = -0.5
    ymax = 0
    boundary_id_offset = 5
    boundary_name_prefix = bottom_half
  []
  [bottom_stitch]
    type = BoundingBoxNodeSetGenerator
    input = bottom_half
    new_boundary = bottom_stitch
    bottom_left = '0.5 0 0'
    top_right = '1 0 0'
  []
  [stitch]
    type = StitchMeshGenerator
    inputs = 'top_stitch bottom_stitch'
    stitch_boundaries_pairs = 'top_stitch bottom_stitch'
  []
  [pin]
    type = ExtraNodesetGenerator
    input = stitch
    new_boundary = pin
    coord = '1 -0.5 0'
  []
  construct_side_list_from_node_list = true
[]

[Adaptivity]
  marker = marker
  initial_marker = marker
  initial_steps = ${refine}
  stop_time = 0
  max_h_level = ${refine}
  [Markers]
    [marker]
      type = BoxMarker
      bottom_left = '0.4 -0.5 0'
      top_right = '1 0.5 0'
      outside = DO_NOTHING
      inside = REFINE
    []
  []
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
[]

[AuxVariables]
  [fy]
  []
  [d]
  []
[]

[Kernels]
  [solid_x]
    type = ADStressDivergenceTensors
    variable = disp_x
    component = 0
  []
  [solid_y]
    type = ADStressDivergenceTensors
    variable = disp_y
    component = 1
    save_in = fy
  []
[]

[BCs]
  [top_ydisp]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = top_half_top
    function = t
  []
  [bottom_ydisp]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = bottom_half_bottom
    function = '-t'
  []
  [xfix]
    type = DirichletBC
    variable = disp_x
    boundary = pin
    value = 0
  []
[]

[Materials]
  [fracture_properties]
    type = ADGenericConstantMaterial
    prop_names = 'Gc psic l'
    prop_values = '${Gc} ${psic} ${l}'
  []
  [crack_geometric]
    type = CrackGeometricFunction
    property_name = alpha
    expression = d
    phase_field = d
  []
  [elasticity_tensor]
    type = ADComputeElasticityTensor
    fill_method = orthotropic
    C_ijkl = '${E1} ${E2} ${E3} ${G12} ${G23} ${G31} ${fparse nu12*E2/E1} ${fparse nu13*E3/E1} ${fparse nu23*E3/E2} ${nu12} ${nu13} ${nu23}'
    # MOOSE uses passive Bunge angles; -45 degrees gives an active +45-degree rotation.
    euler_angle_1 = -45
    euler_angle_2 = 0
    euler_angle_3 = 0
  []
  [degradation]
    type = RationalDegradationFunction
    property_name = g
    expression = (1-d)^p/((1-d)^p+(Gc/psic*xi/c0/l)*d*(1+a2*d+a2*a3*d^2))*(1-eta)+eta
    phase_field = d
    material_property_names = 'Gc psic xi c0 l'
    parameter_names = 'p a2 a3 eta'
    parameter_values = '2 -0.5 0 1e-6'
  []
  [strain]
    type = ADComputeSmallStrain
  []
  [elasticity]
    type = SmallDeformationAnisotropicElasticity
    elasticity_tensor = elasticity_tensor
    phase_field = d
    degradation_function = g
    decomposition = STRESS_SPECTRAL
    output_properties = 'elastic_strain psie_active'
    outputs = exodus
  []
  [stress]
    type = ComputeSmallDeformationStress
    elasticity_model = elasticity
    output_properties = stress
    outputs = exodus
  []
[]

[Postprocessors]
  [damage_change]
    type = ChangeOverFixedPointPostprocessor
    postprocessor = damage_step_norm
    take_absolute_value = true
    execute_on = MULTIAPP_FIXED_POINT_CONVERGENCE
  []
  [damage_step_norm]
    type = SolutionChangeNorm
    variable = d
    execute_on = MULTIAPP_FIXED_POINT_CONVERGENCE
  []
  [Fy]
    type = NodalSum
    variable = fy
    boundary = top_half_top
  []
[]

[Convergence]
  [damage_convergence]
    type = PostprocessorConvergence
    postprocessor = damage_change
    tolerance = ${damage_change_tolerance}
    # Require a mechanics re-solve with the newly transferred damage.
    min_iterations = 2
    max_iterations = ${staggered_max_iterations}
    # If the damage change is not below the tolerance after the maximum number of iterations, consider it converged.
    converge_at_max_iterations = true
  []
[]

[Executioner]
  type = Transient
  solve_type = NEWTON
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu       superlu_dist'
  automatic_scaling = true
  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-10
  dt = 1e-4
  end_time = 5e-3
  multiapp_fixed_point_convergence = damage_convergence
[]

[Outputs]
  exodus = true
  csv = true
  print_linear_residuals = false
[]
