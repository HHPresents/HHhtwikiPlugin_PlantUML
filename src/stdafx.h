// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>



// TODO: プログラムに必要な追加ヘッダーをここで参照してください
#include <comdef.h>

//#include <objbase.h>  
//#include <msxml6.h>  
//#pragma comment (lib, "msxml6.lib")
#import "libid:F5078F18-C551-11D3-89B9-0000F81FE221" version("6.0")

#include <Winver.h>
#pragma comment (lib, "Version.lib")

#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")

#include <winhttp.h>
#pragma comment (lib, "winhttp.lib")

#include <Wincrypt.h>
#pragma comment (lib, "Crypt32.lib")
