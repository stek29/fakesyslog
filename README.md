# fakesyslog
Redirect syslog to stdout/stderr -- useful for minimal alpine containers

# Build
```sh
make CC=x86_64-linux-musl-gcc clean all
```

# Usage
```
export LD_PRELOAD=/fakesyslog.so
```
