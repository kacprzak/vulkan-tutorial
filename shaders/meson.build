glslc = find_program('glslc')

shader_srcs = ['shader.vert', 'shader.frag']

custom_target('vert.spv',
  input: 'shader.vert',
  output: 'vert.spv',
  command: [glslc, '--target-env=vulkan1.0', '@INPUT@', '-o', '@OUTPUT@'],
  build_by_default: true)

custom_target('frag.spv',
  input: 'shader.frag',
  output: 'frag.spv',
  command: [glslc, '--target-env=vulkan1.0', '@INPUT@', '-o', '@OUTPUT@'],
  build_by_default: true)
