//
//  podofo_config.h
//  podofo
//
//  Created by Matias Piipari on 16/04/2012.
//  Copyright (c) 2012 Mekentosj BV. All rights reserved.
//

#ifndef podofo_podofo_config_h
#define podofo_podofo_config_h

/* Template filled out by CMake */

/*
 * *** THIS HEADER IS INCLUDED BY PdfCompilerCompat.h ***
 * *** DO NOT INCLUDE DIRECTLY ***
 */
#ifndef _PDF_COMPILERCOMPAT_H
#error Please include PdfDefines.h instead
#endif

#define PODOFO_VERSION_MAJOR 0
#define PODOFO_VERSION_MINOR 8
#define PODOFO_VERSION_PATCH 3

/* PoDoFo configuration options */
#define PODOFO_MULTI_THREAD

/* somewhat platform-specific headers */
#undef PODOFO_HAVE_STRINGS_H
#undef PODOFO_HAVE_ARPA_INET_H
#define PODOFO_HAVE_WINSOCK2_H 1
/* #undef PODOFO_HAVE_MEM_H */
/* #undef PODOFO_HAVE_CTYPE_H */

/* Integer types - headers */
#define PODOFO_HAVE_STDINT_H 1
/* #undef PODOFO_HAVE_BASETSD_H */
#define PODOFO_HAVE_SYS_TYPES_H 1
/* Integer types - type names */
#define PDF_INT8_TYPENAME   int8_t
#define PDF_INT16_TYPENAME  int16_t
#define PDF_INT32_TYPENAME  int32_t
#define PDF_INT64_TYPENAME  int64_t
#define PDF_UINT8_TYPENAME  uint8_t
#define PDF_UINT16_TYPENAME uint16_t
#define PDF_UINT32_TYPENAME uint32_t
#define PDF_UINT64_TYPENAME uint64_t

/* Endianness */
/* #undef TEST_BIG */

/* Libraries */
#define PODOFO_HAVE_JPEG_LIB 1
#define PODOFO_HAVE_PNG_LIB 1
#define PODOFO_HAVE_TIFF_LIB 1
#undef PODOFO_HAVE_FONTCONFIG
#undef PODOFO_HAVE_LUA 
#undef PODOFO_HAVE_BOOST 
#undef PODOFO_HAVE_CPPUNIT

/* Platform quirks */
#define PODOFO_JPEG_RUNTIME_COMPATIBLE


#endif
