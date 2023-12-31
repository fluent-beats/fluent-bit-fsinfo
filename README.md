# Description

[Fluent Bit](https://fluentbit.io) input plugin that collects file system stats from Linux hosts.

# Requirements

- Docker
- Docker image `fluent-beats/fluent-bit-plugin-dev`

# Build
```bash
./build.sh
```

# Test
```bash
./test.sh
 ```

# Design

This plugin was desined to collect filesystem stats, equivalent to Linux command:
```bash
stat -f /
```

## Configurations

This input plugin can be configured using the following parameters:

 Key                    | Description                                   | Default
------------------------|-----------------------------------------------|------------------
 interval_sec           | Interval in seconds to collect data           | 10
 mount_point            | Mount point used to extract stats             | /