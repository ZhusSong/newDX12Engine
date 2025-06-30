// 25.6.8 李
#include "../../../public/simple_core_minimal/simple_c_path/simple_path.h"
#include "../../../public/simple_core_minimal/simple_c_core/simple_c_array/simple_c_array_string.h"

void get_path_directory_inline(char *path_buf)
{
	normalization_path(path_buf);// \\ /

	char filename[1024] = { 0 };
	get_path_clean_filename(filename, path_buf);

	remove_string_start(path_buf,filename);
}

void get_path_directory(char *buf, const char *path_buf)
{
	strcpy(buf,path_buf);
	get_path_directory_inline(buf);
}

void get_path_clean_filename(char *buf, const char *path_buf)
{
	simple_c_string c_string;
	if (strstr(path_buf,"\\"))
	{
		dismantling_string(path_buf, "\\", &c_string);
	}
	else if (strstr(path_buf, "/"))
	{
		dismantling_string(path_buf, "/", &c_string);
	}

	char* value = get_string(c_string.size - 1, &c_string);
	strcpy(buf, value);
	destroy_string(&c_string);
}

void normalization_path(char *path_buf)
{
	replace_char_inline(path_buf,'\\','/');
}

void normalization_directory(char *buf, const char *path_buf)
{
	simple_c_string c_string;
	dismantling_string(path_buf, "/", &c_string);
	
	for (int i = 0 ; i < c_string.size - 1;i++)
	{
		char* value = get_string(i, &c_string);
		char buf_tmp[1024] = { 0 };
		strcat(buf_tmp, value);
		strcat(buf_tmp,"\\");
		strcat(buf, buf_tmp);
	}

	destroy_string(&c_string);
}

void get_path_clean_filename_w(wchar_t* buf, const wchar_t* path_buf)
{
	simple_c_wstring c_string;
	if (wcsstr(path_buf, L"\\"))
	{
		dismantling_wstring(path_buf, L"\\", &c_string);
	}
	else if (wcsstr(path_buf, L"/"))
	{
		dismantling_wstring(path_buf, L"/", &c_string);
	}

	wchar_t* value = get_wstring(c_string.size - 1, &c_string);
	wcscpy(buf, value);
	destroy_wstring(&c_string);
}

// 通过shader文件名来获取shader文件
wchar_t* get_shader_path_shader_name(const  wchar_t* shaderFileName)
{
	wchar_t absFilePath[128] = { 0 };
	wchar_t currentDir[128] = { 0 };

	// 获取当前源文件的绝对路径（__FILEW__ 需要支持，若无可自行改成转换）
	if (!GetFullPathNameW(__FILEW__, 128, absFilePath, NULL)) {
		return NULL;
	}

	// 去掉文件名，保留目录
	wcscpy(currentDir, absFilePath);
	for (int i = (int)wcslen(currentDir) - 1; i >= 0; i--) {
		if (currentDir[i] == L'\\' || currentDir[i] == L'/') {
			currentDir[i] = L'\0';
			break;
		}
	}

	for (int depth = 0; depth < 128; depth++) {
		// 拼接路径 currentDir + "\Shader\" + shaderFileName
		wchar_t shaderDir[128] = { 0 };
		wcscpy(shaderDir, currentDir);
		size_t len = wcslen(shaderDir);
		if (len > 0 && shaderDir[len - 1] != L'\\' && shaderDir[len - 1] != L'/') {
			wcscat(shaderDir, L"\\");
		}
		// 写死Shader文件夹命名，后续有需要可以改变
		wcscat(shaderDir, L"Shader\\");
		wcscat(shaderDir, shaderFileName);

		// 检查文件是否存在
		DWORD attr = GetFileAttributesW(shaderDir);
		if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
			// 找到，复制路径返回
			size_t pathLen = wcslen(shaderDir);
			wchar_t* result = (wchar_t*)malloc((pathLen + 1) * sizeof(wchar_t));
			if (!result) return NULL;
			wcscpy(result, shaderDir);
			return result;
		}

		// 向上一级目录，去掉最后一个目录名
		bool movedUp = false;
		size_t dirLen = wcslen(currentDir);
		for (int i = (int)dirLen - 1; i >= 0; i--) {
			if (currentDir[i] == L'\\' || currentDir[i] == L'/') {
				currentDir[i] = L'\0';
				movedUp = true;
				break;
			}
		}
		if (!movedUp) {
			// 到顶层，没找到
			break;
		}
	}

	// 找不到
	return NULL;
}


char* get_full_path(char* in_path_buf, int in_buff_len, const char* in_path)
{
#ifdef _WIN32
	_fullpath(in_path_buf, in_path, in_buff_len);
#else
	realpath(in_path, in_path_buf);
#endif
	normalization_path(in_path_buf);

	return in_path_buf;
}
