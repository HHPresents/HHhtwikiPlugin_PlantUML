// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���̃C���N���[�h �t�@�C���A�܂���
// �Q�Ɖ񐔂������A�����܂�ύX����Ȃ��A�v���W�F�N�g��p�̃C���N���[�h �t�@�C��
// ���L�q���܂��B
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
// Windows �w�b�_�[ �t�@�C��:
#include <windows.h>



// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă�������
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
