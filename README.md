# libregex
Regular expression library for C

![](https://github.com/shosatojp/libregex/workflows/C%2FC%2B%2B%20CI/badge.svg)

## Available features
* `+` `*` `.` `^` `$` `(...)` `(?:...)` `(...|...)` `[...]` `[^...]` `?` `+?` `*?` `{m, n}` `{m}` `\t` `\r` `\n` `\f` `\d` `\D` `\s` `\S` `\w` `\W` `\escape` `0-9`

* capture
* find all by regex


## Build

```sh
./autogen.sh
./configure
make
```