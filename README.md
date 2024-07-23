# ambient_cap_net_raw

Some tools require raw device/packet access. This is intended as a trivial wrapper for that.

This program must have the following capabilities to run properly: `CAP_NET_RAW`

A command to equip the binary with the right caps is:
```shell
 setcap cap_net_raw+p ambient_cap_net_raw
```

To get a shell with additional caps that can be inherited by other processes:
```shell
./ambient_cap_net_raw /bin/bash
```

Verifying that it works:

```shell
./ambient_cap_net_raw grep Cap /proc/self/status
```
