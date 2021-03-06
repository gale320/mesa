/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 1999-2008  Brian Paul   All Rights Reserved.
 * Copyright (c) 2008-2009 VMware, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


/**
 * \file texformat.c
 * Texture formats.
 *
 * \author Gareth Hughes
 * \author Brian Paul
 */


#include "context.h"
#include "enums.h"
#include "mtypes.h"
#include "texcompress.h"
#include "texformat.h"

#define RETURN_IF_SUPPORTED(f) do {		\
   if (ctx->TextureFormatSupported[f])		\
      return f;					\
} while (0)

/**
 * Choose an appropriate texture format given the format, type and
 * internalFormat parameters passed to glTexImage().
 *
 * \param ctx  the GL context.
 * \param target  a texture target (GL_TEXTURE_x)
 * \param internalFormat  user's prefered internal texture format.
 * \param format  incoming image pixel format.
 * \param type  incoming image data type.
 *
 * \return the closest gl_format for the given format/type arguments
 *
 * This is called via dd_function_table::ChooseTextureFormat.  Hardware
 * drivers may override this function with a specialized version.
 */
gl_format
_mesa_choose_tex_format(struct gl_context *ctx, GLenum target,
                        GLint internalFormat, GLenum format, GLenum type)
{
   (void) format;

   switch (internalFormat) {
   /* shallow RGBA formats */
   case 4:
   case GL_RGBA:
      if (type == GL_UNSIGNED_SHORT_4_4_4_4_REV) {
         RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB4444);
      } else if (type == GL_UNSIGNED_SHORT_1_5_5_5_REV) {
         RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB1555);
      } else if (type == GL_UNSIGNED_INT_2_10_10_10_REV) {
         RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB2101010);
      }
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;

   case GL_RGBA8:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;
   case GL_RGB5_A1:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB1555);
      break;
   case GL_RGBA2:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB4444_REV); /* just to test another format*/
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB4444);
      break;
   case GL_RGBA4:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB4444);
      break;

   /* deep RGBA formats */
   case GL_RGB10_A2:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB2101010);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;
   case GL_RGBA12:
   case GL_RGBA16:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;

   /* shallow RGB formats */
   case 3:
   case GL_RGB:
      if (type == GL_UNSIGNED_INT_2_10_10_10_REV) {
         RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB2101010);
      }
      /* fallthrough */
   case GL_RGB8:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XRGB8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;
   case GL_R3_G3_B2:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB332);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB565);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB565_REV);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XRGB8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;
   case GL_RGB4:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB565_REV); /* just to test another format */
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB565);
      break;
   case GL_RGB5:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB565);
      break;

   /* deep RGB formats */
   case GL_RGB10:
   case GL_RGB12:
   case GL_RGB16:
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR16161616_UNORM);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XRGB8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;

   /* Alpha formats */
   case GL_ALPHA:
   case GL_ALPHA4:
   case GL_ALPHA8:
      RETURN_IF_SUPPORTED(MESA_FORMAT_A8);
      break;

   case GL_ALPHA12:
   case GL_ALPHA16:
      RETURN_IF_SUPPORTED(MESA_FORMAT_A16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_A8);
      break;

   /* Luminance formats */
   case 1:
   case GL_LUMINANCE:
   case GL_LUMINANCE4:
   case GL_LUMINANCE8:
      RETURN_IF_SUPPORTED(MESA_FORMAT_L8);
      break;

   case GL_LUMINANCE12:
   case GL_LUMINANCE16:
      RETURN_IF_SUPPORTED(MESA_FORMAT_L16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_L8);
      break;

      /* Luminance/Alpha formats */
   case GL_LUMINANCE4_ALPHA4:
      RETURN_IF_SUPPORTED(MESA_FORMAT_AL44);
      RETURN_IF_SUPPORTED(MESA_FORMAT_AL88);
      break;

   case 2:
   case GL_LUMINANCE_ALPHA:
   case GL_LUMINANCE6_ALPHA2:
   case GL_LUMINANCE8_ALPHA8:
      RETURN_IF_SUPPORTED(MESA_FORMAT_AL88);
      break;

   case GL_LUMINANCE12_ALPHA4:
   case GL_LUMINANCE12_ALPHA12:
   case GL_LUMINANCE16_ALPHA16:
      RETURN_IF_SUPPORTED(MESA_FORMAT_AL1616);
      RETURN_IF_SUPPORTED(MESA_FORMAT_AL88);
      break;

   case GL_INTENSITY:
   case GL_INTENSITY4:
   case GL_INTENSITY8:
      RETURN_IF_SUPPORTED(MESA_FORMAT_I8);
      break;

   case GL_INTENSITY12:
   case GL_INTENSITY16:
      RETURN_IF_SUPPORTED(MESA_FORMAT_I16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_I8);
      break;

   case GL_DEPTH_COMPONENT:
   case GL_DEPTH_COMPONENT24:
   case GL_DEPTH_COMPONENT32:
      RETURN_IF_SUPPORTED(MESA_FORMAT_Z32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_X8_Z24);
      RETURN_IF_SUPPORTED(MESA_FORMAT_S8_Z24);
      break;
   case GL_DEPTH_COMPONENT16:
      RETURN_IF_SUPPORTED(MESA_FORMAT_Z16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_X8_Z24);
      RETURN_IF_SUPPORTED(MESA_FORMAT_S8_Z24);
      break;

   case GL_COMPRESSED_ALPHA_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_A8);
      break;
   case GL_COMPRESSED_LUMINANCE_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_L8);
      break;
   case GL_COMPRESSED_LUMINANCE_ALPHA_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_AL88);
      break;
   case GL_COMPRESSED_INTENSITY_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_I8);
      break;
   case GL_COMPRESSED_RGB_ARB:
      /* We don't use texture compression for 1D and 1D array textures.
       * For 1D textures, compressions doesn't buy us much.
       * For 1D ARRAY textures, there's complicated issues with updating
       * sub-regions on non-block boundaries with glCopyTexSubImage, among
       * other issues.  FWIW, the GL_EXT_texture_array extension prohibits
       * 1D ARRAY textures in S3TC format.
       */
      if (target != GL_TEXTURE_1D && target != GL_TEXTURE_1D_ARRAY) {
         if (ctx->Mesa_DXTn)
            RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_DXT1);
         RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_FXT1);
      }
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XRGB8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;
   case GL_COMPRESSED_RGBA_ARB:
      /* We don't use texture compression for 1D and 1D array textures. */
      if (target != GL_TEXTURE_1D && target != GL_TEXTURE_1D_ARRAY) {
         if (ctx->Mesa_DXTn)
            RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_DXT3); /* Not rgba_dxt1, see spec */
         RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FXT1);
      }
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;

   case GL_RGB565:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB565);
      break;

   case GL_YCBCR_MESA:
      if (type == GL_UNSIGNED_SHORT_8_8_MESA)
         RETURN_IF_SUPPORTED(MESA_FORMAT_YCBCR);
      else
         RETURN_IF_SUPPORTED(MESA_FORMAT_YCBCR_REV);
      break;

   /* For non-generic compressed format we assert two things:
    *
    * 1. The format has already been validated against the set of available
    *    extensions.
    *
    * 2. The driver only enables the extension if it supports all of the
    *    formats that are part of that extension.
    */
   case GL_COMPRESSED_RGB_FXT1_3DFX:
      return MESA_FORMAT_RGB_FXT1;
   case GL_COMPRESSED_RGBA_FXT1_3DFX:
      return MESA_FORMAT_RGBA_FXT1;
   case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
   case GL_RGB_S3TC:
   case GL_RGB4_S3TC:
      return MESA_FORMAT_RGB_DXT1;
   case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
      return MESA_FORMAT_RGBA_DXT1;
   case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
   case GL_RGBA_S3TC:
   case GL_RGBA4_S3TC:
      return MESA_FORMAT_RGBA_DXT3;
   case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
      return MESA_FORMAT_RGBA_DXT5;
   case GL_COMPRESSED_RED_RGTC1:
      return MESA_FORMAT_RED_RGTC1;
   case GL_COMPRESSED_SIGNED_RED_RGTC1:
      return MESA_FORMAT_SIGNED_RED_RGTC1;
   case GL_COMPRESSED_RG_RGTC2:
      return MESA_FORMAT_RG_RGTC2;
   case GL_COMPRESSED_SIGNED_RG_RGTC2:
      return MESA_FORMAT_SIGNED_RG_RGTC2;
   case GL_COMPRESSED_LUMINANCE_LATC1_EXT:
      return MESA_FORMAT_L_LATC1;
   case GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT:
      return MESA_FORMAT_SIGNED_L_LATC1;
   case GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT:
      return MESA_FORMAT_LA_LATC2;
   case GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT:
      return MESA_FORMAT_SIGNED_LA_LATC2;
   case GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI:
      return MESA_FORMAT_LA_LATC2;
   case GL_ETC1_RGB8_OES:
      return MESA_FORMAT_ETC1_RGB8;
   case GL_COMPRESSED_RGB8_ETC2:
      return MESA_FORMAT_ETC2_RGB8;
   case GL_COMPRESSED_SRGB8_ETC2:
      return MESA_FORMAT_ETC2_SRGB8;
   case GL_COMPRESSED_RGBA8_ETC2_EAC:
      return MESA_FORMAT_ETC2_RGBA8_EAC;
   case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
      return MESA_FORMAT_ETC2_SRGB8_ALPHA8_EAC;
   case GL_COMPRESSED_R11_EAC:
      return MESA_FORMAT_ETC2_R11_EAC;
   case GL_COMPRESSED_RG11_EAC:
      return MESA_FORMAT_ETC2_RG11_EAC;
   case GL_COMPRESSED_SIGNED_R11_EAC:
      return MESA_FORMAT_ETC2_SIGNED_R11_EAC;
   case GL_COMPRESSED_SIGNED_RG11_EAC:
      return MESA_FORMAT_ETC2_SIGNED_RG11_EAC;
   case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
      return MESA_FORMAT_ETC2_RGB8_PUNCHTHROUGH_ALPHA1;
   case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
      return MESA_FORMAT_ETC2_SRGB8_PUNCHTHROUGH_ALPHA1;
   case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
      return MESA_FORMAT_SRGB_DXT1;
   case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
      return MESA_FORMAT_SRGBA_DXT1;
   case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
      return MESA_FORMAT_SRGBA_DXT3;
   case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
      return MESA_FORMAT_SRGBA_DXT5;

   case GL_ALPHA16F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      break;
   case GL_ALPHA32F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      break;
   case GL_LUMINANCE16F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      break;
   case GL_LUMINANCE32F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      break;
   case GL_LUMINANCE_ALPHA16F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      break;
   case GL_LUMINANCE_ALPHA32F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      break;
   case GL_INTENSITY16F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      break;
   case GL_INTENSITY32F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      break;
   case GL_RGB16F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR16161616_FLOAT);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      break;
   case GL_RGB32F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR32323232_FLOAT);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      break;
   case GL_RGBA16F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      break;
   case GL_RGBA32F_ARB:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      break;

   case GL_RGB9_E5:
      /* GL_EXT_texture_shared_exponent -- just one format to support */
      ASSERT(ctx->TextureFormatSupported[MESA_FORMAT_RGB9_E5_FLOAT]);
      return MESA_FORMAT_RGB9_E5_FLOAT;

   case GL_R11F_G11F_B10F:
      /* GL_EXT_texture_packed_float -- just one format to support */
      ASSERT(ctx->TextureFormatSupported[MESA_FORMAT_R11_G11_B10_FLOAT]);
      return MESA_FORMAT_R11_G11_B10_FLOAT;

   case GL_DEPTH_STENCIL_EXT:
   case GL_DEPTH24_STENCIL8_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_Z24_S8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_S8_Z24);
      break;

   case GL_DEPTH_COMPONENT32F:
      ASSERT(ctx->TextureFormatSupported[MESA_FORMAT_Z32_FLOAT]);
      return MESA_FORMAT_Z32_FLOAT;
   case GL_DEPTH32F_STENCIL8:
      ASSERT(ctx->TextureFormatSupported[MESA_FORMAT_Z32_FLOAT_X24S8]);
      return MESA_FORMAT_Z32_FLOAT_X24S8;

   case GL_DUDV_ATI:
   case GL_DU8DV8_ATI:
      RETURN_IF_SUPPORTED(MESA_FORMAT_DUDV8);
      break;

   case GL_RED_SNORM:
   case GL_R8_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_R8);
      break;
   case GL_RG_SNORM:
   case GL_RG8_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RG88_REV);
      break;
   case GL_RGB_SNORM:
   case GL_RGB8_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBX8888);
      /* FALLTHROUGH */
   case GL_RGBA_SNORM:
   case GL_RGBA8_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888_REV);
      break;
   case GL_ALPHA_SNORM:
   case GL_ALPHA8_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_A8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888_REV);
      break;
   case GL_LUMINANCE_SNORM:
   case GL_LUMINANCE8_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_L8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBX8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888_REV);
      break;
   case GL_LUMINANCE_ALPHA_SNORM:
   case GL_LUMINANCE8_ALPHA8_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_AL88);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888_REV);
      break;
   case GL_INTENSITY_SNORM:
   case GL_INTENSITY8_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_I8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888_REV);
      break;
   case GL_R16_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_R16);
      break;
   case GL_RG16_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_GR1616);
      break;
   case GL_RGB16_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGB_16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR16161616_SNORM);
      /* FALLTHROUGH */
   case GL_RGBA16_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA_16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888_REV);
      break;
   case GL_ALPHA16_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_A16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA_16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888_REV);
      break;
   case GL_LUMINANCE16_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_L16);
      /* FALLTHROUGH */
   case GL_LUMINANCE16_ALPHA16_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_AL1616);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA_16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888_REV);
      break;
   case GL_INTENSITY16_SNORM:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_I16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA_16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SIGNED_RGBA8888_REV);
      break;

   case GL_SRGB_EXT:
   case GL_SRGB8_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SRGB8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SARGB8);
      break;
   case GL_SRGB_ALPHA_EXT:
   case GL_SRGB8_ALPHA8_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SRGBA8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SARGB8);
      break;
   case GL_SLUMINANCE_EXT:
   case GL_SLUMINANCE8_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SL8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SARGB8);
      break;
   case GL_SLUMINANCE_ALPHA_EXT:
   case GL_SLUMINANCE8_ALPHA8_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SLA8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SARGB8);
      break;
   case GL_COMPRESSED_SLUMINANCE_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SL8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SARGB8);
      break;
   case GL_COMPRESSED_SLUMINANCE_ALPHA_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_SLA8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SARGB8);
      break;
   case GL_COMPRESSED_SRGB_EXT:
      if (ctx->Mesa_DXTn)
         RETURN_IF_SUPPORTED(MESA_FORMAT_SRGB_DXT1);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SRGB8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SARGB8);
      break;
   case GL_COMPRESSED_SRGB_ALPHA_EXT:
      if (ctx->Mesa_DXTn)
         RETURN_IF_SUPPORTED(MESA_FORMAT_SRGBA_DXT3); /* Not srgba_dxt1, see spec */
      RETURN_IF_SUPPORTED(MESA_FORMAT_SRGBA8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_SARGB8);
      break;

   case GL_ALPHA8UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_UINT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT8);
      break;
   case GL_ALPHA16UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_UINT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT16);
      break;
   case GL_ALPHA32UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_UINT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT32);
      break;
   case GL_ALPHA8I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_INT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT8);
      break;
   case GL_ALPHA16I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_INT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT16);
      break;
   case GL_ALPHA32I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ALPHA_INT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT32);
      break;
   case GL_LUMINANCE8UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_UINT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT8);
      break;
   case GL_LUMINANCE16UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_UINT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT16);
      break;
   case GL_LUMINANCE32UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_UINT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT32);
      break;
   case GL_LUMINANCE8I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_INT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT8);
      break;
   case GL_LUMINANCE16I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_INT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT16);
      break;
   case GL_LUMINANCE32I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_INT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT32);
      break;
   case GL_LUMINANCE_ALPHA8UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_UINT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT8);
      break;
   case GL_LUMINANCE_ALPHA16UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_UINT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT16);
      break;
   case GL_LUMINANCE_ALPHA32UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_UINT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT32);
      break;
   case GL_LUMINANCE_ALPHA8I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_INT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT8);
      break;
   case GL_LUMINANCE_ALPHA16I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_INT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT16);
      break;
   case GL_LUMINANCE_ALPHA32I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_LUMINANCE_ALPHA_INT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT32);
      break;
   case GL_INTENSITY8UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_UINT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT8);
      break;
   case GL_INTENSITY16UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_UINT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT16);
      break;
   case GL_INTENSITY32UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_UINT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT32);
      break;
   case GL_INTENSITY8I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_INT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT8);
      break;
   case GL_INTENSITY16I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_INT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT16);
      break;
   case GL_INTENSITY32I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_INTENSITY_INT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT32);
      break;

   case GL_RGB8UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_UINT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR8888_UINT);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT8);
      break;
   case GL_RGB16UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_UINT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR16161616_UINT);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT16);
      break;
   case GL_RGB32UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_UINT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR32323232_UINT);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT32);
      break;
   case GL_RGB8I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_INT8);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR8888_SINT);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT8);
      break;
   case GL_RGB16I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_INT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR16161616_SINT);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT16);
      break;
   case GL_RGB32I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGB_INT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_XBGR32323232_SINT);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT32);
      break;
   case GL_RGBA8UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT8);
      break;
   case GL_RGBA16UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT16);
      break;
   case GL_RGBA32UI_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_UINT32);
      break;
   case GL_RGBA8I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT8);
      break;
   case GL_RGBA16I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT16);
      break;
   case GL_RGBA32I_EXT:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_INT32);
      break;

   case GL_R8:
   case GL_RED:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R8);
      break;

   case GL_COMPRESSED_RED:
      if (target != GL_TEXTURE_1D && target != GL_TEXTURE_1D_ARRAY)
         RETURN_IF_SUPPORTED(MESA_FORMAT_RED_RGTC1);
      RETURN_IF_SUPPORTED(MESA_FORMAT_R8);
      break;

   case GL_R16:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R16);
      break;

   case GL_RG:
   case GL_RG8:
      RETURN_IF_SUPPORTED(MESA_FORMAT_GR88);
      break;

   case GL_COMPRESSED_RG:
      if (target != GL_TEXTURE_1D && target != GL_TEXTURE_1D_ARRAY)
         RETURN_IF_SUPPORTED(MESA_FORMAT_RG_RGTC2);
      RETURN_IF_SUPPORTED(MESA_FORMAT_GR88);
      break;

   case GL_RG16:
      RETURN_IF_SUPPORTED(MESA_FORMAT_GR1616);
      break;

   case GL_R16F:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      break;
   case GL_R32F:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      break;
   case GL_RG16F:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      break;
   case GL_RG32F:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT32);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_FLOAT16);
      RETURN_IF_SUPPORTED(MESA_FORMAT_RGBA_FLOAT16);
      break;

   case GL_R8UI:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_UINT8);
      break;
   case GL_RG8UI:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_UINT8);
      break;
   case GL_R16UI:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_UINT16);
      break;
   case GL_RG16UI:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_UINT16);
      break;
   case GL_R32UI:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_UINT32);
      break;
   case GL_RG32UI:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_UINT32);
      break;
   case GL_R8I:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_INT8);
      break;
   case GL_RG8I:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_INT8);
      break;
   case GL_R16I:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_INT16);
      break;
   case GL_RG16I:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_INT16);
      break;
   case GL_R32I:
      RETURN_IF_SUPPORTED(MESA_FORMAT_R_INT32);
      break;
   case GL_RG32I:
      RETURN_IF_SUPPORTED(MESA_FORMAT_RG_INT32);
      break;

   case GL_RGB10_A2UI:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB2101010_UINT);
      RETURN_IF_SUPPORTED(MESA_FORMAT_ABGR2101010_UINT);
      break;

   case GL_BGRA:
      RETURN_IF_SUPPORTED(MESA_FORMAT_ARGB8888);
      break;
   }

   _mesa_problem(ctx, "unexpected format %s in _mesa_choose_tex_format()",
                 _mesa_lookup_enum_by_nr(internalFormat));
   return MESA_FORMAT_NONE;
}

GLboolean
_mesa_tex_target_is_array(GLenum target)
{
   switch (target) {
   case GL_TEXTURE_1D_ARRAY_EXT:
   case GL_TEXTURE_2D_ARRAY_EXT:
   case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
   case GL_TEXTURE_CUBE_MAP_ARRAY:
      return GL_TRUE;
   default:
      return GL_FALSE;
   }
}
