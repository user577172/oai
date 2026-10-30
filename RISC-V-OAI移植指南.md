# OAI no-V RISC-V + B210 / FPGA 移植指南

## 1. 文档适用范围

本文面向当前工程中的OAI no-V版本，说明如何从K3平台迁移到：

1. 另一台运行Linux的64位RISC-V平台，并继续使用USRP B210；
2. 带RISC-V处理器的FPGA SoC平台，Linux仍在处理器侧运行；
3. 将OAI部分PHY计算或射频接口真正下沉到FPGA逻辑侧。

当前工程根目录以下文的变量表示：

```bash
OAI_ROOT=/path/to/openairinterface5g
```

当前已打包的主要依赖：

```text
ext/toolchain/rv64-gcc-16   GCC/G++、binutils、CMake、Ninja、pkg-config和RISC-V sysroot
ext/asn1c                   OAI固定版本ASN.1编译器
ext/simde                   no-V标量构建使用的SIMDe头文件
ext/mysql                   项目内MySQL 8.4运行时、数据目录和SIM订阅数据
ext/cn5gcorenetwork         AMF、SMF、UPF、配置和私有动态库
```

> 使用B210进行的是实际射频OTA测试，不是纯软件仿真。OAI RFsim不需要B210，两者不能混为一谈。

---

## 2. 迁移前必须确认的硬件与系统条件

### 2.1 CPU与ABI

在目标平台执行：

```bash
uname -m
lscpu
getconf GNU_LIBC_VERSION
```

当前工具链与构建参数要求：

```text
体系结构：riscv64
ABI：lp64d
默认ISA：rv64gc_zba
```

目标CPU不支持`zba`时，需要在编译前设置实际ISA，例如：

```bash
export OAI_RISCV_ARCH=rv64gc
export OAI_RISCV_ABI=lp64d
```

不得给no-V版本传入包含以下扩展的ISA：

```text
v、zve*、zv*
```

### 2.2 CPU核心数量

当前gNB启动器按8个CPU核心配置：

| 功能 | CPU |
|---|---:|
| L1 RX | 0 |
| L1 TX | 1 |
| RU主线程 | 2 |
| PHY线程池 | 3、4、5 |
| UHD/libusb辅助线程 | 6 |
| B210 USB中断 | 7 |

相关文件：

```text
scripts/run-a-no-v-gnb.sh
```

目标板少于8核时，必须修改以下参数：

```bash
--property=CPUAffinity=0-6
--thread-pool 3,4,5
--L1s.[0].L1_rx_thread_core 0
--L1s.[0].L1_tx_thread_core 1
--RUs.[0].ru_thread_core 2
```

同时修改`spread_uhd_ru_workers()`和`tune_usb_irq()`使用的CPU编号。所有编号必须小于目标板的CPU总数，并避免PHY工作线程与USB中断使用同一个核心。

### 2.3 内核能力

目标Linux内核至少需要：

```text
SCTP
TUN/TAP
dummy网络接口
IPv4 forwarding
iptables或兼容的netfilter实现
USB 3/xHCI
高精度定时器
```

检查命令：

```bash
sudo modprobe sctp
sudo modprobe tun
sudo modprobe dummy
ip link add oai-test-dummy type dummy
sudo ip link delete oai-test-dummy
```

### 2.4 文件系统与权限

复制工程时必须保留：

- 符号链接；
- 文件可执行权限；
- 数据库文件权限；
- 大文件和稀疏文件属性。

推荐：

```bash
rsync -aHAX --numeric-ids openairinterface5g/ target:/path/openairinterface5g/
```

或使用保留权限的tar包。不要使用会丢失符号链接或执行位的普通FAT/exFAT中转目录。

---

## 3. 首次迁移后的目录检查

```bash
cd "$OAI_ROOT"

test -x ext/toolchain/rv64-gcc-16/bin/gcc
test -x ext/toolchain/rv64-gcc-16/bin/g++
test -x ext/toolchain/rv64-gcc-16/bin/cmake
test -x ext/toolchain/rv64-gcc-16/bin/ninja
test -x ext/asn1c/install-oai-pinned/bin/asn1c
test -f ext/simde/simde/x86/gfni.h
test -x ext/mysql/bin/mysqld
test -d ext/mysql/data/mysql
```

测试项目内编译器：

```bash
printf 'int main(void){return 0;}\n' | \
  ext/toolchain/rv64-gcc-16/bin/gcc \
  -march=rv64gc_zba -mabi=lp64d -x c - \
  -o /tmp/oai-toolchain-test

/tmp/oai-toolchain-test
rm -f /tmp/oai-toolchain-test
```

如果编译器自身因GLIBC版本不兼容而无法启动，说明目标系统与K3用户态不兼容。此时不能只复制二进制，需要在目标系统上重新构建工具链，或使用与K3一致的根文件系统/容器环境。

---

## 4. 清理源平台留下的绝对路径

CMake缓存记录了源平台的工程路径。工程位置变化后，首次构建不要继续使用旧缓存。

推荐保留旧产物作为备份：

```bash
cd "$OAI_ROOT"
mv cmake_targets/ran_build/rv64-no-v-release \
   cmake_targets/ran_build/rv64-no-v-release.from-source-board
```

然后重新构建。确认新构建成功后，再决定是否删除备份。

---

## 5. 修改核心网网络配置

### 5.1 获取目标板地址和外网接口

```bash
ip -br -4 addr
ip -4 route
```

默认路由对应的网卡就是通常需要填写的`WAN_IF`。

### 5.2 修改`runtime.env`

文件：

```text
ext/cn5gcorenetwork/config/runtime.env
```

示例：

```bash
HOST_IP=10.17.98.200
WAN_IF=eth0
UPF_N3_IP=192.168.71.134
GNB_N3_IP=192.168.71.140
UE_SUBNET=12.1.1.0/24
UE_GATEWAY=12.1.1.1
INSTALL_PREFIX=/实际路径/openairinterface5g/ext/cn5gcorenetwork
CONFIG_DIR=/实际路径/openairinterface5g/ext/cn5gcorenetwork/config
STATE_DIR=/实际路径/openairinterface5g/ext/cn5gcorenetwork
```

必须修改：

- `HOST_IP`：目标板可达的管理/N2地址；
- `WAN_IF`：目标板连接外网的接口名；
- `INSTALL_PREFIX`、`CONFIG_DIR`、`STATE_DIR`：工程路径变化时同步更新。

N2/N3地址可以继续使用当前dummy接口规划，但不得与目标板已有网段冲突。

### 5.3 修改射频环境

文件：

```text
ext/cn5gcorenetwork/config/rf.env
```

示例：

```bash
USRP_SERIAL=目标B210序列号
AMF_IP=10.17.98.200
GNB_N2_IP=192.168.71.140
GNB_N3_IP=192.168.71.140
MCC=262
MNC=99
TAC=1

K3_GNB_MIN_RXTXTIME=8
K3_GNB_SL_AHEAD=8
K3_GNB_REORDER_DISABLE=0
```

B210序列号查询：

```bash
uhd_find_devices
uhd_usrp_probe --args='type=b200'
```

### 5.4 检查AMF、SMF、UPF配置

文件：

```text
ext/cn5gcorenetwork/config/amf.yaml
ext/cn5gcorenetwork/config/smf.yaml
ext/cn5gcorenetwork/config/upf.yaml
```

重点确认：

- AMF N2地址；
- UPF N3地址；
- PLMN、TAC和切片；
- DNN是否为`oai`；
- UE地址池是否为`12.1.1.0/24`；
- 数据库地址是否仍为`127.0.0.1:3306`。

三份配置中的PLMN、SST、DNN必须与gNB和SIM订阅数据一致。

---

## 6. 数据库迁移与SIM订阅

当前数据库运行时：

```text
ext/mysql/bin/mysqld
```

当前私有数据目录：

```text
ext/mysql/data
```

启动检查：

```bash
cd "$OAI_ROOT"
./ext/cn5gcorenetwork/scripts/start-local-mysql.sh
./ext/cn5gcorenetwork/scripts/status-local-mysql.sh
```

不要同时启动系统MySQL/MariaDB，否则会争用TCP 3306端口：

```bash
systemctl is-active mysql
systemctl is-active mariadb
ss -ltnp | grep 3306
```

使用同一张测试SIM时，可以保留现有`oai_db`。更换SIM时，必须更新：

```text
IMSI/SUPI
K
OP或OPc
AMF
SQN
DNN与切片订阅
```

数据库包含SIM鉴权密钥，迁移包必须限制读取权限，不得公开分发。

---

## 7. B210与USB适配

### 7.1 USB 3链路

```bash
lsusb -t
uhd_find_devices
uhd_usrp_probe --args="serial=目标序列号"
```

B210必须工作在USB 3 SuperSpeed链路。USB 2通常无法稳定支持当前15.36 MSps双向流。

### 7.2 USB权限

如果只有root能找到B210，需要安装UHD udev规则，或为目标设备添加等效规则。修改规则后重新加载：

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### 7.3 USB中断名称

当前文件：

```text
scripts/run-a-no-v-gnb.sh
```

`tune_usb_irq()`原先按K3上的：

```text
xhci-hcd:usb5
```

查找中断。目标板通常不同，先检查：

```bash
grep -iE 'xhci|usb' /proc/interrupts
```

然后修改`tune_usb_irq()`的`awk`匹配条件，使其命中B210所在xHCI控制器。不要直接假定仍是`usb5`。

### 7.4 UHD镜像

项目sysroot包含编译使用的UHD库，但目标系统仍要能找到B210所需FPGA/固件镜像。`uhd_usrp_probe`报告缺失镜像时，应安装与当前UHD版本匹配的images，而不是混用其他版本。

---

## 8. 编译OAI

```bash
cd "$OAI_ROOT"
./scripts/build-oai-native.sh
```

脚本固定使用ext内：

```text
GCC/G++
CMake
Ninja
pkg-config
binutils
asn1c
SIMDe
RISC-V sysroot
```

成功时应看到：

```text
Verified no-V RV64 artifact: .../nr-softmodem
Verified no-V RV64 artifact: .../nr-uesoftmodem
Verified no-V RV64 artifact: .../libparams_libconfig.so
Verified no-V RV64 artifact: .../libdfts.so
Verified no-V RV64 artifact: .../liboai_device.so
Build complete
```

如果目标CPU不支持`rv64gc_zba`：

```bash
OAI_RISCV_ARCH=rv64gc ./scripts/build-oai-native.sh
```

---

## 9. 启动与分层验证

### 9.1 启动项目内数据库

```bash
./ext/cn5gcorenetwork/scripts/start-local-mysql.sh
```

### 9.2 启动核心网

```bash
sudo ./ext/cn5gcorenetwork/scripts/start-cn5g.sh
```

应看到：

```text
upf: running
smf: running
amf: running
N4: associated
N2/SCTP 38412: listening
```

### 9.3 启动gNB和B210

推荐统一入口：

```bash
sudo ./ext/cn5gcorenetwork/scripts/run-a-no-v.sh start
```

出现旧会话、No Service或PDU冲突时：

```bash
sudo ./ext/cn5gcorenetwork/scripts/run-a-no-v.sh recover
```

查看日志：

```bash
journalctl -fu oai-k3-b210-gnb.service
tail -f ext/cn5gcorenetwork/logs/amf.log
```

关键射频日志：

```text
Actual TX sample rate: 15.360000MSps
Actual TX frequency: 3.604800GHz
got sync
Received NGSetupResponse
```

关键UE接入链：

```text
Initiating RA procedure
PUSCH with TC_RNTI ... received correctly
Received Ack of Msg4
Received RRCSetupComplete
Authentication successful by network
5GMM-REGISTERED
PDU Session Resource Setup
```

---

## 10. 纯软件RFsim测试

如果目标只是验证编译结果和协议栈，不使用B210，可以采用OAI RFsim。RFsim需要分别启动软件gNB和软件UE，使用同一RFsim服务器地址，并使用与核心网一致的PLMN/IMSI配置。

RFsim与当前B210启动器是两条独立路径：

- 不应给RFsim加载B210序列号；
- 不需要UHD设备；
- 不代表USB、中断和实际空口已经验证；
- RFsim成功后仍需单独进行B210 OTA测试。

具体命令应以当前源码版本的`rfsimulator`参数为准，先检查：

```bash
cmake_targets/ran_build/rv64-no-v-release/nr-softmodem --help | grep -i rfsim
cmake_targets/ran_build/rv64-no-v-release/nr-uesoftmodem --help | grep -i rfsim
```

---

## 11. 移植到FPGA平台

“移植到FPGA”有两种完全不同的含义，必须先确定目标。

### 11.1 FPGA SoC上的RISC-V Linux运行完整OAI

此方案中，FPGA逻辑主要提供RISC-V SoC、DDR、USB/PCIe和网口；OAI仍作为Linux用户态程序运行。

需要完成：

1. RISC-V Linux、glibc和`lp64d`用户态；
2. 至少8核或重新规划线程亲和性；
3. 足够的DDR容量和带宽；
4. USB 3 Host/xHCI控制器及B210稳定DMA；
5. SCTP、TUN/TAP、dummy、iptables内核功能；
6. 高精度定时器和稳定的CPU频率；
7. 避免CPU动态降频造成L1 deadline miss；
8. 按第5至第9章修改网络、B210和启动参数。

FPGA SoC设备树通常还需正确描述：

```text
USB Host/xHCI
PCIe（如果使用其他SDR）
中断控制器
DMA一致性
网卡
时钟
DDR保留区
```

验证顺序：

```text
Linux启动
→ USB 3枚举
→ UHD识别B210
→ 项目内GCC编译
→ 项目内MySQL启动
→ 核心网N4/N2
→ gNB TX/RX同步
→ PRACH/RRC/NAS/PDU
```

### 11.2 将PHY计算真正下沉到FPGA逻辑

这种方案不是修改几个配置文件即可完成，而是一次新的软硬件协同设计。需要确定下沉边界，例如：

```text
FFT/IFFT
LDPC编码/译码
Polar编码/译码
信道估计
预编码
速率匹配
IQ搬运
整个低PHY
```

推荐分层：

```text
OAI MAC/RLC/RRC/NGAP（Linux）
        │ FAPI/nFAPI或自定义低PHY接口
        ▼
FPGA控制驱动 + DMA环形队列
        ▼
FPGA低PHY/加速器
        ▼
射频前端或B210
```

必须开发或修改的内容包括：

1. FPGA RTL/HLS模块；
2. AXI/PCIe/DMA数据通路；
3. Linux内核驱动或UIO/VFIO接口；
4. 用户态设备库；
5. OAI中对应PHY函数的硬件后端；
6. 缓冲区格式、字节序、定点量化和饱和规则；
7. 帧、slot、symbol级时间戳；
8. 中断合并、DMA队列深度和超时处理；
9. 硬件复位、错误恢复和统计接口；
10. 与OAI软件参考实现逐比特对比的验证程序。

如果仍通过B210收发，FPGA加速器与UHD之间还要决定IQ数据路径：

- IQ先进入Linux内存，再由DMA送入FPGA：实现简单，但多一次内存搬运；
- FPGA与USB/PCIe直接交换IQ：延迟更低，但需要重写设备与DMA数据面；
- 不再使用B210，FPGA直接连接RF ADC/DAC：需要自研完整射频接口、时钟同步和校准链路。

### 11.3 FPGA接口时序要求

FPGA下沉后必须在OAI调度窗口内完成：

```text
输入DMA
→ FPGA计算
→ 输出DMA
→ OAI消费结果
```

当前30 kHz SCS下：

```text
1 slot = 0.5 ms
1 OFDM symbol约为35.7 µs（不同CP符号略有差异）
```

硬件接口不能只满足平均吞吐，还必须满足最坏情况延迟。建议为每个模块记录：

```text
输入字节数
DMA延迟
计算周期数
输出延迟
最坏情况抖动
是否跨slot
超时后的恢复策略
```

### 11.4 FPGA移植建议阶段

1. 在目标FPGA SoC的CPU侧先跑通当前纯软件no-V OAI；
2. 使用RFsim验证协议栈；
3. 使用B210验证真实空口；
4. 选择一个可独立验证的模块下沉，例如FFT或LDPC；
5. 保留软件参考路径，运行时可切换软件/FPGA实现；
6. 做逐比特结果对比和压力测试；
7. 测量slot级最坏延迟；
8. 最后再扩大到完整低PHY。

不要在CPU侧OAI尚未稳定时同时调试FPGA PHY，否则无法区分软件、DMA、时钟和RTL问题。

---

## 12. 常见故障定位

### 编译器无法启动

```text
原因：目标系统GLIBC/动态加载器不兼容。
处理：统一根文件系统，或在目标板重新构建工具链。
```

### CMake报告源目录不一致

```text
原因：复制了源平台CMake缓存。
处理：备份并重新创建rv64-no-v-release构建目录。
```

### B210找不到

```text
检查：USB 3链路、udev权限、UHD images、序列号和供电。
```

### gNB启动但没有PRACH

```text
检查：B210 TX、频点、SSB、UE锁频、天线、TX幅度以及UE CFUN复位。
```

### 大量late/underflow/deadline miss

```text
检查：CPU性能、亲和性、USB IRQ、降频、DDR带宽、USB链路和实时调度参数。
```

### 核心网启动失败并提示3306被占用

```text
原因：系统MySQL/MariaDB与项目数据库同时运行。
处理：停止系统数据库，仅运行ext/mysql实例。
```

### UE注册成功但没有数据地址

```text
检查：DNN、SMF/UPF、旧PDU Session、tun0、NAT、FORWARD规则和Windows MBIM状态。
```

---

## 13. 最终验收清单

- [ ] `uname -m`为`riscv64`；
- [ ] CPU支持配置的ISA和ABI；
- [ ] ext GCC能编译并运行测试程序；
- [ ] OAI全量构建完成并通过no-V检查；
- [ ] 项目内MySQL运行，系统数据库停止；
- [ ] `oai_db`包含正确SIM订阅；
- [ ] UPF、SMF、AMF运行；
- [ ] N4 associated；
- [ ] N2/SCTP listening；
- [ ] UHD识别目标B210；
- [ ] B210工作在USB 3；
- [ ] USB IRQ和CPU亲和性已适配目标板；
- [ ] gNB收到NG Setup Response；
- [ ] RU显示got sync；
- [ ] UE完成PRACH、RRC、NAS注册；
- [ ] UE获得`12.1.1.x`地址；
- [ ] UE可访问gNB N3地址；
- [ ] UE可访问外网；
- [ ] 运行期间无持续underflow、overflow或deadline miss。

只有全部通过后，才能认为目标RISC-V/FPGA SoC平台完成了端到端移植。
