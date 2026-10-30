# OAI RISC-V 可迁移依赖

本目录已经包含K3当前验证环境使用的项目内依赖，不再要求构建脚本从系统路径选择GCC或数据库。

## 目录

- `toolchain/rv64-gcc-16/`：GCC/G++ 16、binutils、CMake、Ninja、pkg-config、标准头文件、C/C++运行库、UHD及OAI编译所需RISC-V sysroot。
- `asn1c/`：OAI固定版本ASN.1编译器。
- `simde/`：无V扩展标量构建使用的SIMDe头文件。
- `mysql/`：MySQL 8.4服务器、客户端、插件、共享文件、动态库、私有数据目录及SIM订阅数据。
- `cn5gcorenetwork/`：AMF、SMF、UPF及其私有运行库和启停脚本。

## 构建

```bash
cd /path/to/openairinterface5g
./scripts/build-oai-native.sh
```

构建脚本固定使用：

```text
ext/toolchain/rv64-gcc-16/bin/gcc
ext/toolchain/rv64-gcc-16/bin/g++
ext/toolchain/rv64-gcc-16/bin/cmake
ext/toolchain/rv64-gcc-16/bin/ninja
ext/toolchain/rv64-gcc-16/bin/pkg-config
```

## 运行核心网和射频

```bash
sudo ./ext/cn5gcorenetwork/scripts/run-a-no-v.sh start
```

核心网启动脚本会使用 `ext/mysql/bin/mysqld` 和 `ext/mysql/data`，不会启动系统MySQL/MariaDB服务。

## 目标平台要求

- 64位RISC-V Linux，glibc，`lp64d` ABI；
- CPU至少支持构建参数 `rv64gc_zba`，否则通过 `OAI_RISCV_ARCH`选择目标板实际支持的ISA；
- Linux内核具备SCTP、TUN/TAP、dummy接口、iptables和USB 3控制器支持；
- B210运行仍需要内核USB访问权限和兼容的UHD固件/FPGA镜像；
- 工程复制后应保持可执行位和符号链接，推荐使用 `tar` 或 `rsync -a`。

数据库数据目录来自当前已验证实例，包含敏感SIM鉴权信息。迁移包必须限制访问权限，不应公开分发。
