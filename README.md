# cpp-simd-programming

## 几种C++并发计算方案的实践

2021.3.9 订正：

+ 重新设计测试算法，以 [卷积混响](http://songho.ca/dsp/convolution/convolution.html#cpp_conv1d) 代替调节音量。混响时间复杂度更高。
+ 去掉对 C++17 STL 并发扩展的测试，[毫无意义](https://ermao.live/2021/03/05/C++17%E5%B9%B6%E8%A1%8C%E7%AE%97%E6%B3%95%E6%8E%A2%E7%A9%B6/)。

~~### 将一个单声道、小端 32 位符点采样、44100 Hz 采样率的 PCM 格式的声音音量加倍~~

~~+ C++17 STL~~ [Extensions for Parallelism](https://en.cppreference.com/w/cpp/experimental/parallelism)

|               系统                           | seq | par | par_unseq |
| :-----------------------------------------: | :---: | :---: | :---: |
| Windows 10, 16GB RAM, AMD Ryzen 7 1700 8 核 | 139ms | 35ms | 35ms |
| Windows 10, 16GB RAM, Intel i7-7700 4 核 | 106ms | 33ms | 33ms |

+ [AVX](https://software.intel.com/content/www/us/en/develop/articles/introduction-to-intel-advanced-vector-extensions.html)
+ [Neon for Android](https://developer.arm.com/architectures/instruction-sets/simd-isas/neon)

**测试结果：**

混响一段 60s 的单声道 float 采样的 pcm 音频：

|               系统                           | normal       | avx2       | avx512      | neon      |
| :-----------------------------------------:  | :----------: | :--------: | :---------: | :-------: |
| Windows 10, 16GB RAM, AMD Ryzen 7 1700 8 核  | 17200 ms     | 2896 ms    | NONE        | NONE      |
| Windows 10, 16GB RAM, Intel i7-7700 4 核     | 12452 ms     | 1285 ms    | NONE        | NONE      |
| HuaWei Mate 20, 8GB RAM, Kirin 980 8 核      | 5262 ms      | NONE       | NONE        | 2644 ms   |