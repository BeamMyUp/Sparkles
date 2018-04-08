cmake_minimum_required (VERSION 2.8.3)
project(sparkles CXX)

add_subdirectory(ext ext_build)

find_package(GLEW REQUIRED)

include_directories(
  # Sparkles include files
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  # tinyformat string formatting library
  ${TFM_INCLUDE_DIR}
  # Eigen linear algebra library
  ${EIGEN_INCLUDE_DIR}
  # OpenEXR high dynamic range bitmap library
  ${OPENEXR_INCLUDE_DIRS}
  # Intel Thread Building Blocks
  ${TBB_INCLUDE_DIR}
  # Pseudorandom number generator
  ${PCG32_INCLUDE_DIR}
  # PugiXML parser
  ${PUGIXML_INCLUDE_DIR}
  # Helper functions for statistical hypothesis tests
  ${HYPOTHESIS_INCLUDE_DIR}
  # GLFW library for OpenGL context creation
  ${GLFW_INCLUDE_DIR}
  # GLEW library for accessing OpenGL functions
  ${GLEW_INCLUDE_DIR}
  # GLM library for glviewer math 
  ${GLM_INCLUDE_DIR}
  # NanoVG drawing library
  ${NANOVG_INCLUDE_DIR}
  # NanoGUI user interface library
  ${NANOGUI_INCLUDE_DIR}
  ${NANOGUI_EXTRA_INCS}
  # Portable filesystem API
  ${FILESYSTEM_INCLUDE_DIR}
)

set(SPARKLES_FILES
  # Header files
  include/nori/accelerators/accel.h
  include/nori/accelerators/bvh.h
  include/nori/bsdfs/bsdf.h
  include/nori/bsdfs/phong.h
  include/nori/cameras/camera.h
  include/nori/cameras/perspective.h
  include/nori/core/bbox.h
  include/nori/core/bitmap.h
  include/nori/core/block.h
  include/nori/core/color.h
  include/nori/core/common.h
  include/nori/core/dpdf.h
  include/nori/core/frame.h
  include/nori/core/gui.h
  include/nori/core/math.h
  include/nori/core/object.h
  include/nori/core/parser.h
  include/nori/core/proplist.h
  include/nori/core/ray.h
  include/nori/core/rfilter.h
  include/nori/core/scene.h
  include/nori/core/timer.h
  include/nori/core/transform.h
  include/nori/core/vector.h
  include/nori/emitters/area.h
  include/nori/emitters/directional.h
  include/nori/emitters/emitter.h
  include/nori/emitters/point.h
  include/nori/integrators/ao.h
  include/nori/integrators/direct.h
  include/nori/integrators/directMIS.h
  include/nori/integrators/integrator.h
  include/nori/integrators/normals.h
  include/nori/integrators/path.h
  include/nori/integrators/simple.h
  include/nori/integrators/volPath.h
  include/nori/mediums/medium.h
  include/nori/mediums/homogeneous.h
  include/nori/phases/phaseFunction.h
  include/nori/phases/isotropic.h
  include/nori/samplers/sampler.h
  include/nori/shapes/mesh.h
  include/nori/shapes/shape.h
  include/nori/shapes/sphere.h
  include/nori/warp/mis.h
  include/nori/warp/warp.h

  # Source code files
  src/accelerators/accel.cpp
  src/accelerators/bvh.cpp
  src/bsdfs/dielectric.cpp
  src/bsdfs/diffuse.cpp
  src/bsdfs/microfacet.cpp
  src/bsdfs/mirror.cpp
  src/bsdfs/phong.cpp
  src/cameras/perspective.cpp
  src/core/bitmap.cpp
  src/core/block.cpp
  src/core/chi2test.cpp
  src/core/common.cpp
  src/core/gui.cpp
  src/core/main.cpp
  src/core/math.cpp
  src/core/object.cpp
  src/core/parser.cpp
  src/core/proplist.cpp
  src/core/rfilter.cpp
  src/core/scene.cpp
  src/emitters/area.cpp
  src/emitters/directional.cpp
  src/emitters/emitter.cpp
  src/emitters/point.cpp
  src/integrators/ao.cpp
  src/integrators/direct.cpp
  src/integrators/directMIS.cpp
  src/integrators/integrator.cpp
  src/integrators/normals.cpp
  src/integrators/path.cpp
  src/integrators/simple.cpp
  src/integrators/volPath.cpp
  src/mediums/medium.cpp
  src/mediums/homogeneous.cpp
  src/phases/isotropic.cpp
  src/samplers/independent.cpp
  src/shapes/mesh.cpp
  src/shapes/obj.cpp
  src/shapes/shape.cpp
  src/shapes/sphere.cpp
  src/warp/ttest.cpp
  src/warp/mis.cpp
  src/warp/warp.cpp
 )

set(WARP_FILES
  include/nori/warp/warp.h
  include/nori/core/object.h

  src/warp/warp.cpp
  src/warp/warptest.cpp
  src/bsdfs/microfacet.cpp
  src/core/object.cpp
  src/core/proplist.cpp
  src/core/common.cpp
)

set(VIEWER_FILES
  include/glviewer/camera.h
  include/glviewer/shader.h
  include/glviewer/shaderProgram.h
  include/glviewer/viewer.h

  src/glviewer/camera.cpp
  src/glviewer/shader.cpp
  src/glviewer/shaderProgram.cpp
  src/glviewer/viewer.cpp
)
 
add_executable(sparkles ${SPARKLES_FILES} ${VIEWER_FILES})
add_executable(warptest ${WARP_FILES})

add_definitions(${NANOGUI_EXTRA_DEFS})

foreach(FILE ${SPARKLES_FILES}) 
  get_filename_component(PARENT_DIR "${FILE}" PATH)

  # skip src or include and changes /'s to \\'s
  string(REGEX REPLACE "(\\./)?(src|include/nori)/?" "" GROUP "${PARENT_DIR}")
  string(REPLACE "/" "\\" GROUP "${GROUP}")

  source_group("${GROUP}" FILES "${FILE}")
endforeach()

foreach(FILE ${VIEWER_FILES}) 
  get_filename_component(PARENT_DIR "${FILE}" PATH)

  # skip src or include and changes /'s to \\'s
  string(REGEX REPLACE "(\\./)?(src|include)/?" "" GROUP "${PARENT_DIR}")
  string(REPLACE "/" "\\" GROUP "${GROUP}")

  source_group("${GROUP}" FILES "${FILE}")
endforeach()

foreach(FILE ${WARP_FILES}) 
  get_filename_component(PARENT_DIR "${FILE}" PATH)

  # skip src or include and changes /'s to \\'s
  string(REGEX REPLACE "(\\./)?(src|include/nori)/?" "" GROUP "${PARENT_DIR}")
  string(REPLACE "/" "\\" GROUP "${GROUP}")

  source_group("${GROUP}" FILES "${FILE}")
endforeach()

target_link_libraries(sparkles tbb_static pugixml IlmImf nanogui ${GLEW_LIBRARIES} ${NANOGUI_EXTRA_LIBS})
target_link_libraries(warptest tbb_static nanogui ${NANOGUI_EXTRA_LIBS})

# vim: set et ts=2 sw=2 ft=cmake nospell:
