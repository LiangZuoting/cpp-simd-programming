#include <jni.h>
#include <string>
#include "main.cpp"

extern "C" JNIEXPORT jstring JNICALL
Java_com_huya_simd_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = std::to_string(__cplusplus);
    change_volume("mnt/sdcard/f32le.pcm", "mnt/sdcard/f32le-0", 0);
    return env->NewStringUTF(hello.c_str());
}
