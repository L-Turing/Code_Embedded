"""
parse_log.py — 解析 SD 卡日志文件 log.bin

用法：
  python parse_log.py log.bin              # 列出所有有效块
  python parse_log.py log.bin --all        # 列出所有块（含损坏块）
  python parse_log.py log.bin --last       # 只看最后一个有效块
  python parse_log.py log.bin -o out.json  # 导出 JSON
"""

import argparse
import json
import struct
import sys
import zlib
from dataclasses import dataclass, field
from pathlib import Path

# ============================================================
# SD_LogBlock 结构体定义 (#pragma pack(1), 264 bytes)
# ============================================================
#   uint32_t seq;          offset  0 (4B)
#   uint32_t timestamp_us; offset  4 (4B)
#   uint16_t type;         offset  8 (2B)
#   uint16_t length;       offset 10 (2B)
#   uint32_t crc;          offset 12 (4B)
#   uint8_t  payload[248]; offset 16 (248B)
#   Total: 264 bytes
#
# CRC 计算 (与 SD_LogWriteBlock 一致):
#   local.crc = CRC32_Calc(&local, sizeof(local) - sizeof(local.crc));
#   对结构体前 260 字节计算 CRC32，此时 crc 字段为 0

BLOCK_SIZE = 264
CRC_LEN = 260
HEADER_FMT = "<IIHHI"
HEADER_SIZE = struct.calcsize(HEADER_FMT)

# ============================================================
# 类型名称映射 — 按你的需求在这里添加
# ============================================================
TYPE_NAMES = {
    0x0001: "测试文本",
    0x0010: "IMU 数据",
    0x0011: "GNSS 数据",
    0x0020: "系统状态",
    0x00FF: "错误信息",
}


def get_type_name(typ: int) -> str:
    name = TYPE_NAMES.get(typ)
    return f"0x{typ:04X} ({name})" if name else f"0x{typ:04X}"


# ============================================================
# Payload 解码器 — 按 type 编号注册解码函数
# ============================================================

def _decode_text(payload: bytes, length: int) -> str:
    """默认解码: 尝试当 ASCII/UTF-8 文本显示"""
    raw = payload[:length]
    # 如果全是可打印 ASCII 字符，直接当文本显示
    if all(0x20 <= b <= 0x7E for b in raw):
        return raw.decode("ascii")
    # 否则显示 hex + 尝试 UTF-8
    try:
        s = raw.decode("utf-8")
        if s.isprintable():
            return s
    except UnicodeDecodeError:
        pass
    return raw.hex(" ")


def _decode_imu(payload: bytes, length: int) -> str:
    """示例: IMU 数据解码 (假设 6×float = 24 bytes)"""
    if length < 24:
        return _decode_text(payload, length)
    ax, ay, az, gx, gy, gz = struct.unpack_from("<ffffff", payload, 0)
    return (f"accel:({ax:+.2f} {ay:+.2f} {az:+.2f}) g  "
            f"gyro:({gx:+.2f} {gy:+.2f} {gz:+.2f}) dps")


def _decode_gnss(payload: bytes, length: int) -> str:
    """示例: GNSS 数据解码 (NMEA 文本)"""
    return _decode_text(payload, length)


# 解码器注册表: type → 解码函数
DECODERS = {
    0x0001: _decode_text,
    0x0010: _decode_imu,
    0x0011: _decode_gnss,
    0x0020: _decode_text,
    0x00FF: _decode_text,
}


def decode_payload(typ: int, payload: bytes, length: int) -> str:
    """根据 type 解码 payload 为可读字符串"""
    decoder = DECODERS.get(typ, _decode_text)
    return decoder(payload, length)


# ============================================================
# 解析逻辑
# ============================================================

@dataclass
class LogBlock:
    index: int
    seq: int
    timestamp_us: int
    typ: int
    length: int
    crc: int
    crc_valid: bool
    payload: bytes
    offset: int
    # 解码后缓存
    _decoded: str = field(default="", repr=False)

    def decoded(self) -> str:
        if not self._decoded:
            self._decoded = decode_payload(self.typ, self.payload, self.length)
        return self._decoded


def parse_block(data: bytes, offset: int, index: int) -> LogBlock:
    block = data[offset : offset + BLOCK_SIZE]
    seq, ts, typ, length, crc_stored = struct.unpack_from(HEADER_FMT, block, 0)
    payload = block[16:]

    # CRC 校验
    crc_input = bytearray(block[:CRC_LEN])
    crc_input[12:16] = b'\x00\x00\x00\x00'
    crc_calc = zlib.crc32(bytes(crc_input), 0xFFFFFFFF) & 0xFFFFFFFF

    return LogBlock(
        index=index, seq=seq, timestamp_us=ts,
        typ=typ, length=length, crc=crc_stored,
        crc_valid=(crc_calc == crc_stored),
        payload=payload, offset=offset,
    )


def read_all_blocks(filepath: str) -> list[LogBlock]:
    data = Path(filepath).read_bytes()
    total = len(data) // BLOCK_SIZE
    remnant = len(data) % BLOCK_SIZE
    if remnant:
        print(f"警告: 文件 {len(data)} 字节，末尾 {remnant} 字节忽略\n")
    return [parse_block(data, i * BLOCK_SIZE, i) for i in range(total)]


# ============================================================
# 输出
# ============================================================

def fmt_ts(us: int) -> str:
    ms = us / 1000
    if ms >= 60000:
        return f"{ms / 60000:.2f} min"
    if ms >= 1000:
        return f"{ms / 1000:.2f} s"
    return f"{ms:.1f} ms"


def print_block(b: LogBlock):
    tag = "✓" if b.crc_valid else "✗ CRC错误"
    print(f"┌─ [{b.index}] seq={b.seq}  ts={b.timestamp_us}us ({fmt_ts(b.timestamp_us)})")
    print(f"├─ type={get_type_name(b.typ)}  length={b.length}  crc=0x{b.crc:08X}  [{tag}]")
    print(f"└─ payload: {b.decoded()}")
    print()


def cmd_list(args):
    blocks = read_all_blocks(args.file)
    valid = [b for b in blocks if b.crc_valid]
    bad = len(blocks) - len(valid)

    if args.last:
        if valid:
            print_block(valid[-1])
        else:
            print("没有有效块")
        return

    if not args.all and bad:
        print(f"共 {len(blocks)} 块，有效 {len(valid)}，损坏 {bad}\n")

    show = blocks if args.all else valid
    print(f"{'全部' if args.all else '有效'}块: {len(show)} 个\n")
    for b in show:
        print_block(b)


def cmd_json(args):
    blocks = read_all_blocks(args.file)
    result = []
    for b in blocks:
        result.append({
            "index": b.index,
            "seq": b.seq,
            "timestamp_us": b.timestamp_us,
            "type": get_type_name(b.typ),
            "type_raw": f"0x{b.typ:04X}",
            "length": b.length,
            "crc": f"0x{b.crc:08X}",
            "crc_valid": b.crc_valid,
            "payload": b.decoded(),
            "payload_hex": b.payload[:b.length].hex() if b.length <= 248 else b.payload.hex(),
        })
    out = args.output or "parsed.json"
    Path(out).write_text(json.dumps(result, indent=2, ensure_ascii=False), encoding="utf-8")
    print(f"导出 {len(result)} 块 → {out}")


def main():
    p = argparse.ArgumentParser(description="解析 SD 卡日志 log.bin")
    p.add_argument("file", help="log.bin 路径")
    p.add_argument("--all", "-a", action="store_true", help="显示所有块(含CRC损坏)")
    p.add_argument("--last", "-l", action="store_true", help="只显示最后有效块")
    p.add_argument("--output", "-o", metavar="FILE", help="导出 JSON")
    args = p.parse_args()

    if not Path(args.file).exists():
        print(f"错误: 文件不存在 — {args.file}")
        sys.exit(1)

    if args.output:
        cmd_json(args)
    else:
        cmd_list(args)


if __name__ == "__main__":
    main()
