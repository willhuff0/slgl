#pragma once

#if !defined(NDEBUG)
#define RESOURCE_BASEPATH "../../../res"
#else
#define RESOURCE_BASEPATH "./res"
#endif

#define TERRAIN_ALBEDO_TEXTURE_PATH RESOURCE_BASEPATH "/terrain/albedo.jpg"
#define TERRAIN_METALLIC_TEXTURE_PATH RESOURCE_BASEPATH "/terrain/metallic.png"
#define TERRAIN_ROUGHNESS_TEXTURE_PATH RESOURCE_BASEPATH "/terrain/roughness.jpg"
#define TERRAIN_AO_TEXTURE_PATH RESOURCE_BASEPATH "/terrain/ao.jpg"

#define SKYBOX_HDRI_PATH RESOURCE_BASEPATH "/skybox.hdr"
