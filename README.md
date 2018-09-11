# fakesyslog
Redirect syslog to stderr -- useful for minimal alpine containers

# Build
```sh
make CC=x86_64-linux-musl-gcc clean all
```

# Usage
```
export LD_PRELOAD=/fakesyslog.so
```

# License
MIT, based on musl's syslog iirc
