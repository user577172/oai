# A no-V RF validation — 2026-10-24

Outcome: core and gNB running, UE communication NOT established.
A: 10.17.98.165. B: 10.17.116.247 (read-only reference; no RF launched).
B210 serial: 31EC606. Windows UE: Quectel RM520N-GL, same SIM as B validation.

## Final configuration
- Binary and OAI plugins: cmake_targets/ran_build/rv64-no-v-release.
- Launcher: ext/cn5gcorenetwork/scripts/run-a-no-v.sh start|stop|status.
- Explicit ext/config template based on B snapshot; PLMN 262/99, TAC 1, band 78, 24 PRB, SSB ARFCN 640320.
- AMF 10.17.98.165; gNB N2/N3 192.168.71.140; UPF N3 192.168.71.134.
- min_rxtxtime 12, sl_ahead 10; RU worker cores 1,3,4,5,6; TX thread mode 1; continuous TX disabled.
- Core YAML matches B with A's address substituted; DNS entries aligned to B (8.8.8.8, 1.1.1.1). SMF configuration already matched.
- Windows mode_pref restored to AUTO after temporary NR5G-only diagnostic.

## Evidence and limits
- Existing startup was B reference nr-softmodem, not A no-V.
- N4 associated, N2 SCTP listens, NGSetupResponse received with A no-V.
- No UE PRACH/RRC registration observed in A no-V tests. Windows modem saw other n78 cells but not registered to 26299.
- Initial no-V run had U/L underrun/late markers; pinned RU workers and additional timing margin reduced these in observed windows.
- Synchronous TX and continuous-TX diagnostics did not establish registration. Continuous TX reintroduced U/L markers and was reverted.
- Main binary, LDPC, DFT, USRP and config libraries disassembled: zero V-instruction matches. libldpc has broad ELF ISA metadata including V, despite no V instructions found; no metadata or binaries modified.
- Direct runtime idft512 tests: relative RMS error 0.000106817 (DC), 0.000742466 (bin 7). These limited tests do not validate all PHY processing.
- Tracked source diff unchanged from initial snapshot: True. No LDPC/DFT/other production source edits or rebuilds.
- Backup: /home/ubuntu/openairinterface5g/ext/cn5gcorenetwork/backups/20261024-191048
- Full session journal: /home/ubuntu/openairinterface5g/ext/cn5gcorenetwork/diagnostics/gnb-session.log

Next diagnostic: capture/decode actual A downlink IQ/SSB or inspect RF transmit sample path; core config alone has not resolved the issue. Do not substitute B or RVV binaries while reporting A no-V validation.

## Follow-up IQ investigation
TX snapshot a-tx-frame1.iq: 153600 complex int16 samples, fs=15.36MHz, FFT512. PSS symbol2, start bin448, NID2=0, correlation 0.9999854084584915.
SSS candidates [(0, 0.9999878773330683), (954, 0.13464306285284663), (358, 0.13460617489173898)]
RX snapshot: no convincing PSS match; normal correlation 0.181697, conjugate 0.174999. RX snapshot alone cannot establish absent RF due snapshot/timing/antenna conditions.
Wire conversion diagnostic: compiled unchanged A converter source with rv64gc_zba, matched native UHD sc16 -> sc16_item32_le for all 8 tested int16 lanes. Does not establish full runtime correctness.
GDB: tx_count 1062655, queue count 0, last TX timestamp 61337 samples (~3.99ms) ahead of RX timestamp.
GDB stops perturb real-time radio; restart after sampling.

Production tracked source unchanged: True
Restored/restarted explicit A no-V launcher after GDB snapshots. No production binary replaced. UE registration remains unconfirmed.
