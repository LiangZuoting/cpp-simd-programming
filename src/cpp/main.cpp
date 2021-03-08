/**
 * Make sure compiler's std option equal to or greater than C++17.
 * 
 * change f32le-format pcm file's volume in different ways.
 */
#include <iostream>
#include <string>
#include <functional>
#include <chrono>
#include <algorithm>
#if __has_include(<execution>)
#include <execution>
#endif
#include <unordered_map>
#ifndef ARM
#include <immintrin.h>
#endif
#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#endif

using namespace std;

float factor = 2;

// print duration in millisecond between constructor and destructor.
class Timer
{
public:
	Timer(const char *prefix)
		: _prefix(prefix)
	{
		_start = chrono::system_clock::now();
	}

	~Timer()
	{
		auto ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - _start).count();
#ifdef ANDROID
		__android_log_print(ANDROID_LOG_INFO, "change_volume", "%s %d", _prefix.data(), ms);
#else
		cout << _prefix << ' ' << ms << " ms." << endl;
#endif
	}

private:
	string _prefix;
	chrono::system_clock::time_point _start;
};

struct FileHandler
{
	float *in_file_data = nullptr; // original file data
	float *out_file_data = nullptr; // processed file data
	size_t file_size = 0;
};

FileHandler read_file(const char *file_path)
{
	FileHandler result = { 0 };
	FILE *f = fopen(file_path, "rb");
	fseek(f, 0, SEEK_END);
	result.file_size = ftell(f);
	fseek(f, 0, SEEK_SET);
	result.in_file_data = (float*)new char[result.file_size];
	result.out_file_data = (float*)new uint16_t[result.file_size];
	fread(result.in_file_data, result.file_size, 1, f);
	fclose(f);
	return result;
}

void write_file(const char *file_path, const FileHandler &file)
{
	FILE *f = fopen(file_path, "wb");
	fwrite(file.out_file_data, file.file_size, 1, f);
	fclose(f);
}

const float ir[2048] = { 0 };

// sequenced stl
void change_volume_0(FileHandler &file)
{
	int samples = file.file_size / 4;
	for (int i = 0; i < samples; ++i)
	{
		for (int j = 0; j < 2048 && i >= j; ++j) 
		{
			file.out_file_data[i] += file.in_file_data[i - j] * ir[j];
		}
	}
}

#if __cplusplus > 201700L && __has_include(<execution>) && !defined(ANDROID)

// parallel stl
void change_volume_1(FileHandler &file)
{
	//transform(execution::par, file.in_file_data, file.in_file_data + file.file_size / sizeof(float), file.out_file_data, change_sample);
}

// parallel and unsequenced stl
void change_volume_2(FileHandler &file)
{
	//transform(execution::par_unseq, file.in_file_data, file.in_file_data + file.file_size / sizeof(float), file.out_file_data, change_sample);
}

#endif

#ifdef ARM

// NEON
void change_volume_3(FileHandler &file)
{

}

#endif // ARM

#ifdef AVX2

void change_volume_4(FileHandler &file)
{
	int samples = file.file_size / 4;
	for (int i = 0; i < samples; ++i)
	{
		__m256 out = { 0 };
		for (int j = 0; j < 2048 && i >= j; j += 8) 
		{
			__m256 mm256_ir = _mm256_load_ps(ir + j);
			__m256 in;
			switch (i - j)
			{
			case 0:
				in = _mm256_setr_ps(file.in_file_data[0], 0, 0, 0, 0, 0, 0, 0);
				break;
			case 1:
				in = _mm256_setr_ps(file.in_file_data[1], file.in_file_data[0], 0, 0, 0, 0, 0, 0);
				break;
			case 2:
				in = _mm256_setr_ps(file.in_file_data[2], file.in_file_data[1], file.in_file_data[0], 0, 0, 0, 0, 0);
				break;
			case 3:
				in = _mm256_setr_ps(file.in_file_data[3], file.in_file_data[2], file.in_file_data[1], file.in_file_data[0], 0, 0, 0, 0);
				break;
			case 4:
				in = _mm256_setr_ps(file.in_file_data[4], file.in_file_data[3], file.in_file_data[2], file.in_file_data[1], file.in_file_data[0], 0, 0, 0);
				break;
			case 5:
				in = _mm256_setr_ps(file.in_file_data[5], file.in_file_data[4], file.in_file_data[3], file.in_file_data[2], file.in_file_data[1], file.in_file_data[0], 0, 0);
				break;
			case 6:
				in = _mm256_setr_ps(file.in_file_data[6], file.in_file_data[5], file.in_file_data[4], file.in_file_data[3], file.in_file_data[2], file.in_file_data[1], file.in_file_data[0], 0);
				break;
			default:
				in = _mm256_setr_ps(file.in_file_data[i - j], file.in_file_data[i - j - 1], file.in_file_data[i - j - 2], file.in_file_data[i - j - 3], file.in_file_data[i - j - 4], file.in_file_data[i - j - 5], file.in_file_data[i - j - 6], file.in_file_data[i - j - 7]);
				break;
			}
			out = _mm256_fmadd_ps(in, mm256_ir, out);
		}
		file.out_file_data[i] = out.m256_f32[0] + out.m256_f32[1] + out.m256_f32[2] + out.m256_f32[3] + out.m256_f32[4] + out.m256_f32[5] + out.m256_f32[6] + out.m256_f32[7];
	}
}

#endif

#ifdef AVX512

void change_volume_5(FileHandler &file)
{

}

#endif

unordered_map<int, function<void(FileHandler &)>> funs =
{
	{0, change_volume_0},
#if __cplusplus > 201700L && __has_include(<execution>) && !defined(ANDROID)
		{1, change_volume_1},
		{2, change_volume_2},
#endif
#ifdef ARM
		{3, change_volume_3},
#endif
#ifdef AVX2
		{4, change_volume_4},
#endif
#ifdef AVX512
		{5, change_volume_5},
#endif
};

void change_volume(const char *in_file_path, const char *out_file_path, int method_index)
{
	auto fun = funs[method_index];
	auto file = read_file(in_file_path);
	{
		Timer _("transform:");
		fun(file);
	}
	write_file(out_file_path, file);
}

#ifndef ARM

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		cout << "Usage: change_volume in_file_path out_file_path method_index" << endl;
		return -1;
	}

	change_volume(argv[1], argv[2], stoi(argv[3]));
}

#endif

#ifdef ANDROID

extern "C" JNIEXPORT jstring JNICALL
Java_com_huya_simd_MainActivity_stringFromJNI(
		JNIEnv* env,
		jobject /* this */) {
	std::string hello = std::to_string(__cplusplus);
	change_volume("/mnt/sdcard/f32le.pcm", "/mnt/sdcard/f32le-0.pcm", 0);
	return env->NewStringUTF(hello.c_str());
}

#endif