## Support

- Android devices above android 10
- Only arch aarch64 (64 bit)
- Require root

## Build

```bash
$ cmake -Bbuild . -DCMAKE_ANDROID_NDK=<path to ndk>
$ cd build
$ make
```

## Output

- Binary `build/bin`
