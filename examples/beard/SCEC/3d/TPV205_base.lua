-- refinement parameters
min_level = 0
max_level = 0
output_prefix = "TPV205"

-- connectivity info
connectivity = "brick"
brick =
{
  nx = 40,
  ny = 20,
  nz = 20,
  periodic_x = 0,
  periodic_y = 0,
  periodic_z = 0,
}

-- set up the domain
Lx = 1.5
Ly = 1.5
Lz = 1.5

Cx = brick.nx/2
Cy = brick.ny/2
Cz = brick.nz

function connectivity_vertices(x, y, z)
  xout = Lx*(x-Cx)
  yout = Ly*(y-Cy)
  zout = Lz*(z-Cz)
  return xout,yout,zout
end

function refinement_function(
  x0,y0,z0,x1,y1,z1,
  x2,y2,z2,x3,y3,z3,
  x4,y4,z4,x5,y5,z5,
  x6,y6,z6,x7,y7,z7,
  level, treeid)

  return 0

  -- if level < min_level then
  --   return 1
  -- elseif level >= max_level then
  --   return 0
  -- else
  --   return 0
  -- end
end

function element_order(
  x0,y0,z0,x1,y1,z1,
  x2,y2,z2,x3,y3,z3,
  x4,y4,z4,x5,y5,z5,
  x6,y6,z6,x7,y7,z7,
  level, treeid)

  -- N = treeid%3+1
  N = 2

  return N
end

-- material properties
cs = 3.464
cp = 6
rho = 2.670
mu  = rho*cs^2
lam = rho*cp^2-2*mu

-- field conditions
S11 = 0
S22 = 0
S33 = 0
S12 = 0
S13 = 0
S23 = 0
v1  = 0
v2  = 0
v3  = 0

-- time stepper to use
lsrk_method  = "KC54"

tend  = 12
tout  = 0.1
tdisp = 0.1
nerr  = 0

function time_step_parameters(dt)
  dt      = 0.5*dt
  noutput = math.ceil(tout / dt)
  dt      = tout / noutput
  ndisp   = tdisp / dt
  nsteps  = tend / dt
  nsteps  = 1
  return dt,nsteps, ndisp, noutput
end

-- faults

fault_1 = {
  type = "friction",
  friction = "slip weakening",
  fd =     0.525,
  fs =     0.4,
  S11 =    0.0,
  S12 =   70.0,
  S13 =    0.0,
  S22 = -120.0,
  S23 =    0.0,
  S33 =    0.0,
}

fault_2 = {
  type = "friction",
  friction = "slip weakening",
  fd =     0.525,
  fs =     0.4,
  S11 =    0.0,
  S12 =   81.6,
  S13 =    0.0,
  S22 = -120.0,
  S23 =    0.0,
  S33 =    0.0,
}

fault_3 = {
  type = "friction",
  friction = "slip weakening",
  fd =     0.525,
  fs =     0.4,
  S11 =   78.0,
  S12 =    0.0,
  S13 =    0.0,
  S22 = -120.0,
  S23 =    0.0,
  S33 =    0.0,
}

fault_4 = {
  type = "friction",
  friction = "slip weakening",
  fd =     0.525,
  fs =     0.4,
  S11 =    0.0,
  S12 =   62.0,
  S13 =    0.0,
  S22 = -120.0,
  S23 =    0.0,
  S33 =    0.0,
}

bc_free = {
  type = "boundary",
  R    = -1,
}

bc_nonreflect = {
  type = "boundary",
  R    = 0,
}

bc_rigid = {
  type = "boundary",
  R    = 1,
}

glue_info = {
  fault_1,
  fault_2,
  fault_3,
  fault_4,
  bc_free,
  bc_nonreflect,
}

-- friction stuff
fault_fd  =    0.525
fault_Dc  =    0.4

glueid_treeid_faceid = {
  1,  3640, 2,
  1,  3641, 2,
  1,  3644, 2,
  1,  3645, 2,
  1,  3696, 2,
  1,  3697, 2,
  1,  3700, 2,
  1,  3701, 2,
  1,  3704, 2,
  1,  3705, 2,
  1,  3708, 2,
  1,  3709, 2,
  1,  3864, 2,
  1,  3865, 2,
  1,  3868, 2,
  1,  3869, 2,
  1,  3896, 2,
  1,  3897, 2,
  1,  3900, 2,
  1,  3901, 2,
  1,  3920, 2,
  1,  3921, 2,
  1,  3924, 2,
  1,  3925, 2,
  1,  3928, 2,
  1,  3929, 2,
  1,  3932, 2,
  1,  3933, 2,
  1,  3952, 2,
  1,  3953, 2,
  1,  3956, 2,
  1,  3957, 2,
  1,  7216, 2,
  1,  7217, 2,
  1,  7220, 2,
  1,  7221, 2,
  1,  7224, 2,
  1,  7225, 2,
  1,  7228, 2,
  1,  7229, 2,
  1,  7280, 2,
  1,  7281, 2,
  1,  7284, 2,
  1,  7285, 2,
  1,  7288, 2,
  1,  7289, 2,
  1,  7292, 2,
  1,  7293, 2,
  1,  7440, 2,
  1,  7441, 2,
  1,  7444, 2,
  1,  7445, 2,
  1,  7448, 2,
  1,  7449, 2,
  1,  7452, 2,
  1,  7453, 2,
  1,  7472, 2,
  1,  7473, 2,
  1,  7476, 2,
  1,  7477, 2,
  1,  7480, 2,
  1,  7484, 2,
  1,  7504, 2,
  1,  7505, 2,
  1,  7508, 2,
  1,  7509, 2,
  1,  7512, 2,
  1,  7513, 2,
  1,  7516, 2,
  1,  7517, 2,
  1,  7537, 2,
  1,  7541, 2,
  1,  7544, 2,
  1,  7545, 2,
  1,  7548, 2,
  1,  7549, 2,
  1,  7728, 2,
  1,  7729, 2,
  1,  7732, 2,
  1,  7733, 2,
  1,  7736, 2,
  1,  7737, 2,
  1,  7740, 2,
  1,  7741, 2,
  1,  7792, 2,
  1,  7793, 2,
  1,  7796, 2,
  1,  7797, 2,
  1,  7952, 2,
  1,  7953, 2,
  1,  7956, 2,
  1,  7957, 2,
  1,  7960, 2,
  1,  7961, 2,
  1,  7964, 2,
  1,  7965, 2,
  1,  7992, 2,
  1,  7993, 2,
  1,  7996, 2,
  1,  7997, 2,
  1,  8016, 2,
  1,  8017, 2,
  1,  8020, 2,
  1,  8021, 2,
  1,  8048, 2,
  1,  8049, 2,
  1,  8052, 2,
  1,  8053, 2,
  1, 11032, 2,
  1, 11033, 2,
  1, 11036, 2,
  1, 11037, 2,
  1, 11064, 2,
  1, 11065, 2,
  1, 11068, 2,
  1, 11069, 2,
  1, 11088, 2,
  1, 11089, 2,
  1, 11092, 2,
  1, 11093, 2,
  1, 11096, 2,
  1, 11097, 2,
  1, 11100, 2,
  1, 11101, 2,
  1, 11120, 2,
  1, 11121, 2,
  1, 11124, 2,
  1, 11125, 2,
  1, 11128, 2,
  1, 11129, 2,
  1, 11132, 2,
  1, 11133, 2,
  1, 11792, 2,
  1, 11793, 2,
  1, 11796, 2,
  1, 11797, 2,
  1, 11800, 2,
  1, 11801, 2,
  1, 11804, 2,
  1, 11805, 2,
  1, 11824, 2,
  1, 11825, 2,
  1, 11828, 2,
  1, 11829, 2,
  1, 11832, 2,
  1, 11833, 2,
  1, 11836, 2,
  1, 11837, 2,
  1, 11856, 2,
  1, 11857, 2,
  1, 11860, 2,
  1, 11861, 2,
  1, 11864, 2,
  1, 11865, 2,
  1, 11868, 2,
  1, 11869, 2,
  1, 11888, 2,
  1, 11889, 2,
  1, 11892, 2,
  1, 11893, 2,
  1, 11896, 2,
  1, 11897, 2,
  1, 11900, 2,
  1, 11901, 2,
  1, 12048, 2,
  1, 12049, 2,
  1, 12052, 2,
  1, 12053, 2,
  1, 12056, 2,
  1, 12057, 2,
  1, 12060, 2,
  1, 12061, 2,
  1, 12080, 2,
  1, 12081, 2,
  1, 12084, 2,
  1, 12085, 2,
  1, 12088, 2,
  1, 12089, 2,
  1, 12092, 2,
  1, 12093, 2,
  1, 12112, 2,
  1, 12113, 2,
  1, 12116, 2,
  1, 12117, 2,
  1, 12144, 2,
  1, 12145, 2,
  1, 12148, 2,
  1, 12149, 2,
  2,  3960, 2,
  2,  3961, 2,
  2,  3964, 2,
  2,  3965, 2,
  3,  7481, 2,
  3,  7485, 2,
  3,  7536, 2,
  3,  7540, 2,
  4,  7984, 2,
  4,  7985, 2,
  4,  7988, 2,
  4,  7989, 2,
  1,  3626, 3,
  1,  3627, 3,
  1,  3630, 3,
  1,  3631, 3,
  1,  3682, 3,
  1,  3683, 3,
  1,  3686, 3,
  1,  3687, 3,
  1,  3690, 3,
  1,  3691, 3,
  1,  3694, 3,
  1,  3695, 3,
  1,  3850, 3,
  1,  3851, 3,
  1,  3854, 3,
  1,  3855, 3,
  1,  3882, 3,
  1,  3883, 3,
  1,  3886, 3,
  1,  3887, 3,
  1,  3906, 3,
  1,  3907, 3,
  1,  3910, 3,
  1,  3911, 3,
  1,  3914, 3,
  1,  3915, 3,
  1,  3918, 3,
  1,  3919, 3,
  1,  3938, 3,
  1,  3939, 3,
  1,  3942, 3,
  1,  3943, 3,
  1,  7202, 3,
  1,  7203, 3,
  1,  7206, 3,
  1,  7207, 3,
  1,  7210, 3,
  1,  7211, 3,
  1,  7214, 3,
  1,  7215, 3,
  1,  7266, 3,
  1,  7267, 3,
  1,  7270, 3,
  1,  7271, 3,
  1,  7274, 3,
  1,  7275, 3,
  1,  7278, 3,
  1,  7279, 3,
  1,  7426, 3,
  1,  7427, 3,
  1,  7430, 3,
  1,  7431, 3,
  1,  7434, 3,
  1,  7435, 3,
  1,  7438, 3,
  1,  7439, 3,
  1,  7458, 3,
  1,  7459, 3,
  1,  7462, 3,
  1,  7463, 3,
  1,  7466, 3,
  1,  7470, 3,
  1,  7490, 3,
  1,  7491, 3,
  1,  7494, 3,
  1,  7495, 3,
  1,  7498, 3,
  1,  7499, 3,
  1,  7502, 3,
  1,  7503, 3,
  1,  7523, 3,
  1,  7527, 3,
  1,  7530, 3,
  1,  7531, 3,
  1,  7534, 3,
  1,  7535, 3,
  1,  7714, 3,
  1,  7715, 3,
  1,  7718, 3,
  1,  7719, 3,
  1,  7722, 3,
  1,  7723, 3,
  1,  7726, 3,
  1,  7727, 3,
  1,  7778, 3,
  1,  7779, 3,
  1,  7782, 3,
  1,  7783, 3,
  1,  7938, 3,
  1,  7939, 3,
  1,  7942, 3,
  1,  7943, 3,
  1,  7946, 3,
  1,  7947, 3,
  1,  7950, 3,
  1,  7951, 3,
  1,  7978, 3,
  1,  7979, 3,
  1,  7982, 3,
  1,  7983, 3,
  1,  8002, 3,
  1,  8003, 3,
  1,  8006, 3,
  1,  8007, 3,
  1,  8034, 3,
  1,  8035, 3,
  1,  8038, 3,
  1,  8039, 3,
  1, 11018, 3,
  1, 11019, 3,
  1, 11022, 3,
  1, 11023, 3,
  1, 11050, 3,
  1, 11051, 3,
  1, 11054, 3,
  1, 11055, 3,
  1, 11074, 3,
  1, 11075, 3,
  1, 11078, 3,
  1, 11079, 3,
  1, 11082, 3,
  1, 11083, 3,
  1, 11086, 3,
  1, 11087, 3,
  1, 11106, 3,
  1, 11107, 3,
  1, 11110, 3,
  1, 11111, 3,
  1, 11114, 3,
  1, 11115, 3,
  1, 11118, 3,
  1, 11119, 3,
  1, 11778, 3,
  1, 11779, 3,
  1, 11782, 3,
  1, 11783, 3,
  1, 11786, 3,
  1, 11787, 3,
  1, 11790, 3,
  1, 11791, 3,
  1, 11810, 3,
  1, 11811, 3,
  1, 11814, 3,
  1, 11815, 3,
  1, 11818, 3,
  1, 11819, 3,
  1, 11822, 3,
  1, 11823, 3,
  1, 11842, 3,
  1, 11843, 3,
  1, 11846, 3,
  1, 11847, 3,
  1, 11850, 3,
  1, 11851, 3,
  1, 11854, 3,
  1, 11855, 3,
  1, 11874, 3,
  1, 11875, 3,
  1, 11878, 3,
  1, 11879, 3,
  1, 11882, 3,
  1, 11883, 3,
  1, 11886, 3,
  1, 11887, 3,
  1, 12034, 3,
  1, 12035, 3,
  1, 12038, 3,
  1, 12039, 3,
  1, 12042, 3,
  1, 12043, 3,
  1, 12046, 3,
  1, 12047, 3,
  1, 12066, 3,
  1, 12067, 3,
  1, 12070, 3,
  1, 12071, 3,
  1, 12074, 3,
  1, 12075, 3,
  1, 12078, 3,
  1, 12079, 3,
  1, 12098, 3,
  1, 12099, 3,
  1, 12102, 3,
  1, 12103, 3,
  1, 12130, 3,
  1, 12131, 3,
  1, 12134, 3,
  1, 12135, 3,
  2,  3946, 3,
  2,  3947, 3,
  2,  3950, 3,
  2,  3951, 3,
  3,  7467, 3,
  3,  7471, 3,
  3,  7522, 3,
  3,  7526, 3,
  4,  7970, 3,
  4,  7971, 3,
  4,  7974, 3,
  4,  7975, 3,
}