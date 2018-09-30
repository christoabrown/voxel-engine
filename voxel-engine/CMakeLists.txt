cmake_minimum_required(VERSION 2.8)

## section: Macro
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledBinary}\" /FI\"${PrecompiledBinary}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

## start setting
SET (this_target voxel-engine)
PROJECT(${this_target})
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(PkgConfig REQUIRED)
#FIND_PACKAGE(Freetype REQUIRED)

# PkgConfig searches
#PKG_SEARCH_MODULE(GLFW REQUIRED glfw3)

add_definitions(-DUNICODE -D_UNICODE)


## section: include directory
INCLUDE_DIRECTORIES(
  
  #$(ProjectDir)
	lib/msdfgen/include/
	${OPENGL_INCLUDE_DIRS}
	lib/glfw/include/
#	${GLEW_INCLUDE_DIRECTORY}
  lib/glad/include/
	lib/freetype/include/
  )

## section: source files
# Add your source files here (one file per line), please SORT in alphabetical order for future maintenance
SET (${this_target}_SOURCE_FILES
    camera.cpp
	chunk.cpp
	chunk_vao_manager.cpp
	client/input.cpp
	lib/fastnoise/FastNoiseSIMD.cpp
	lib/fastnoise/FastNoiseSIMD_avx2.cpp
	lib/fastnoise/FastNoiseSIMD_internal.cpp
	lib/fastnoise/FastNoiseSIMD_neon.cpp
	lib/fastnoise/FastNoiseSIMD_sse2.cpp
	lib/fastnoise/FastNoiseSIMD_sse41.cpp
	lib/msdfgen/core/Bitmap.cpp
	lib/msdfgen/core/Contour.cpp
	lib/msdfgen/core/edge-coloring.cpp
	lib/msdfgen/core/edge-segments.cpp
	lib/msdfgen/core/EdgeHolder.cpp
	lib/msdfgen/core/equation-solver.cpp
	lib/msdfgen/core/msdfgen.cpp
	lib/msdfgen/core/render-sdf.cpp
	lib/msdfgen/core/save-bmp.cpp
	lib/msdfgen/core/shape-description.cpp
	lib/msdfgen/core/Shape.cpp
	lib/msdfgen/core/SignedDistance.cpp
	lib/msdfgen/core/Vector2.cpp
	lib/msdfgen/ext/import-font.cpp
	lib/msdfgen/ext/import-svg.cpp
	lib/msdfgen/ext/save-png.cpp
	lib/msdfgen/lib/lodepng.cpp
	lib/msdfgen/lib/tinyxml2.cpp
	lib/glad/src/glad.c
	renderer.cpp
	stdafx.cpp
	text_renderer.cpp
	util.cpp
	vbo_arena.cpp
	client/window.cpp
	world.cpp
	world_gen.cpp
	world_mesher.cpp
	client/voxel-engine.cpp
	resource/resource_manager.cpp
	lib/jsoncpp/jsoncpp.cpp
    )

## section: header files
# Add your header files here(one file per line), please SORT in alphabetical order for future maintenance!
SET(${this_target}_HEADER_FILES
    block.h
	camera.h
	chunk.h
	chunk_vao_manager.h
	client/input.h
	lib/fastnoise/FastNoiseSIMD.h
	lib/fastnoise/FastNoiseSIMD_internal.h
	lib/glm/common.hpp
	lib/glm/detail/func_common.hpp
	lib/glm/detail/func_exponential.hpp
	lib/glm/detail/func_geometric.hpp
	lib/glm/detail/func_integer.hpp
	lib/glm/detail/func_matrix.hpp
	lib/glm/detail/func_packing.hpp
	lib/glm/detail/func_trigonometric.hpp
	lib/glm/detail/func_vector_relational.hpp
	lib/glm/detail/precision.hpp
	lib/glm/detail/setup.hpp
	lib/glm/detail/type_float.hpp
	lib/glm/detail/type_gentype.hpp
	lib/glm/detail/type_half.hpp
	lib/glm/detail/type_int.hpp
	lib/glm/detail/type_mat.hpp
	lib/glm/detail/type_mat2x2.hpp
	lib/glm/detail/type_mat2x3.hpp
	lib/glm/detail/type_mat2x4.hpp
	lib/glm/detail/type_mat3x2.hpp
	lib/glm/detail/type_mat3x3.hpp
	lib/glm/detail/type_mat3x4.hpp
	lib/glm/detail/type_mat4x2.hpp
	lib/glm/detail/type_mat4x3.hpp
	lib/glm/detail/type_mat4x4.hpp
	lib/glm/detail/type_vec.hpp
	lib/glm/detail/type_vec1.hpp
	lib/glm/detail/type_vec2.hpp
	lib/glm/detail/type_vec3.hpp
	lib/glm/detail/type_vec4.hpp
	lib/glm/detail/_features.hpp
	lib/glm/detail/_fixes.hpp
	lib/glm/detail/_noise.hpp
	lib/glm/detail/_swizzle.hpp
	lib/glm/detail/_swizzle_func.hpp
	lib/glm/detail/_vectorize.hpp
	lib/glm/exponential.hpp
	lib/glm/ext.hpp
	lib/glm/fwd.hpp
	lib/glm/geometric.hpp
	lib/glm/glm.hpp
	lib/glm/gtc/bitfield.hpp
	lib/glm/gtc/color_space.hpp
	lib/glm/gtc/constants.hpp
	lib/glm/gtc/epsilon.hpp
	lib/glm/gtc/functions.hpp
	lib/glm/gtc/integer.hpp
	lib/glm/gtc/matrix_access.hpp
	lib/glm/gtc/matrix_integer.hpp
	lib/glm/gtc/matrix_inverse.hpp
	lib/glm/gtc/matrix_transform.hpp
	lib/glm/gtc/noise.hpp
	lib/glm/gtc/packing.hpp
	lib/glm/gtc/quaternion.hpp
	lib/glm/gtc/random.hpp
	lib/glm/gtc/reciprocal.hpp
	lib/glm/gtc/round.hpp
	lib/glm/gtc/type_aligned.hpp
	lib/glm/gtc/type_precision.hpp
	lib/glm/gtc/type_ptr.hpp
	lib/glm/gtc/ulp.hpp
	lib/glm/gtc/vec1.hpp
	lib/glm/gtx/associated_min_max.hpp
	lib/glm/gtx/bit.hpp
	lib/glm/gtx/closest_point.hpp
	lib/glm/gtx/color_space.hpp
	lib/glm/gtx/color_space_YCoCg.hpp
	lib/glm/gtx/common.hpp
	lib/glm/gtx/compatibility.hpp
	lib/glm/gtx/component_wise.hpp
	lib/glm/gtx/dual_quaternion.hpp
	lib/glm/gtx/euler_angles.hpp
	lib/glm/gtx/extend.hpp
	lib/glm/gtx/extended_min_max.hpp
	lib/glm/gtx/fast_exponential.hpp
	lib/glm/gtx/fast_square_root.hpp
	lib/glm/gtx/fast_trigonometry.hpp
	lib/glm/gtx/gradient_paint.hpp
	lib/glm/gtx/handed_coordinate_space.hpp
	lib/glm/gtx/hash.hpp
	lib/glm/gtx/integer.hpp
	lib/glm/gtx/intersect.hpp
	lib/glm/gtx/io.hpp
	lib/glm/gtx/log_base.hpp
	lib/glm/gtx/matrix_cross_product.hpp
	lib/glm/gtx/matrix_decompose.hpp
	lib/glm/gtx/matrix_interpolation.hpp
	lib/glm/gtx/matrix_major_storage.hpp
	lib/glm/gtx/matrix_operation.hpp
	lib/glm/gtx/matrix_query.hpp
	lib/glm/gtx/matrix_transform_2d.hpp
	lib/glm/gtx/mixed_product.hpp
	lib/glm/gtx/norm.hpp
	lib/glm/gtx/normal.hpp
	lib/glm/gtx/normalize_dot.hpp
	lib/glm/gtx/number_precision.hpp
	lib/glm/gtx/optimum_pow.hpp
	lib/glm/gtx/orthonormalize.hpp
	lib/glm/gtx/perpendicular.hpp
	lib/glm/gtx/polar_coordinates.hpp
	lib/glm/gtx/projection.hpp
	lib/glm/gtx/quaternion.hpp
	lib/glm/gtx/range.hpp
	lib/glm/gtx/raw_data.hpp
	lib/glm/gtx/rotate_normalized_axis.hpp
	lib/glm/gtx/rotate_vector.hpp
	lib/glm/gtx/scalar_multiplication.hpp
	lib/glm/gtx/scalar_relational.hpp
	lib/glm/gtx/simd_mat4.hpp
	lib/glm/gtx/simd_quat.hpp
	lib/glm/gtx/simd_vec4.hpp
	lib/glm/gtx/spline.hpp
	lib/glm/gtx/std_based_type.hpp
	lib/glm/gtx/string_cast.hpp
	lib/glm/gtx/transform.hpp
	lib/glm/gtx/transform2.hpp
	lib/glm/gtx/type_aligned.hpp
	lib/glm/gtx/type_trait.hpp
	lib/glm/gtx/vector_angle.hpp
	lib/glm/gtx/vector_query.hpp
	lib/glm/gtx/wrap.hpp
	lib/glm/integer.hpp
	lib/glm/mat2x2.hpp
	lib/glm/mat2x3.hpp
	lib/glm/mat2x4.hpp
	lib/glm/mat3x2.hpp
	lib/glm/mat3x3.hpp
	lib/glm/mat3x4.hpp
	lib/glm/mat4x2.hpp
	lib/glm/mat4x3.hpp
	lib/glm/mat4x4.hpp
	lib/glm/matrix.hpp
	lib/glm/packing.hpp
	lib/glm/simd/common.h
	lib/glm/simd/exponential.h
	lib/glm/simd/geometric.h
	lib/glm/simd/integer.h
	lib/glm/simd/matrix.h
	lib/glm/simd/packing.h
	lib/glm/simd/platform.h
	lib/glm/simd/trigonometric.h
	lib/glm/simd/vector_relational.h
	lib/glm/trigonometric.hpp
	lib/glm/vec2.hpp
	lib/glm/vec3.hpp
	lib/glm/vec4.hpp
	lib/glm/vector_relational.hpp
	lib/msdfgen/core/arithmetics.hpp
	lib/msdfgen/core/Bitmap.h
	lib/msdfgen/core/Contour.h
	lib/msdfgen/core/edge-coloring.h
	lib/msdfgen/core/edge-segments.h
	lib/msdfgen/core/EdgeColor.h
	lib/msdfgen/core/EdgeHolder.h
	lib/msdfgen/core/equation-solver.h
	lib/msdfgen/core/render-sdf.h
	lib/msdfgen/core/save-bmp.h
	lib/msdfgen/core/shape-description.h
	lib/msdfgen/core/Shape.h
	lib/msdfgen/core/SignedDistance.h
	lib/msdfgen/core/Vector2.h
	lib/msdfgen/ext/import-font.h
	lib/msdfgen/ext/import-svg.h
	lib/msdfgen/ext/save-png.h
	lib/msdfgen/include/freetype/config/ftconfig.h
	lib/msdfgen/include/freetype/config/ftheader.h
	lib/msdfgen/include/freetype/config/ftmodule.h
	lib/msdfgen/include/freetype/config/ftoption.h
	lib/msdfgen/include/freetype/config/ftstdlib.h
	lib/msdfgen/include/freetype/freetype.h
	lib/msdfgen/include/freetype/ftbbox.h
	lib/msdfgen/include/freetype/ftbdf.h
	lib/msdfgen/include/freetype/ftbitmap.h
	lib/msdfgen/include/freetype/ftcache.h
	lib/msdfgen/include/freetype/ftchapters.h
	lib/msdfgen/include/freetype/fterrdef.h
	lib/msdfgen/include/freetype/fterrors.h
	lib/msdfgen/include/freetype/ftgasp.h
	lib/msdfgen/include/freetype/ftglyph.h
	lib/msdfgen/include/freetype/ftgxval.h
	lib/msdfgen/include/freetype/ftgzip.h
	lib/msdfgen/include/freetype/ftimage.h
	lib/msdfgen/include/freetype/ftincrem.h
	lib/msdfgen/include/freetype/ftlcdfil.h
	lib/msdfgen/include/freetype/ftlist.h
	lib/msdfgen/include/freetype/ftlzw.h
	lib/msdfgen/include/freetype/ftmac.h
	lib/msdfgen/include/freetype/ftmm.h
	lib/msdfgen/include/freetype/ftmodapi.h
	lib/msdfgen/include/freetype/ftmoderr.h
	lib/msdfgen/include/freetype/ftotval.h
	lib/msdfgen/include/freetype/ftoutln.h
	lib/msdfgen/include/freetype/ftpfr.h
	lib/msdfgen/include/freetype/ftrender.h
	lib/msdfgen/include/freetype/ftsizes.h
	lib/msdfgen/include/freetype/ftsnames.h
	lib/msdfgen/include/freetype/ftstroke.h
	lib/msdfgen/include/freetype/ftsynth.h
	lib/msdfgen/include/freetype/ftsystem.h
	lib/msdfgen/include/freetype/fttrigon.h
	lib/msdfgen/include/freetype/fttypes.h
	lib/msdfgen/include/freetype/ftwinfnt.h
	lib/msdfgen/include/freetype/ftxf86.h
	lib/msdfgen/include/freetype/t1tables.h
	lib/msdfgen/include/freetype/ttnameid.h
	lib/msdfgen/include/freetype/tttables.h
	lib/msdfgen/include/freetype/tttags.h
	lib/msdfgen/include/freetype/ttunpat.h
	lib/msdfgen/include/ft2build.h
	lib/msdfgen/include/lodepng.h
	lib/msdfgen/include/tinyxml2.h
	lib/msdfgen/msdfgen-ext.h
	lib/msdfgen/msdfgen.h
	lib/stb_image.h
	lib/glad/include/glad/glad.h
	map3d.h
	renderer.h
	safe_queue.h
	shader.h
	stdafx.h
	text_renderer.h
	util.h
	vbo_arena.h
	client/window.h
	world.h
	world_gen.h
	world_mesher.h
	resource/resource_manager.h
	lib/jsoncpp/json/json.h
	lib/jsoncpp/json/json-forwards.h
    )

SOURCE_GROUP("shaders" FILES 
	
	)
SOURCE_GROUP("Header Files\\fastnoise" FILES 
	lib/fastnoise/FastNoiseSIMD.h
	lib/fastnoise/FastNoiseSIMD_internal.h
	)
SOURCE_GROUP("Header Files\\msdfgen" FILES 
	lib/msdfgen/core/arithmetics.hpp
	lib/msdfgen/core/Bitmap.h
	lib/msdfgen/core/Contour.h
	lib/msdfgen/core/edge-coloring.h
	lib/msdfgen/core/edge-segments.h
	lib/msdfgen/core/EdgeColor.h
	lib/msdfgen/core/EdgeHolder.h
	lib/msdfgen/core/equation-solver.h
	lib/msdfgen/core/render-sdf.h
	lib/msdfgen/core/save-bmp.h
	lib/msdfgen/core/shape-description.h
	lib/msdfgen/core/Shape.h
	lib/msdfgen/core/SignedDistance.h
	lib/msdfgen/core/Vector2.h
	lib/msdfgen/ext/import-font.h
	lib/msdfgen/ext/import-svg.h
	lib/msdfgen/ext/save-png.h
	lib/msdfgen/include/freetype/config/ftconfig.h
	lib/msdfgen/include/freetype/config/ftheader.h
	lib/msdfgen/include/freetype/config/ftmodule.h
	lib/msdfgen/include/freetype/config/ftoption.h
	lib/msdfgen/include/freetype/config/ftstdlib.h
	lib/msdfgen/include/freetype/freetype.h
	lib/msdfgen/include/freetype/ftbbox.h
	lib/msdfgen/include/freetype/ftbdf.h
	lib/msdfgen/include/freetype/ftbitmap.h
	lib/msdfgen/include/freetype/ftcache.h
	lib/msdfgen/include/freetype/ftchapters.h
	lib/msdfgen/include/freetype/fterrdef.h
	lib/msdfgen/include/freetype/fterrors.h
	lib/msdfgen/include/freetype/ftgasp.h
	lib/msdfgen/include/freetype/ftglyph.h
	lib/msdfgen/include/freetype/ftgxval.h
	lib/msdfgen/include/freetype/ftgzip.h
	lib/msdfgen/include/freetype/ftimage.h
	lib/msdfgen/include/freetype/ftincrem.h
	lib/msdfgen/include/freetype/ftlcdfil.h
	lib/msdfgen/include/freetype/ftlist.h
	lib/msdfgen/include/freetype/ftlzw.h
	lib/msdfgen/include/freetype/ftmac.h
	lib/msdfgen/include/freetype/ftmm.h
	lib/msdfgen/include/freetype/ftmodapi.h
	lib/msdfgen/include/freetype/ftmoderr.h
	lib/msdfgen/include/freetype/ftotval.h
	lib/msdfgen/include/freetype/ftoutln.h
	lib/msdfgen/include/freetype/ftpfr.h
	lib/msdfgen/include/freetype/ftrender.h
	lib/msdfgen/include/freetype/ftsizes.h
	lib/msdfgen/include/freetype/ftsnames.h
	lib/msdfgen/include/freetype/ftstroke.h
	lib/msdfgen/include/freetype/ftsynth.h
	lib/msdfgen/include/freetype/ftsystem.h
	lib/msdfgen/include/freetype/fttrigon.h
	lib/msdfgen/include/freetype/fttypes.h
	lib/msdfgen/include/freetype/ftwinfnt.h
	lib/msdfgen/include/freetype/ftxf86.h
	lib/msdfgen/include/freetype/t1tables.h
	lib/msdfgen/include/freetype/ttnameid.h
	lib/msdfgen/include/freetype/tttables.h
	lib/msdfgen/include/freetype/tttags.h
	lib/msdfgen/include/freetype/ttunpat.h
	lib/msdfgen/include/ft2build.h
	lib/msdfgen/include/lodepng.h
	lib/msdfgen/include/tinyxml2.h
	lib/msdfgen/msdfgen.h
	lib/msdfgen/msdfgen-ext.h
	)
SOURCE_GROUP("Source Files" FILES 
	
	)
SOURCE_GROUP("Header Files" FILES 
	stdafx.h
	renderer.h
	shader.h
	lib/stb_image.h
	client/input.h
	camera.h
	chunk.h
	map3d.h
	safe_queue.h
	world.h
	world_gen.h
	block.h
	util.h
	vbo_arena.h
	chunk_vao_manager.h
	world_mesher.h
	textRenderer.h
	)
SOURCE_GROUP("Header Files\\glm" FILES 
	lib/glm/mat4x3.hpp
	lib/glm/mat4x4.hpp
	lib/glm/matrix.hpp
	lib/glm/packing.hpp
	lib/glm/trigonometric.hpp
	lib/glm/vec2.hpp
	lib/glm/vec3.hpp
	lib/glm/vec4.hpp
	lib/glm/vector_relational.hpp
	lib/glm/detail/func_common.hpp
	lib/glm/detail/func_exponential.hpp
	lib/glm/detail/func_geometric.hpp
	lib/glm/detail/func_integer.hpp
	lib/glm/detail/func_matrix.hpp
	lib/glm/detail/func_packing.hpp
	lib/glm/detail/func_trigonometric.hpp
	lib/glm/detail/func_vector_relational.hpp
	lib/glm/detail/precision.hpp
	lib/glm/detail/setup.hpp
	lib/glm/detail/type_float.hpp
	lib/glm/detail/type_gentype.hpp
	lib/glm/detail/type_half.hpp
	lib/glm/detail/type_int.hpp
	lib/glm/detail/type_mat.hpp
	lib/glm/detail/type_mat2x2.hpp
	lib/glm/detail/type_mat2x3.hpp
	lib/glm/detail/type_mat2x4.hpp
	lib/glm/detail/type_mat3x2.hpp
	lib/glm/detail/type_mat3x3.hpp
	lib/glm/detail/type_mat3x4.hpp
	lib/glm/detail/type_mat4x2.hpp
	lib/glm/detail/type_mat4x3.hpp
	lib/glm/detail/type_mat4x4.hpp
	lib/glm/detail/type_vec.hpp
	lib/glm/detail/type_vec1.hpp
	lib/glm/detail/type_vec2.hpp
	lib/glm/detail/type_vec3.hpp
	lib/glm/detail/type_vec4.hpp
	lib/glm/detail/_features.hpp
	lib/glm/detail/_fixes.hpp
	lib/glm/detail/_noise.hpp
	lib/glm/detail/_swizzle.hpp
	lib/glm/detail/_swizzle_func.hpp
	lib/glm/detail/_vectorize.hpp
	lib/glm/gtc/bitfield.hpp
	lib/glm/gtc/color_space.hpp
	lib/glm/gtc/constants.hpp
	lib/glm/gtc/epsilon.hpp
	lib/glm/gtc/functions.hpp
	lib/glm/gtc/integer.hpp
	lib/glm/gtc/matrix_access.hpp
	lib/glm/gtc/matrix_integer.hpp
	lib/glm/gtc/matrix_inverse.hpp
	lib/glm/gtc/matrix_transform.hpp
	lib/glm/gtc/noise.hpp
	lib/glm/gtc/packing.hpp
	lib/glm/gtc/quaternion.hpp
	lib/glm/gtc/random.hpp
	lib/glm/gtc/reciprocal.hpp
	lib/glm/gtc/round.hpp
	lib/glm/gtc/type_aligned.hpp
	lib/glm/gtc/type_precision.hpp
	lib/glm/gtc/type_ptr.hpp
	lib/glm/gtc/ulp.hpp
	lib/glm/gtc/vec1.hpp
	lib/glm/gtx/associated_min_max.hpp
	lib/glm/gtx/bit.hpp
	lib/glm/gtx/closest_point.hpp
	lib/glm/gtx/color_space.hpp
	lib/glm/gtx/color_space_YCoCg.hpp
	lib/glm/gtx/common.hpp
	lib/glm/gtx/compatibility.hpp
	lib/glm/gtx/component_wise.hpp
	lib/glm/gtx/dual_quaternion.hpp
	lib/glm/gtx/euler_angles.hpp
	lib/glm/gtx/extend.hpp
	lib/glm/gtx/extended_min_max.hpp
	lib/glm/gtx/fast_exponential.hpp
	lib/glm/gtx/fast_square_root.hpp
	lib/glm/gtx/fast_trigonometry.hpp
	lib/glm/gtx/gradient_paint.hpp
	lib/glm/gtx/handed_coordinate_space.hpp
	lib/glm/gtx/hash.hpp
	lib/glm/gtx/integer.hpp
	lib/glm/gtx/intersect.hpp
	lib/glm/gtx/io.hpp
	lib/glm/gtx/log_base.hpp
	lib/glm/gtx/matrix_cross_product.hpp
	lib/glm/gtx/matrix_decompose.hpp
	lib/glm/gtx/matrix_interpolation.hpp
	lib/glm/gtx/matrix_major_storage.hpp
	lib/glm/gtx/matrix_operation.hpp
	lib/glm/gtx/matrix_query.hpp
	lib/glm/gtx/matrix_transform_2d.hpp
	lib/glm/gtx/mixed_product.hpp
	lib/glm/gtx/norm.hpp
	lib/glm/gtx/normal.hpp
	lib/glm/gtx/normalize_dot.hpp
	lib/glm/gtx/number_precision.hpp
	lib/glm/gtx/optimum_pow.hpp
	lib/glm/gtx/orthonormalize.hpp
	lib/glm/gtx/perpendicular.hpp
	lib/glm/gtx/polar_coordinates.hpp
	lib/glm/gtx/projection.hpp
	lib/glm/gtx/quaternion.hpp
	lib/glm/gtx/range.hpp
	lib/glm/gtx/raw_data.hpp
	lib/glm/gtx/rotate_normalized_axis.hpp
	lib/glm/gtx/rotate_vector.hpp
	lib/glm/gtx/scalar_multiplication.hpp
	lib/glm/gtx/scalar_relational.hpp
	lib/glm/gtx/simd_mat4.hpp
	lib/glm/gtx/simd_quat.hpp
	lib/glm/gtx/simd_vec4.hpp
	lib/glm/gtx/spline.hpp
	lib/glm/gtx/std_based_type.hpp
	lib/glm/gtx/string_cast.hpp
	lib/glm/gtx/transform.hpp
	lib/glm/gtx/transform2.hpp
	lib/glm/gtx/type_aligned.hpp
	lib/glm/gtx/type_trait.hpp
	lib/glm/gtx/vector_angle.hpp
	lib/glm/gtx/vector_query.hpp
	lib/glm/gtx/wrap.hpp
	lib/glm/simd/common.h
	lib/glm/simd/exponential.h
	lib/glm/simd/geometric.h
	lib/glm/simd/integer.h
	lib/glm/simd/matrix.h
	lib/glm/simd/packing.h
	lib/glm/simd/platform.h
	lib/glm/simd/trigonometric.h
	lib/glm/simd/vector_relational.h
	lib/glm/common.hpp
	lib/glm/exponential.hpp
	lib/glm/ext.hpp
	lib/glm/fwd.hpp
	lib/glm/geometric.hpp
	lib/glm/glm.hpp
	lib/glm/integer.hpp
	lib/glm/mat2x2.hpp
	lib/glm/mat2x3.hpp
	lib/glm/mat2x4.hpp
	lib/glm/mat3x2.hpp
	lib/glm/mat3x3.hpp
	lib/glm/mat3x4.hpp
	lib/glm/mat4x2.hpp
	)


## section: precompiled header	
#ADD_MSVC_PRECOMPILED_HEADER("precompiled.h" "precompiled.cpp" MySources)
#ADD_LIBRARY(MyLibrary ${MySources})

SET_SOURCE_FILES_PROPERTIES(${this_target}_HEADER_FILES
                            PROPERTIES HEADER_FILE_ONLY TRUE)
LIST(APPEND ${this_target}_SOURCE_FILES ${${this_target}_HEADER_FILES})

## section: add definitions
#   add prefix -D. example> -DSHP
#  - DO NOT add  the following definitions(already defined in ${OSP_DEFINITIONS}:
#   -DSHP, -DWIN32, -D_WINDOWS, -D_DEBUG, -D_USRDLL, -D_CRT_SECURE_NO_DEPRECATE
ADD_DEFINITIONS(
#   -DWIN32
#	-DNDEBUG
#	-D_WINDOWS
	-D_ITERATOR_DEBUG_LEVEL=0
    )

## section: add target

ADD_EXECUTABLE(${this_target} ${${this_target}_SOURCE_FILES})

## Copies shaders folder on post build
ADD_CUSTOM_COMMAND(TARGET voxel-engine PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:voxel-engine>/shaders)
## Copies textures folder
FILE(GLOB ResourceTextures ${CMAKE_SOURCE_DIR}/resource/textures/*)
ADD_CUSTOM_COMMAND(TARGET voxel-engine PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:voxel-engine>/resource/textures)
#FOREACH(ResourceTexture ${ResourceTextures})
#ADD_CUSTOM_COMMAND(TARGET voxel-engine PRE_BUILD
#                   COMMAND mogrify -format bmp -path $<TARGET_FILE_DIR:voxel-engine>/resource/textures ${ResourceTexture})
#ENDFOREACH()
## Copies fonts
ADD_CUSTOM_COMMAND(TARGET voxel-engine PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
						${CMAKE_SOURCE_DIR}/resource/fonts $<TARGET_FILE_DIR:voxel-engine>/resource/fonts)
## Create font cache directory
ADD_CUSTOM_COMMAND(TARGET voxel-engine PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:voxel-engine>/resource/fonts/msdf)
## Create font cache directory
ADD_CUSTOM_COMMAND(TARGET voxel-engine PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:voxel-engine>/resource/fonts/info)

## section: add dependency
# dependency determines overall build order.
#ADD_DEPENDENCIES(${this_target} 
#   ${OPENGL_LIBRARIES}
#	)

## section: set link libraries
TARGET_LINK_LIBRARIES( ${this_target}
        ${OPENGL_LIBRARIES}
		$(ProjectDir)lib/glfw/lib/glfw3.lib
		$(ProjectDir)lib/freetype/win64/freetype.lib
		${CMAKE_DL_LIBS}
        )
