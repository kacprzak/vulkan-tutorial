project('vulkan-tutorial', 'cpp',
  default_options: ['cpp_std=c++20'])

subdir('src')
subdir('shaders')

# clangd
if build_machine.system() != 'windows'
   ln = find_program('ln')

   run_command(ln, '-sf',
               meson.project_build_root() / 'compile_commands.json',
               meson.project_source_root())
endif
