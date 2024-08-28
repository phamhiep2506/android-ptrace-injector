## Build

```bash
$ cmake -Bbuild . -DCMAKE_ANDROID_NDK=<path to ndk> -DANDROID_ABI=<arm64-v8a or armeabi-v7a>
$ cd build
$ make
```

## Output

- Binary `build/bin`
