/*
   nlame - an alternative API for libmp3lame
   copyright (c) 2001-2009 Michael Fink
   Copyright (c) 2004 DeXT

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
  
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id: nlame.c,v 1.30 2009/11/02 21:02:17 vividos Exp $

*/
/*! \file nlame.c

   \brief nlame API implementation

   In this file the functions of the nlame API are implemented. Most functions
   are just calls to the original functions.

*/

/* needed includes */
#include "nlame.h"
#include "lame.h"
#include <stdlib.h>
#include <string.h>


/*! quality values to use in a call to nlame_var_set_int(nle_var_quality,x)
    note: these are now internal and can't be reached from nlame.h anymore.
          please use nle_var_quality_value_high and nle_var_quality_value_fast
          to query for the exact values.
*/
typedef enum
{
   nle_quality_high=2,   
   nle_quality_fast=7,

} nlame_quality_value;



/* nlame API functions */

/*! returns the version string, according to the nlame_lame_version_type
    that is passed as parameter */
const char* nlame_lame_version_get(nlame_lame_version_type type)
{
   const char* str="";
   switch(type)
   {
   case nle_lame_version_normal:
      str = get_lame_version();
      break;

   case nle_lame_version_short:
      str = get_lame_short_version();
      break;

   case nle_lame_version_psy:
      str = get_psy_version();
      break;
   }

   return str;
}

/*! returns the numerical version number of libmp3lame
    when libmp3lame is not an alpha version, the alpha value is 0
    when libmp3lame is not a beta version, the beta value is 0 */
void nlame_lame_version_get_num(int* major, int* minor, int* alpha, int* beta)
{
   lame_version_t ver;
   get_lame_version_numerical(&ver);
   *major = ver.major; *minor = ver.minor;
   *alpha = ver.alpha; *beta = ver.beta;
}

/*! returns the numerical version number of the psychoacoustic model
    when psymodel is not an alpha version, the alpha value is 0
    when psymodel is not a beta version, the beta value is 0 */
void nlame_lame_version_get_psy_num(int* major, int* minor, int* alpha, int* beta)
{
   lame_version_t ver;
   get_lame_version_numerical(&ver);
   *major = ver.psy_major; *minor = ver.psy_minor;
   *alpha = ver.psy_alpha; *beta = ver.psy_beta;
}


static char nlame_string_compiler[256] = "\0";

/*! note that the first call to this function is not reentrant!
    infos about ICL macros were take from http://predef.sourceforge.net/precomp.html
*/
const char* nlame_get_string_compiler()
{
   if (nlame_string_compiler[0] == 0)
   {
#if defined(__ICL) || defined(__INTEL_COMPILER)

      _snprintf(nlame_string_compiler, sizeof(nlame_string_compiler)/sizeof(*nlame_string_compiler),
         "Intel C/C++ Compiler (%d.%d.%d)"
#ifdef __INTEL_COMPILER_BUILD_DATE
         " %4d%02d%02d"
#endif
         ,
#if defined(__INTEL_COMPILER)
         __INTEL_COMPILER/100, (__INTEL_COMPILER%100)/10, (__INTEL_COMPILER%10)
#elif defined(__ICL)
         __ICL/100, (__ICL%100)/10, (__ICL%10)
#endif
#ifdef __INTEL_COMPILER_BUILD_DATE
         ,
         __INTEL_COMPILER_BUILD_DATE/10000,
         (__INTEL_COMPILER_BUILD_DATE%10000)/100,
         __INTEL_COMPILER_BUILD_DATE%100
#endif
         );

#elif defined(_MSC_VER)

   _snprintf(nlame_string_compiler, sizeof(nlame_string_compiler)/sizeof(*nlame_string_compiler),
#if (_MSC_VER <= 1200)
      "Microsoft Visual C++ 6"
#elif (_MSC_VER <= 1300)
      "Microsoft Visual C++ .NET 2002"
#elif (_MSC_VER <= 1310)
      "Microsoft Visual C++ .NET 2003"
#elif (_MSC_VER <= 1400)
      "Microsoft Visual C++ 2005"
#elif (_MSC_VER <= 1500)
      "Microsoft Visual C++ 2008"
#elif (_MSC_VER <= 1600)
      "Microsoft Visual C++ 2010"
#elif (_MSC_VER <= 1700)
      "Microsoft Visual C++ 2012"
#else
      "Microsoft Visual C++ > 2012"
#endif
      " (%d.%d.%d.%d)",
      _MSC_VER/100, (_MSC_VER%100)/10, (_MSC_VER%10), _MSC_FULL_VER%100000);

#else
      strcpy(nlame_string_compiler, "unknown compiler");
#endif
#ifdef _DEBUG
   strcat(nlame_string_compiler, "; DEBUG");
#endif

   }

   return nlame_string_compiler;
}


static char nlame_cpu_features[256] = "\0";

/*! note that the first call to this function is not reentrant! */
const char* nlame_get_cpu_features()
{
   if (nlame_cpu_features[0]==0)
   {
//      extern int  has_i387  ( void );
      extern int  has_MMX   ( void );
      extern int  has_3DNow ( void );
      extern int  has_SSE  ( void );
      extern int  has_SSE2 ( void );
//      extern int  has_SIMD  ( void );
//      extern int  has_SIMD2 ( void );

      // check for processor feature
      if (has_3DNow()) strcat(nlame_cpu_features,"3DNow ");
//      if (has_SIMD())  strcat(nlame_cpu_features,"SIMD ");
//      if (has_SIMD2()) strcat(nlame_cpu_features,"SIMD2 ");
      if (has_SSE())  strcat(nlame_cpu_features,"SSE ");
      if (has_SSE2()) strcat(nlame_cpu_features,"SSE2 ");
      if (has_MMX())   strcat(nlame_cpu_features,"MMX ");
//      if (has_i387())  strcat(nlame_cpu_features,"i387-FPU ");

      if (nlame_cpu_features[0]==0)
         strcat(nlame_cpu_features,"none");
      else
         nlame_cpu_features[strlen(nlame_cpu_features)-1]=0;
   }

   return nlame_cpu_features;
}


/*! just returns the specified string */
const char* nlame_lame_string_get(nlame_lame_string_type type)
{
   const char* str="";
   switch(type)
   {
   case nle_lame_string_url:
      str = get_lame_url();
      break;

   case nle_lame_string_features:
      {
         lame_version_t ver;
         get_lame_version_numerical(&ver);
         str = ver.features;
      }
      break;

   case nle_lame_string_compiler:
      str = nlame_get_string_compiler();
      break;

   case nle_lame_string_cpu_features:
      str = nlame_get_cpu_features();
      break;
   }

   return str;
}

/*! contains the global flags struct */
struct nlame_struct
{
   lame_global_flags* lgf; /*!< global flags instance */
};

nlame_instance_t* nlame_new()
{
   nlame_instance_t* inst=NULL;
   inst = malloc(sizeof(struct nlame_struct));
   inst->lgf = lame_init();
   return inst;
}

void nlame_delete(nlame_instance_t* inst)
{
   lame_close(inst->lgf);
   free(inst);
}


/* as we are lazy people, we let macros do the work */

/*! sets a variable according to type, via a lame_set_* function */
#define nlame_var_set_switch_type(type,func)    \
   case type:                                   \
      ret = lame_set_##func##(inst->lgf,value); \
      break;

/*! sets a variable according to type, via a lame_set_* function;
    uses the val given */
#define nlame_var_set_switch_type_val(type,func,val) \
   case type:                                   \
      ret = lame_set_##func##(inst->lgf,val);   \
      break;

/*! returns a variable according to type, via a lame_get_* function */
#define nlame_var_get_switch_type(type,func)    \
   case type:                                   \
      val = lame_get_##func##(inst->lgf);       \
      break;

/*! returns a variable to a lvalue, according to type */
#define nlame_var_get_switch_type_lval(type,func,lval) \
   case type:                                   \
      lval lame_get_##func##(inst->lgf);        \
      break;


/* preset functions */

/*! set alt-preset VBR type */
int lame_set_alt_preset_vbr(lame_global_flags* gfp, int type)
{
   if ((type < nle_preset_first || type > nle_preset_last) &&
       (type < nle_preset_vx_first || type > nle_preset_vx_last))
      return -1;
   return lame_set_preset(gfp,type);
}

/*! set alt-preset ABR bitrate */
int lame_set_alt_preset_abr(lame_global_flags* gfp, int abr)
{
   if (abr<8 || abr>320)
      return -1;

   return lame_set_preset(gfp,abr);
}

/*! set alt-preset CBR bitrate */
int lame_set_alt_preset_cbr(lame_global_flags* gfp, int cbr)
{
   int ret = lame_set_alt_preset_abr(gfp,cbr);
   lame_set_VBR(gfp, vbr_off);
   return ret;
}


/*! sets the integer variable value for the type
    if an error occured, this function returns -1 */
int nlame_var_set_int(nlame_instance_t* inst, nlame_var_int_type type, int value)
{
   int ret = -1;
   switch(type){
      // general settings
      nlame_var_set_switch_type(nle_var_bitrate,brate)
      nlame_var_set_switch_type(nle_var_quality,quality)
      nlame_var_set_switch_type(nle_var_out_samplerate,out_samplerate)
      nlame_var_set_switch_type_val(nle_var_channel_mode,mode,(MPEG_mode)value)
      nlame_var_set_switch_type(nle_var_num_channels,num_channels);
      nlame_var_set_switch_type(nle_var_in_samplerate,in_samplerate);
      nlame_var_set_switch_type(nle_var_free_format,free_format);
//      nlame_var_set_switch_type(nle_var_auto_ms,mode_automs);
      nlame_var_set_switch_type(nle_var_force_ms,force_ms);

      // vbr settings
      nlame_var_set_switch_type_val(nle_var_vbr_mode,VBR,(vbr_mode)value);
      nlame_var_set_switch_type(nle_var_vbr_quality,VBR_q);
      nlame_var_set_switch_type(nle_var_vbr_min_bitrate,VBR_min_bitrate_kbps);
      nlame_var_set_switch_type(nle_var_vbr_max_bitrate,VBR_max_bitrate_kbps);
      nlame_var_set_switch_type(nle_var_vbr_hard_min,VBR_hard_min);
      nlame_var_set_switch_type(nle_var_abr_mean_bitrate,VBR_mean_bitrate_kbps);
      nlame_var_set_switch_type(nle_var_vbr_generate_info_tag,bWriteVbrTag);

      // filter settings
      nlame_var_set_switch_type(nle_var_lowpass_freq,lowpassfreq);
      nlame_var_set_switch_type(nle_var_lowpass_width,lowpasswidth);
      nlame_var_set_switch_type(nle_var_highpass_freq,highpassfreq);
      nlame_var_set_switch_type(nle_var_highpass_width,highpasswidth);

      // mp3 frame/stream settings
      nlame_var_set_switch_type(nle_var_copyright,copyright);
      nlame_var_set_switch_type(nle_var_original,original);
      nlame_var_set_switch_type(nle_var_error_protection,error_protection);
      nlame_var_set_switch_type(nle_var_priv_extension,extension);
//      nlame_var_set_switch_type_val(nle_var_padding_type,padding_type,(enum Padding_type_e)value);
      nlame_var_set_switch_type(nle_var_strict_iso,strict_ISO);

      // ATH settings
      nlame_var_set_switch_type(nle_var_ath_disable,noATH);
      nlame_var_set_switch_type(nle_var_ath_only,ATHonly);
      nlame_var_set_switch_type(nle_var_ath_type,ATHtype);
      nlame_var_set_switch_type(nle_var_ath_short,ATHshort);
      nlame_var_set_switch_type(nle_var_athaa_type,athaa_type);
      nlame_var_set_switch_type(nle_var_athaa_loudapprox,athaa_loudapprox);
//      nlame_var_set_switch_type(nle_var_athaa_cwlimit,cwlimit);

      // misc settings
      nlame_var_set_switch_type(nle_var_no_short_blocks,no_short_blocks);
      nlame_var_set_switch_type(nle_var_allow_diff_short,allow_diff_short);
      nlame_var_set_switch_type(nle_var_use_temporal,useTemporal);
      nlame_var_set_switch_type(nle_var_emphasis,emphasis);
      nlame_var_set_switch_type(nle_var_disable_reservoir,disable_reservoir);

      // write-only variables
      nlame_var_set_switch_type(nle_var_preset_vbr,alt_preset_vbr);
      nlame_var_set_switch_type(nle_var_preset_cbr,alt_preset_cbr);
      nlame_var_set_switch_type(nle_var_preset_abr,alt_preset_abr);

      // replay-gain settings
      nlame_var_set_switch_type(nle_var_find_replay_gain,findReplayGain);
      nlame_var_set_switch_type(nle_var_decode_on_the_fly,decode_on_the_fly);
   }

   return ret;
}

/*! returns the integer variable value for the type */
int nlame_var_get_int(nlame_instance_t* inst, nlame_var_int_type type)
{
   int val = -1;
   switch(type){
      // general settings
      nlame_var_get_switch_type(nle_var_bitrate,brate);
      nlame_var_get_switch_type(nle_var_quality,quality);
      nlame_var_get_switch_type(nle_var_out_samplerate,out_samplerate);
      nlame_var_get_switch_type(nle_var_channel_mode,mode);
      nlame_var_get_switch_type(nle_var_num_channels,num_channels);
      nlame_var_get_switch_type(nle_var_in_samplerate,in_samplerate);
      nlame_var_get_switch_type(nle_var_free_format,free_format);
//      nlame_var_get_switch_type(nle_var_auto_ms,mode_automs);
      nlame_var_get_switch_type(nle_var_force_ms,force_ms);

      // vbr settings
      nlame_var_get_switch_type(nle_var_vbr_mode,VBR);
      nlame_var_get_switch_type(nle_var_vbr_quality,VBR_q);
      nlame_var_get_switch_type(nle_var_vbr_min_bitrate,VBR_min_bitrate_kbps);
      nlame_var_get_switch_type(nle_var_vbr_max_bitrate,VBR_max_bitrate_kbps);
      nlame_var_get_switch_type(nle_var_vbr_hard_min,VBR_hard_min);
      nlame_var_get_switch_type(nle_var_abr_mean_bitrate,VBR_mean_bitrate_kbps);
      nlame_var_get_switch_type(nle_var_vbr_generate_info_tag,bWriteVbrTag);

      // filter settings
      nlame_var_get_switch_type(nle_var_lowpass_freq,lowpassfreq);
      nlame_var_get_switch_type(nle_var_lowpass_width,lowpasswidth);
      nlame_var_get_switch_type(nle_var_highpass_freq,highpassfreq);
      nlame_var_get_switch_type(nle_var_highpass_width,highpasswidth);

      // mp3 frame/stream settings
      nlame_var_get_switch_type(nle_var_copyright,copyright);
      nlame_var_get_switch_type(nle_var_original,original);
      nlame_var_get_switch_type(nle_var_error_protection,error_protection);
      nlame_var_get_switch_type(nle_var_priv_extension,extension);
//      nlame_var_get_switch_type(nle_var_padding_type,padding_type);
      nlame_var_get_switch_type(nle_var_strict_iso,strict_ISO);

      // ATH settings
      nlame_var_get_switch_type(nle_var_ath_disable,noATH);
      nlame_var_get_switch_type(nle_var_ath_only,ATHonly);
      nlame_var_get_switch_type(nle_var_ath_type,ATHtype);
      nlame_var_get_switch_type(nle_var_ath_short,ATHshort);
      nlame_var_get_switch_type(nle_var_athaa_type,athaa_type);
      nlame_var_get_switch_type(nle_var_athaa_loudapprox,athaa_loudapprox);
//      nlame_var_get_switch_type(nle_var_athaa_cwlimit,cwlimit);

      // misc settings
      nlame_var_get_switch_type(nle_var_no_short_blocks,no_short_blocks);
      nlame_var_get_switch_type(nle_var_allow_diff_short,allow_diff_short);
      nlame_var_get_switch_type(nle_var_use_temporal,useTemporal);
      nlame_var_get_switch_type(nle_var_emphasis,emphasis);
      nlame_var_get_switch_type(nle_var_disable_reservoir,disable_reservoir);

      // read-only variables
      nlame_var_get_switch_type(nle_var_mpeg_version,version);
      nlame_var_get_switch_type(nle_var_encoder_delay,encoder_delay);
      nlame_var_get_switch_type(nle_var_samples_buffered,mf_samples_to_encode);
      nlame_var_get_switch_type(nle_var_frames_encoded,frameNum);
      nlame_var_get_switch_type(nle_var_size_mp3buffer,size_mp3buffer);

      case nle_var_quality_value_high: val = nle_quality_high; break;
      case nle_var_quality_value_fast: val = nle_quality_fast; break;

      // replay-gain settings
      nlame_var_get_switch_type(nle_var_find_replay_gain,findReplayGain);
      nlame_var_get_switch_type(nle_var_decode_on_the_fly,decode_on_the_fly);

      nlame_var_get_switch_type(nle_var_framesize,framesize);
   }
   return val;
}

/*! sets the float variable value for the type
    if an error occured, this function returns -1 */
int nlame_var_set_float(nlame_instance_t* inst, nlame_var_float_type type, float value)
{
   int ret = -1;
   switch(type){
      nlame_var_set_switch_type(nle_var_float_scale,scale);
      nlame_var_set_switch_type(nle_var_float_compression_ratio,compression_ratio);
      nlame_var_set_switch_type(nle_var_float_ath_lower,ATHlower);
      nlame_var_set_switch_type(nle_var_float_athaa_sensitivity,athaa_sensitivity);
      nlame_var_set_switch_type(nle_var_float_interch,interChRatio);
      nlame_var_set_switch_type(nle_var_float_vbr_quality,VBR_quality);
   }

   return ret;
}

/*! returns the float variable value for the type */
float nlame_var_get_float(nlame_instance_t* inst, nlame_var_float_type type)
{
   float val = 0.f;
   switch(type){
      nlame_var_get_switch_type(nle_var_float_scale,scale);
      nlame_var_get_switch_type(nle_var_float_compression_ratio,compression_ratio);
      nlame_var_get_switch_type(nle_var_float_ath_lower,ATHlower);
      nlame_var_get_switch_type(nle_var_float_athaa_sensitivity,athaa_sensitivity);
      nlame_var_get_switch_type(nle_var_float_interch,interChRatio);
      nlame_var_get_switch_type(nle_var_float_vbr_quality,VBR_quality);
   }
   return val;
}

int nlame_init_params(nlame_instance_t* inst)
{
   return lame_init_params(inst->lgf);
}

/*! mute callback function; does nothing */
void nlame_mute_callback_func(const char* msg, va_list list)
{
}

void nlame_callback_set(nlame_instance_t* inst, nlame_callback_type type, nlame_callback_func func)
{
   if (func==NULL)
      func = nlame_mute_callback_func;

   switch(type)
   {
   case nle_callback_error:
      lame_set_errorf(inst->lgf,func);
      break;
   case nle_callback_debug:
      lame_set_debugf(inst->lgf,func);
      break;
   case nle_callback_message:
      lame_set_msgf(inst->lgf,func);
      break;
   }
}

int nlame_encode_buffer( nlame_instance_t* inst,
   nlame_encode_buffer_type buftype,
   const void* buffer_l, const void* buffer_r, const int nsamples,
   unsigned char* mp3buf, const unsigned int mp3buf_size)
{
   int ret = -1;

   // call function, according to type
   switch(buftype)
   {
   case nle_buffer_short:
      ret = lame_encode_buffer(inst->lgf,
         (short*)buffer_l,(short*)buffer_r,nsamples,
         mp3buf,mp3buf_size);
      break;

   case nle_buffer_int:
      ret = lame_encode_buffer_int(inst->lgf,
         (int*)buffer_l,(int*)buffer_r,nsamples,
         mp3buf,mp3buf_size);
      break;

   case nle_buffer_float:
      ret = lame_encode_buffer_float(inst->lgf,
         (float*)buffer_l,(float*)buffer_r,nsamples,
         mp3buf,mp3buf_size);
      break;

   case nle_buffer_long:
      ret = lame_encode_buffer_long2(inst->lgf,
         (long*)buffer_l,(long*)buffer_r,nsamples,
         mp3buf,mp3buf_size);
      break;
   }

   return ret;
}

int nlame_encode_buffer_mono( nlame_instance_t* inst,
   nlame_encode_buffer_type buftype,
   const void* buffer_m, const int nsamples,
   unsigned char* mp3buf, const unsigned int mp3buf_size)
{
   return nlame_encode_buffer(inst,buftype,buffer_m,buffer_m,nsamples,
      mp3buf,mp3buf_size);
}

int nlame_encode_buffer_interleaved( nlame_instance_t* inst,
   nlame_encode_buffer_type buftype,
   const void* buffer, const int nsamples,
   unsigned char* mp3buf, const unsigned int mp3buf_size)
{
   int ret = -1;

   // call function, according to type
   switch(buftype)
   {
   case nle_buffer_short:
      ret = lame_encode_buffer_interleaved(inst->lgf,
         (short*)buffer,nsamples,mp3buf,mp3buf_size);
      break;

   case nle_buffer_int:
      ret = lame_encode_buffer_interleaved_int(inst->lgf,
         (int*)buffer,nsamples,mp3buf,mp3buf_size);
      break;
   }
   return ret;
}

int nlame_encode_flush( nlame_instance_t* inst,
   unsigned char* mp3buf, const unsigned int mp3buf_size)
{
   return lame_encode_flush(inst->lgf, mp3buf, mp3buf_size);
}

int nlame_encode_flush_nogap( nlame_instance_t* inst,
   unsigned char* mp3buf, const unsigned int mp3buf_size )
{
   return lame_encode_flush_nogap(inst->lgf, mp3buf, mp3buf_size);
}

int nlame_reinit_bitstream( nlame_instance_t* inst )
{
   return lame_init_bitstream(inst->lgf);
}

void nlame_write_vbr_infotag( nlame_instance_t* inst, FILE* fd )
{
   lame_mp3_tags_fid(inst->lgf,fd);
}

#pragma warning( push )
#pragma warning( disable: 4047 4024 )

void nlame_histogram_get( nlame_instance_t* inst,
   nlame_histogram_type type, int* histogram )
{
   switch(type)
   {
   case nle_hist_bitrate:
      lame_bitrate_hist(inst->lgf, histogram);
      break;
   case nle_hist_kbps:
      lame_bitrate_kbps(inst->lgf, histogram);
      break;
   case nle_hist_stereo_mode:
      lame_stereo_mode_hist(inst->lgf, histogram);
      break;
   case nle_hist_bitrate_stereo_mode:
      lame_bitrate_stereo_mode_hist(inst->lgf, histogram);
      break;
   }
}

#pragma warning( pop )

/*! nice trick: since this function is in nLAME.dll, the function always tells
    the user the version of the API of the nLAME.dll, even if the user of the dll
    has another nlame.h */
int nlame_get_api_version()
{
   return NLAME_CURRENT_API_VERSION;
}

void nlame_id3tag_init( nlame_instance_t* inst,
   int bWriteId3v1Tag, int bWriteId3v2Tag, unsigned int uiV2ExtraPadSize)
{
   if (!bWriteId3v1Tag && !bWriteId3v2Tag)
      return;

   id3tag_init(inst->lgf);

   if (bWriteId3v1Tag && !bWriteId3v2Tag)
   {
      // only v1 tag
      id3tag_v1_only(inst->lgf);
   }
   else
   if (!bWriteId3v1Tag && bWriteId3v2Tag)
   {
      // only v2 tag
      id3tag_add_v2(inst->lgf);
      id3tag_v2_only(inst->lgf);
   }
   else
   {
      // both
      id3tag_add_v2(inst->lgf);
   }

   if (bWriteId3v2Tag && uiV2ExtraPadSize > 0)
      id3tag_set_pad(inst->lgf, uiV2ExtraPadSize);
}

void nlame_id3tag_setfield_latin1( nlame_instance_t* inst,
   enum nlame_id3tag_field field, const char* text)
{
   switch (field)
   {
   case nif_title:   id3tag_set_title(inst->lgf, text); break;
   case nif_artist:  id3tag_set_artist(inst->lgf, text); break;
   case nif_album:   id3tag_set_album(inst->lgf, text); break;
   case nif_year:    id3tag_set_year(inst->lgf, text); break;
   case nif_comment: id3tag_set_comment(inst->lgf, text); break;
   case nif_track:   id3tag_set_track(inst->lgf, text); break;
   case nif_genre:   id3tag_set_genre(inst->lgf, text); break;
   }
}
