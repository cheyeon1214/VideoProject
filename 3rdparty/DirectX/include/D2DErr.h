/*=========================================================================*\

    Copyright (c) Microsoft Corporation.  All rights reserved.

\*=========================================================================*/

#pragma once

/*=========================================================================*\
    D2D Status Codes
\*=========================================================================*/

#define FACILITY_D2D 0x899

#define MAKE_D2DHR( sev, code )\
    MAKE_HRESULT( sev, FACILITY_D2D, (code) )

#define MAKE_D2DHR_ERR( code )\
    MAKE_D2DHR( 1, code )


//+----------------------------------------------------------------------------
//
// D2D error codes
//
//------------------------------------------------------------------------------

//
//  Error codes shared with WINCODECS
//

//
// The pixel format is not supported.
//
#define D2DERR_UNSUPPORTED_PIXEL_FORMAT     WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT

//
// Error codes that were already returned in prior versions and were part of the
// MIL facility.

//
// Error codes mapped from WIN32 where there isn't already another HRESULT based
// define
//

//
// The supplied buffer was too small to accomodate the data.
//
#define D2DERR_INSUFFICIENT_BUFFER          HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)


//
// D2D specific codes
//

//
// The object was not in the correct state to process the method.
//
#ifndef D2DERR_WRONG_STATE
#define D2DERR_WRONG_STATE                  MAKE_D2DHR_ERR(0x001)
#endif

//
// The object has not yet been initialized.
//
#ifndef D2DERR_NOT_INITIALIZED
#define D2DERR_NOT_INITIALIZED              MAKE_D2DHR_ERR(0x002)
#endif

//
// The requested opertion is not supported.
//
#ifndef D2DERR_UNSUPPORTED_OPERATION
#define D2DERR_UNSUPPORTED_OPERATION        MAKE_D2DHR_ERR(0x003)
#endif

//
// The geomery scanner failed to process the data.
//
#ifndef D2DERR_SCANNER_FAILED
#define D2DERR_SCANNER_FAILED               MAKE_D2DHR_ERR(0x004)
#endif

//
// D2D could not access the screen.
//
#ifndef D2DERR_SCREEN_ACCESS_DENIED
#define D2DERR_SCREEN_ACCESS_DENIED         MAKE_D2DHR_ERR(0x005)
#endif

//
// A valid display state could not be determined.
//
#ifndef D2DERR_DISPLAY_STATE_INVALID
#define D2DERR_DISPLAY_STATE_INVALID        MAKE_D2DHR_ERR(0x006)
#endif

//
// The supplied vector is vero.
//
#ifndef D2DERR_ZERO_VECTOR
#define D2DERR_ZERO_VECTOR                  MAKE_D2DHR_ERR(0x007)
#endif

//
// An internal error (D2D bug) occurred. On checked builds, we would assert.
//
// The application should close this instance of D2D and should consider
// restarting its process.
//
#ifndef D2DERR_INTERNAL_ERROR
#define D2DERR_INTERNAL_ERROR               MAKE_D2DHR_ERR(0x008)
#endif

//
// The display format we need to render is not supported by the
// hardware device.
//
#ifndef D2DERR_DISPLAY_FORMAT_NOT_SUPPORTED
#define D2DERR_DISPLAY_FORMAT_NOT_SUPPORTED MAKE_D2DHR_ERR(0x009)
#endif

//
// A call to this method is invalid.
//
#ifndef D2DERR_INVALID_CALL
#define D2DERR_INVALID_CALL                 MAKE_D2DHR_ERR(0x00A)
#endif

//
// No HW rendering device is available for this operation.
//
#ifndef D2DERR_NO_HARDWARE_DEVICE
#define D2DERR_NO_HARDWARE_DEVICE           MAKE_D2DHR_ERR(0x00B)
#endif

//
// There has been a presentation error that may be recoverable. The caller
// needs to recreate, rerender the entire frame, and reattempt present.
//
#ifndef D2DERR_RECREATE_TARGET
#define D2DERR_RECREATE_TARGET              MAKE_D2DHR_ERR(0x00C)
#endif

//
// Shader construction failed because it was too complex.
//
#ifndef D2DERR_TOO_MANY_SHADER_ELEMENTS
#define D2DERR_TOO_MANY_SHADER_ELEMENTS     MAKE_D2DHR_ERR(0x00D)
#endif

//
// Shader compilation failed.
//
#ifndef D2DERR_SHADER_COMPILE_FAILED
#define D2DERR_SHADER_COMPILE_FAILED        MAKE_D2DHR_ERR(0x00E)
#endif

//
// Requested DX surface size exceeded maximum texture size.
//
#ifndef D2DERR_MAX_TEXTURE_SIZE_EXCEEDED
#define D2DERR_MAX_TEXTURE_SIZE_EXCEEDED    MAKE_D2DHR_ERR(0x00F)
#endif

//
// The requested D2D version is not supported.
//
#ifndef D2DERR_UNSUPPORTED_VERSION
#define D2DERR_UNSUPPORTED_VERSION          MAKE_D2DHR_ERR(0x010)
#endif

//
// Invalid number.
//
#ifndef D2DERR_BAD_NUMBER
#define D2DERR_BAD_NUMBER                   MAKE_D2DHR_ERR(0x0011)
#endif

//
// Objects used together must be created from the same factory instance.
//
#ifndef D2DERR_WRONG_FACTORY
#define D2DERR_WRONG_FACTORY                MAKE_D2DHR_ERR(0x012)
#endif

//
// A layer resource can only be in use once at any point in time.
//
#ifndef D2DERR_LAYER_ALREADY_IN_USE
#define D2DERR_LAYER_ALREADY_IN_USE         MAKE_D2DHR_ERR(0x013)
#endif

//
// The pop call did not match the corresponding push call
//
#ifndef D2DERR_POP_CALL_DID_NOT_MATCH_PUSH
#define D2DERR_POP_CALL_DID_NOT_MATCH_PUSH  MAKE_D2DHR_ERR(0x014)
#endif

//
// The resource was realized on the wrong render target
//
#ifndef D2DERR_WRONG_RESOURCE_DOMAIN
#define D2DERR_WRONG_RESOURCE_DOMAIN        MAKE_D2DHR_ERR(0x015)
#endif

//
// The push and pop calls were unbalanced
//
#ifndef D2DERR_PUSH_POP_UNBALANCED
#define D2DERR_PUSH_POP_UNBALANCED          MAKE_D2DHR_ERR(0x016)
#endif

//
// Attempt to copy from a render target while a layer or clip rect is applied
//
#ifndef D2DERR_RENDER_TARGET_HAS_LAYER_OR_CLIPRECT
#define D2DERR_RENDER_TARGET_HAS_LAYER_OR_CLIPRECT MAKE_D2DHR_ERR(0x017)
#endif

//
// The brush types are incompatible for the call.
//
#ifndef D2DERR_INCOMPATIBLE_BRUSH_TYPES
#define D2DERR_INCOMPATIBLE_BRUSH_TYPES     MAKE_D2DHR_ERR(0x018)
#endif

//
// An unknown win32 failure occurred.
//
#ifndef D2DERR_WIN32_ERROR
#define D2DERR_WIN32_ERROR                  MAKE_D2DHR_ERR(0x019)
#endif

//
// The render target is not compatible with GDI
//
#ifndef D2DERR_TARGET_NOT_GDI_COMPATIBLE
#define D2DERR_TARGET_NOT_GDI_COMPATIBLE    MAKE_D2DHR_ERR(0x01A)
#endif

//
// A text client drawing effect object is of the wrong type
// 
#ifndef D2DERR_TEXT_EFFECT_IS_WRONG_TYPE
#define D2DERR_TEXT_EFFECT_IS_WRONG_TYPE    MAKE_D2DHR_ERR(0x01B)
#endif

//
// The application is holding a reference to the IDWriteTextRenderer interface 
// after the corresponding DrawText or DrawTextLayout call has returned. The 
// IDWriteTextRenderer instance will be zombied.
// 
#ifndef D2DERR_TEXT_RENDERER_NOT_RELEASED
#define D2DERR_TEXT_RENDERER_NOT_RELEASED   MAKE_D2DHR_ERR(0x01C)
#endif

//
// The requested size is larger than the guaranteed supported texture size.
// 
#ifndef D2DERR_EXCEEDS_MAX_BITMAP_SIZE
#define D2DERR_EXCEEDS_MAX_BITMAP_SIZE     MAKE_D2DHR_ERR(0x01D)
#endif
