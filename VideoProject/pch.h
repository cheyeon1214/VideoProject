// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"


/*
* opencv
*/
#pragma warning(disable: 4819)

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../3rdparty/opencv-4.6.0/x64/lib/opencv_world460d.lib")
#else
#pragma comment(lib, "../3rdparty/opencv-4.6.0/x64/lib/opencv_world460.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../3rdparty/opencv-3.3.1/x86/libd/opencv_world331d.lib")
#else
#pragma comment(lib, "../3rdparty/opencv-3.3.1/x86/lib/opencv_world331.lib")
#endif
#endif

#include <opencv2/opencv.hpp>

/*
* ffmpeg
*/
#ifdef _WIN64
#pragma comment(lib, "../3rdparty/ffmpeg-3.1.5/x64/avcodec.lib")
#pragma comment(lib, "../3rdparty/ffmpeg-3.1.5/x64/avutil.lib")
#pragma comment(lib, "../3rdparty/ffmpeg-3.1.5/x64/swscale.lib")
#else
#pragma comment(lib, "../3rdparty/ffmpeg-3.1.5/x86/avcodec.lib")
#pragma comment(lib, "../3rdparty/ffmpeg-3.1.5/x86/avutil.lib")
#pragma comment(lib, "../3rdparty/ffmpeg-3.1.5/x86/swscale.lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif
#define __STDC_CONSTANT_MACROS
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#ifdef __cplusplus
}
#endif 


#include "resource.h"
#include "mmsystem.h"

#include "common.h"
#include "cmn_global.h"

#include "DxDefs.h"
#include "SockDefs.h"
#include "DialogDefs.h"

#include "../include/DefsTrace.h"

#endif //PCH_H
