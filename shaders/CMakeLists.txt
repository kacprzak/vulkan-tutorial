find_program(
  CMAKE_GLSL_COMPILER
  NAMES "glslangValidator"
  HINTS "/usr/bin"
  DOC "GLSL compiler"
  )

set(CMAKE_GLSL_SOURCE_FILE_EXTENSIONS vert;frag;geom;tesc;tese;comp)
set(CMAKE_GLSL_OUTPUT_EXTENSION .spv)

function(add_spirv_shader OUTPUT_FILE INPUT_FILE)
  add_custom_command(
    OUTPUT ${OUTPUT_FILE}
    COMMAND "${CMAKE_GLSL_COMPILER}" ARGS "${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_FILE}" -V -o "${OUTPUT_FILE}"
    MAIN_DEPENDENCY ${INPUT_FILE}
    )
endfunction()

add_spirv_shader(vert.spv shader.vert)
add_spirv_shader(frag.spv shader.frag)

add_custom_target(shaders ALL
  DEPENDS vert.spv frag.spv
  )
