# cpp-simd-programming

## 几种C++并发计算方案的实践

### 将一个单声道、小端 32 位符点采样、44100 Hz 采样率的 PCM 格式的声音音量加倍

+ C++17 STL [Extensions for Parallelism](https://en.cppreference.com/w/cpp/experimental/parallelism)

|               系统                           | seq | par | par_unseq |
| :-----------------------------------------: | :---: | :---: | :---: |
| Windows 10, 16GB RAM, AMD Ryzen 7 1700 8 核 | 139ms | 35ms | 35ms |
| Windows 10, 16GB RAM, Intel i7-7700 4 核 | 106ms | 33ms | 33ms |

+ [AVX](https://software.intel.com/content/www/us/en/develop/articles/introduction-to-intel-advanced-vector-extensions.html)
+ [Neon for Android](https://developer.arm.com/architectures/instruction-sets/simd-isas/neon)