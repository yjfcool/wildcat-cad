/*******************************************************************************
* Copyright (c) 2007, 2008, CerroKai Development
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of CerroKai Development nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY CerroKai Development ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL CerroKai Development BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/


/*** Included Header Files ***/
#include "Utility/adapter.h"
#include "Utility/log_manager.h"


//Initialize static variables
bool WCAdapter::_isInitialized = false;
float WCAdapter::_version = 0.0;
bool *WCAdapter::_extensions = NULL;


//Initialize static limits
GLint WCAdapter::_maxGeometryOutputVertices = 0;
GLint WCAdapter::_max2DTextureSize = 0;


/*** Extension Definitions ***/
char* adapterExtensions[ADAPTER_EXTENSION_COUNT] = {
	"GL_ARB_multitexture",						// 0
	"GLX_ARB_get_proc_address",					// 1
	"GL_ARB_transpose_matrix",					// 2
	"WGL_ARB_buffer_region",					// 3
	"GL_ARB_multisample",						// 4
	"GLX_ARB_multisample",						// 5
	"WGL_ARB_multisample",						// 6
	"GL_ARB_texture_env_add",					// 7
	"GL_ARB_texture_cube_map",					// 8
	"WGL_ARB_extensions_string",				// 9
	"WGL_ARB_pixel_format",						// 10
	"WGL_ARB_make_current_read",				// 11
	"WGL_ARB_pbuffer",							// 12
	"GL_ARB_texture_compression",				// 13
	"GL_ARB_texture_border_clamp",				// 14
	"GL_ARB_point_parameters",					// 15
	"GL_ARB_vertex_blend",						// 16
	"GL_ARB_matrix_palette",					// 17
	"GL_ARB_texture_env_combine",				// 18
	"GL_ARB_texture_env_crossbar",				// 19
	"GL_ARB_texture_env_dot3",					// 20
	"WGL_ARB_render_texture",					// 21
	"GL_ARB_texture_mirrored_repeat",			// 22
	"GL_ARB_depth_texture",						// 23
	"GL_ARB_shadow",							// 24
	"GL_ARB_shadow_ambient",					// 25
	"GL_ARB_window_pos",						// 26
	"GL_ARB_vertex_program",					// 27
	"GL_ARB_fragment_program",					// 28
	"GL_ARB_vertex_buffer_object",				// 29
	"GL_ARB_occlusion_query",					// 30
	"GL_ARB_shader_objects",					// 31
	"GL_ARB_vertex_shader",						// 32
	"GL_ARB_fragment_shader",					// 33
	"GL_ARB_shading_language_100",				// 34
	"GL_ARB_texture_non_power_of_two",			// 35
	"GL_ARB_point_sprite",						// 36
	"GL_ARB_fragment_program_shadow",			// 37
	"GL_ARB_draw_buffers",						// 38
	"GL_ARB_texture_rectangle",					// 39
	"GL_ARB_color_buffer_float",				// 40		
	"WGL_ARB_pixel_format_float",				// 41
	"GLX_ARB_fbconfig_float",					// 42
	"GL_ARB_half_float_pixel",					// 43
	"GL_ARB_texture_float",						// 44
	"GL_ARB_pixel_buffer_object",				// 45
	//Vendor and EXT Extensions by number					
	"GL_EXT_abgr",								// 46
	"GL_EXT_blend_color",						// 47
	"GL_EXT_polygon_offset",					// 48
	"GL_EXT_texture",							// 49
	"GL_EXT_texture3D",							// 50
	"GL_SGIS_texture_filter4",					// 51
	"GL_EXT_subtexture",						// 52
	"GL_EXT_copy_texture",						// 53
	"GL_EXT_histogram",							// 54
	"GL_EXT_convolution",						// 55
	"GL_SGI_color_matrix",						// 56
	"GL_SGI_color_table",						// 57
	"GL_SGIS_pixel_texture",					// 58
	"GL_SGIX_pixel_texture",					// 59
	"GL_SGIS_texture4D",						// 60
	"GL_SGI_texture_color_table",				// 61
	"GL_EXT_cmyka",								// 62
	"GL_EXT_texture_object",					// 63
	"GL_SGIS_detail_texture",					// 64
	"GL_SGIS_sharpen_texture",					// 65
	"GL_EXT_packed_pixels",						// 66
	"GL_SGIS_texture_lod",						// 67
	"GL_SGIS_multisample",						// 68
	"GLX_SGIS_multisample",						// 69
	"GL_EXT_rescale_normal",					// 70
	"GLX_EXT_visual_info",						// 71
	"GL_EXT_vertex_array",						// 72
	"GL_EXT_misc_attribute",					// 73
	"GL_SGIS_generate_mipmap",					// 74
	"GL_SGIX_clipmap",							// 75	
	"GL_SGIX_shadow",							// 76
	"GL_SGIS_texture_edge_clamp",				// 77
	"GL_SGIS_texture_border_clamp",				// 78
	"GL_EXT_blend_minmax",						// 79
	"GL_EXT_blend_subtract",					// 80
	"GL_EXT_blend_logic_op",					// 81
	"GLX_SGI_swap_control",						// 82
	"GLX_SGI_video_sync",						// 83
	"GLX_SGI_make_current_read",				// 84
	"GLX_SGIX_video_source",					// 85
	"GLX_EXT_visual_rating",					// 86
	"GL_SGIX_interlace",						// 87
	"GLX_EXT_import_context",					// 88
	"GLX_SGIX_fbconfig",						// 89
	"GLX_SGIX_pbuffer",							// 90
	"GL_SGIS_texture_select",					// 91
	"GL_SGIX_sprite",							// 92
	"GL_SGIX_texture_multi_buffer",				// 93
	"GL_EXT_point_parameters",					// 94
	"GL_SGIX_instruments",						// 95
	"GL_SGIX_texture_scale_bias",				// 96
	"GL_SGIX_framezoom",						// 97
	"GL_SGIX_tag_sample_buffer",				// 98
	"GL_SGIX_reference_plane",					// 99
	"GL_SGIX_flush_raster",						// 100
	"GLX_SGI_cushion",							// 101
	"GL_SGIX_depth_texture",					// 102
	"GL_SGIS_fog_function",						// 103
	"GL_SGIX_fog_offset",						// 104
	"GL_HP_image_transform",					// 105
	"GL_HP_convolution_border_modes",			// 106
	"GL_SGIX_texture_add_env",					// 107
	"GL_EXT_color_subtable",					// 108
	"GLU_EXT_object_space_tess",				// 109
	"GL_PGI_vertex_hints",						// 110
	"GL_PGI_misc_hints",						// 111
	"GL_EXT_paletted_texture",					// 112
	"GL_EXT_clip_volume_hint",					// 113
	"GL_SGIX_list_priority",					// 114
	"GL_SGIX_ir_instrument1",					// 115
	"GLX_SGIX_video_resize",					// 116
	"GL_SGIX_texture_lod_bias",					// 117
	"GLU_SGI_filter4_parameters",				// 118
	"GLX_SGIX_dm_buffer",						// 119
	"GL_SGIX_shadow_ambient",					// 120
	"GLX_SGIX_swap_group",						// 121
	"GLX_SGIX_swap_barrier",					// 122
	"GL_EXT_index_texture",						// 123
	"GL_EXT_index_material",					// 124
	"GL_EXT_index_func",						// 125
	"GL_EXT_index_array_formats",				// 126
	"GL_EXT_compiled_vertex_array",				// 127
	"GL_EXT_cull_vertex",						// 128
	"GLU_EXT_nurbs_tessellator",				// 129
	"GL_SGIX_ycrcb",							// 130
	"GL_EXT_fragment_lighting",					// 131
	"GL_IBM_rasterpos_clip",					// 132
	"GL_HP_texture_lighting",					// 133
	"GL_EXT_draw_range_elements",				// 134
	"GL_WIN_phong_shading",						// 135
	"GL_WIN_specular_fog",						// 136
	"GLX_SGIS_color_range",						// 137
	"GL_SGIS_color_range",						// 138
	"GL_EXT_light_texture",						// 139
	"GL_SGIX_blend_alpha_minmax",				// 140
	"GL_EXT_scene_marker",						// 141
	"GLX_EXT_scene_marker",						// 142
	"GL_SGIX_pixel_texture_bits",				// 143
	"GL_EXT_bgra",								// 144
	"GL_SGIX_async",							// 145
	"GL_SGIX_async_pixel",						// 146
	"GL_SGIX_async_histogram",					// 147
	"GL_INTEL_texture_scissor",					// 148
	"GL_INTEL_parallel_arrays",					// 149
	"GL_HP_occlusion_test",						// 150
	"GL_EXT_pixel_transform",					// 151
	"GL_EXT_pixel_transform_color_table",		// 152
	"GL_EXT_shared_texture_palette",			// 153
	"GLX_SGIS_blended_overlay",					// 154
	"GL_EXT_separate_specular_color",			// 155
	"GL_EXT_secondary_color",					// 156
	"GL_EXT_texture_env",						// 157
	"GL_EXT_texture_perturb_normal",			// 158
	"GL_EXT_multi_draw_arrays",					// 159
	"GL_SUN_multi_draw_arrays",					// 160
	"GL_EXT_fog_coord",							// 161
	"GL_REND_screen_coordinates",				// 162
	"GL_EXT_coordinate_frame",					// 163
	"GL_EXT_texture_env_combine",				// 164
	"GL_APPLE_specular_vector",					// 165
	"GL_APPLE_transform_hint",					// 166
	"GL_SUNX_constant_data",					// 167
	"GL_SUN_global_alpha",						// 168
	"GL_SUN_triangle_list",						// 169
	"GL_SUN_vertex",							// 170
	"WGL_EXT_display_color_table",				// 171
	"WGL_EXT_extensions_string",				// 172
	"WGL_EXT_make_current_read",				// 173
	"WGL_EXT_pixel_format",						// 174
	"WGL_EXT_pbuffer",							// 175
	"WGL_EXT_swap_control",						// 176
	"GL_EXT_blend_func_separate",				// 177
	"GL_INGR_color_clamp",						// 178
	"GL_INGR_interlace_read",					// 179
	"GL_EXT_stencil_wrap",						// 180
	"WGL_EXT_depth_float",						// 181
	"GL_EXT_422_pixels",						// 182
	"GL_NV_texgen_reflection",					// 183
	"GL_SGIX_texture_range",					// 184
	"GL_SUN_convolution_border_modes",			// 185
	"GLX_SUN_get_transparent_index",			// 186
	"GL_EXT_texture_env_add",					// 187
	"GL_EXT_texture_lod_bias",					// 188
	"GL_EXT_texture_filter_anisotropic",		// 189
	"GL_EXT_vertex_weighting",					// 190
	"GL_NV_light_max_exponent",					// 191
	"GL_NV_vertex_array_range",					// 192
	"GL_NV_register_combiners",					// 193
	"GL_NV_fog_distance",						// 194
	"GL_NV_texgen_emboss",						// 195
	"GL_NV_blend_square",						// 196
	"GL_NV_texture_env_combine4",				// 197
	"GL_MESA_resize_buffers",					// 198
	"GL_MESA_window_pos",						// 199
	"GL_EXT_texture_compression_s3tc",			// 200
	"GL_IBM_cull_vertex",						// 201
	"GL_IBM_multimode_draw_arrays",				// 202
	"GL_IBM_vertex_array_lists",				// 203
	"GL_3DFX_texture_compression_FXT1",			// 204
	"GL_3DFX_multisample",						// 205
	"GL_3DFX_tbuffer",							// 206
	"WGL_EXT_multisample",						// 207
	"GL_EXT_multisample",						// 208
	"GL_SGIX_vertex_preclip",					// 209
	"GL_SGIX_vertex_preclip_hint",				// 210
	"GL_SGIX_resample",							// 211
	"GL_SGIS_texture_color_mask",				// 212
	"GLX_MESA_copy_sub_buffer",					// 213
	"GLX_MESA_pixmap_colormap",					// 214
	"GLX_MESA_release_buffers",					// 215
	"GLX_MESA_set_3dfx_mode",					// 216
	"GL_EXT_texture_env_dot3",					// 217
	"GL_ATI_texture_mirror_once",				// 218
	"GL_NV_fence",								// 219
	"GL_IBM_static_data",						// 220
	"GL_IBM_texture_mirrored_repeat",			// 221
	"GL_NV_evaluators",							// 222
	"GL_NV_packed_depth_stencil",				// 223
	"GL_NV_register_combiners2",				// 224
	"GL_NV_texture_compression_vtc",			// 225
	"GL_NV_texture_rectangle",					// 226
	"GL_NV_texture_shader",						// 227
	"GL_NV_texture_shader2",					// 228
	"GL_NV_vertex_array_range2",				// 229
	"GL_NV_vertex_program",						// 230	
	"GLX_SGIX_visual_select_group",				// 231
	"GL_SGIX_texture_coordinate_clamp",			// 232
	"GLX_OML_swap_method",						// 233
	"GLX_OML_sync_control",						// 234
	"GL_OML_interlace",							// 235
	"GL_OML_subsample",							// 236
	"GL_OML_resample",							// 237
	"WGL_OML_sync_control",						// 238
	"GL_NV_copy_depth_to_color",				// 239
	"GL_ATI_envmap_bumpmap",					// 240
	"GL_ATI_fragment_shader",					// 241
	"GL_ATI_pn_triangles",						// 242
	"GL_ATI_vertex_array_object",				// 243
	"GL_EXT_vertex_shader",						// 244
	"GL_ATI_vertex_streams",					// 245
	"WGL_I3D_digital_video_control",			// 246
	"WGL_I3D_gamma",							// 247
	"WGL_I3D_genlock",							// 248
	"WGL_I3D_image_buffer",						// 249
	"WGL_I3D_swap_frame_lock",					// 250
	"WGL_I3D_swap_frame_usage",					// 251
	"GL_ATI_element_array",						// 252
	"GL_SUN_mesh_array",						// 253
	"GL_SUN_slice_accum",						// 254
	"GL_NV_multisample_filter_hint",			// 255
	"GL_NV_depth_clamp",						// 256
	"GL_NV_occlusion_query",					// 257
	"GL_NV_point_sprite",						// 258
	"WGL_NV_render_depth_texture",				// 259
	"WGL_NV_render_texture_rectangle",			// 260
	"GL_NV_texture_shader3",					// 261
	"GL_NV_vertex_program1_1",					// 262
	"GL_EXT_shadow_funcs",						// 263
	"GL_EXT_stencil_two_side",					// 264
	"GL_ATI_text_fragment_shader",				// 265
	"GL_APPLE_client_storage",					// 266	
	"GL_APPLE_element_array",					// 267
	"GL_APPLE_fence",							// 268
	"GL_APPLE_vertex_array_object",				// 269
	"GL_APPLE_vertex_array_range",				// 270
	"GL_APPLE_ycbcr_422",						// 271
	"GL_S3_s3tc",								// 272
	"GL_ATI_draw_buffers",						// 273
	"WGL_ATI_pixel_format_float",				// 274
	"GL_ATI_texture_env_combine3",				// 275
	"GL_ATI_texture_float",						// 276
	"GL_NV_float_buffer",						// 277
	"WGL_NV_float_buffer",						// 278
	"GL_NV_fragment_program",					// 279
	"GL_NV_half_float",							// 280
	"GL_NV_pixel_data_range",					// 281
	"GL_NV_primitive_restart",					// 282
	"GL_NV_texture_expand_normal",				// 283
	"GL_NV_vertex_program2",					// 284
	"GL_ATI_map_object_buffer",					// 285
	"GL_ATI_separate_stencil",					// 286
	"GL_ATI_vertex_attrib_array_object",		// 287
	"GL_OES_byte_coordinates",					// 288
	"GL_OES_fixed_point",						// 289
	"GL_OES_single_precision",					// 290
	"GL_OES_compressed_paletted_texture",		// 291
	"GL_OES_read_format",						// 292
	"GL_OES_query_matrix",						// 293
	"GL_EXT_depth_bounds_test",					// 294
	"GL_EXT_texture_mirror_clamp",				// 295
	"GL_EXT_blend_equation_separate",			// 296
	"GL_MESA_pack_invert",						// 297
	"GL_MESA_ycbcr_texture",					// 298
	"GL_EXT_pixel_buffer_object",				// 299
	"GL_NV_fragment_program_option",			// 300
	"GL_NV_fragment_program2",					// 301
	"GL_NV_vertex_program2_option",				// 302
	"GL_NV_vertex_program3",					// 303
	"GLX_SGIX_hyperpipe",						// 304
	"GLX_MESA_agp_offset",						// 305
	"GL_EXT_texture_compression_dxt1",			// 306
	"GL_EXT_framebuffer_object",				// 307
	"GL_GREMEDY_string_marker",					// 308
	"GL_EXT_packed_depth_stencil",				// 309
	"WGL_3DL_stereo_control",					// 310
	"GL_EXT_stencil_clear_tag",					// 311
	"GL_EXT_texture_sRGB",						// 312
	"GL_EXT_framebuffer_blit",					// 313
	"GL_EXT_framebuffer_multisample",			// 314
	"GL_MESAX_texture_stack",					// 315
	"GL_EXT_timer_query",						// 316
	"GL_EXT_gpu_program_parameters",			// 317
	"GL_APPLE_flush_buffer_range",				// 318
	"GL_NV_gpu_program4",						// 319
	"GL_NV_geometry_program4",					// 320
	"GL_EXT_geometry_shader4",					// 321
	"GL_NV_vertex_program4",					// 322
	"GL_EXT_gpu_shader4",						// 323
	"GL_EXT_draw_instanced",					// 324
	"GL_EXT_packed_float",						// 325
	"WGL_EXT_pixel_format_packed_float",		// 326
	"GLX_EXT_fbconfig_packed_float",			// 327
	"GL_EXT_texture_array",						// 328
	"GL_EXT_texture_buffer_object",				// 329
	"GL_EXT_texture_compression_latc",			// 330
	"GL_EXT_texture_compression_rgtc",			// 331
	"GL_EXT_texture_shared_exponent",			// 332
	"GL_NV_depth_buffer_float",					// 333
	"GL_NV_fragment_program4",					// 334
	"GL_NV_framebuffer_multisample_coverage",	// 335
	"GL_EXT_framebuffer_sRGB",					// 336
	"GLX_EXT_framebuffer_sRGB",					// 337
	"WGL_EXT_framebuffer_sRGB",					// 338	
	"GL_NV_geometry_shader4",					// 339
	"GL_NV_parameter_buffer_object",			// 340
	"GL_EXT_draw_buffers2",						// 341
	"GL_NV_transform_feedback",					// 342
	"GL_EXT_bindable_uniform",					// 343
	"GL_EXT_texture_integer",					// 344
	"GLX_EXT_texture_from_pixmap",				// 345
	"GL_EXT_transform_feedback",				// 346
	"GL_APPLE_float_pixels"						// 347
};


/***********************************************~***************************************************/


 void WCAdapter::Initialize(void) {
	//Don't need multiple initializations
	if (WCAdapter::_isInitialized) return;

	//Set OpenGL version
	char* version = (char*)glGetString(GL_VERSION);
	//Get version
	switch (version[0]) {
		//Must be versions 1.3, 1.4, or 1.5
		case '1': if (version[2] == '5') WCAdapter::_version = 1.5;
			else if (version[2] == '4') WCAdapter::_version = 1.4;
			else WCAdapter::_version = 1.3;
			break;
		case '2': if (version[2] == '1') WCAdapter::_version = 2.1;
			else WCAdapter::_version = 2.0;
			break;
	}
	 
	//Get the list of supported extensions
	char* extensions = (char*)glGetString(GL_EXTENSIONS);
	//Allocate space for the extension array
	WCAdapter::_extensions = new bool[ADAPTER_EXTENSION_COUNT];
	for (int i=0; i<ADAPTER_EXTENSION_COUNT; i++) {
		WCAdapter::_extensions[i] = (strstr(extensions, adapterExtensions[i]) != NULL);
	}
		
	/*** Get Limits ***/

	//Maximum geometry output vertices - requires GeometryShader4
	if (WCAdapter::_extensions[321]) glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &(WCAdapter::_maxGeometryOutputVertices));
	else WCAdapter::_maxGeometryOutputVertices = 0;

	//Maximum 2D texture size
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &(WCAdapter::_max2DTextureSize));

	/*** End Limits ***/

	//Mark the adapter as initialized
	WCAdapter::_isInitialized = true;
	//Check for GL errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCAdapter::Initialize - GL Error: " << err);
	}
 }
 
 
void WCAdapter::Terminate(void) {
	//Delete the extensions array
	if (WCAdapter::_extensions != NULL) delete WCAdapter::_extensions;
}


 /***********************************************~***************************************************/
 
 