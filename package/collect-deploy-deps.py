#!/usr/bin/env python3
"""按真实 PE 导入表递归补齐、并校验部署目录里的运行时 DLL。

为什么需要它
------------
打包步骤里手写一长串 ``cp C:/msys64/... /deploy/`` 是**易碎**的：

* 上游一改 soname（实测：``libxml2-2.dll`` → ``libxml2-16.dll``、
  ``libcares-5.dll`` → ``libcares-2.dll``）或换掉某个依赖（``libssl-1_1-x64``
  → ``libssl-3-x64``），这一串 cp 就会**静默**漏文件（PowerShell 的
  ``Copy-Item`` 报错是非终止性的，job 照样绿）；
* 漏一个直接依赖 = 进程连启动都做不到（``ModManager.exe`` 静态导入
  ``libaria2-0.dll``、``libquazip1-qt6.dll``）；
* 更隐蔽的是 **ABI 不匹配**：DLL 都在，但导入的符号在运行库的新版本里才
  存在。实测过一次：MSYS2 的 ``libquazip1-qt6.dll`` 是针对 Qt 6.11 编的，
  导入 ``qt_version_tag_6_11`` / ``QDir::mkpath(..., std::optional<...>)``，
  配官方 Qt 6.7.2 会在加载阶段直接失败。

所以这里做两件事：**递归补齐**（一个不漏）＋**符号级校验**（补来的 DLL
真的能满足调用方）。

用法
----
  python package/collect-deploy-deps.py --deploy deploy \
      --search C:/msys64/mingw64/bin --search <quazip>/bin --search <Qt>/bin

  # 只校验不拷贝（作为打包后的守门步骤）
  python package/collect-deploy-deps.py --deploy deploy --check-only

退出码非 0 表示存在「无法解析的导入」或「符号缺失」，CI 会因此失败。
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys

# 这些是 Windows 自带 / 由系统提供的 DLL，不作为依赖收集
SYSTEM_PREFIXES = (
    "api-ms-win-", "ext-ms-win-", "ucrtbase", "vcruntime",
)
SYSTEM_NAMES = {
    "advapi32.dll", "bcrypt.dll", "comctl32.dll", "comdlg32.dll", "crypt32.dll",
    "d2d1.dll", "d3d9.dll", "d3d11.dll", "d3d12.dll", "dcomp.dll", "dwmapi.dll",
    "dxgi.dll", "gdi32.dll", "gdiplus.dll", "imm32.dll", "iphlpapi.dll",
    "kernel32.dll", "mf.dll", "mfplat.dll", "mfreadwrite.dll", "mpr.dll",
    "msvcrt.dll", "mswsock.dll", "ncrypt.dll", "netapi32.dll", "normaliz.dll",
    "ntdll.dll", "odbc32.dll", "ole32.dll", "oleaut32.dll", "opengl32.dll",
    "powrprof.dll", "propsys.dll", "psapi.dll", "rpcrt4.dll", "secur32.dll",
    "setupapi.dll", "shcore.dll", "shell32.dll", "shlwapi.dll", "user32.dll",
    "userenv.dll", "uxtheme.dll", "version.dll", "wer.dll", "winhttp.dll",
    "wininet.dll", "winmm.dll", "winspool.drv", "wintrust.dll", "ws2_32.dll",
    "wsock32.dll", "wtsapi32.dll", "cryptbase.dll", "dnsapi.dll",
}

PE_EXT = (".exe", ".dll")

# Windows 上 Python 的 stdout/stderr 在**被重定向**（管道 / 文件）时按 **ANSI 代码页**
# 编码 —— en-US 的 runner 上是 cp1252，本脚本要打中文，于是第一行 print 就
# UnicodeEncodeError、traceback 直接 exit 1（CI 里表现为"只看到 exit code 1，
# 完全没有上下文"，极难定位）。强制 UTF-8 与 CI 日志的编码对齐，并让不可编码的字符
# 降级而不是抛异常；控制台代码页是多少都与本脚本无关。
for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8", errors="replace")
    except (AttributeError, OSError, ValueError):  # 老 Python / 非标准流
        pass


def log(msg):
    print(msg, flush=True)


def find_objdump(explicit, search_dirs):
    """objdump 是解析 PE 的手段；优先用显式路径，其次 PATH，再次 Qt 工具链里找。"""
    if explicit:
        if os.path.isfile(explicit):
            return explicit
        sys.exit(f"[FATAL] 指定的 objdump 不存在: {explicit}")
    found = shutil.which("objdump") or shutil.which("objdump.exe")
    if found:
        return found
    # <Qt>/Tools/<mingw>/bin/objdump.exe —— 传进来的 Qt bin 目录的兄弟目录
    for d in search_dirs:
        for up in (os.path.join(d, os.pardir, os.pardir, "Tools"),
                   os.path.join(d, os.pardir, os.pardir, os.pardir, "Tools")):
            if not os.path.isdir(up):
                continue
            for name in sorted(os.listdir(up)):
                cand = os.path.join(up, name, "bin", "objdump.exe")
                if os.path.isfile(cand):
                    return cand
    sys.exit("[FATAL] 找不到 objdump.exe（它在 MinGW 工具链的 bin 里），用 --objdump 指定")


class Objdump:
    def __init__(self, path):
        self.path = path
        self._cache = {}

    def _run(self, path):
        if path in self._cache:
            return self._cache[path]
        out = subprocess.run([self.path, "-p", path], capture_output=True,
                             text=True, errors="replace").stdout
        self._cache[path] = out
        return out

    def imports(self, path):
        """{dll小写名: [符号名, ...]}；符号名为 PE 导入名（可能是 C++ mangled）。"""
        res, cur = {}, None
        for line in self._run(path).splitlines():
            m = re.match(r"\s*DLL Name:\s*(\S+)", line)
            if m:
                cur = m.group(1).lower()
                res.setdefault(cur, [])
                continue
            if cur is not None and line.startswith("\t"):
                m2 = re.match(r"\s+[0-9a-fA-F]{4,}\s+\d+\s+(\S+)", line)
                if m2:
                    res[cur].append(m2.group(1))
        return res

    def exports(self, path):
        """导出符号名集合。"""
        res, in_tbl = set(), False
        for line in self._run(path).splitlines():
            if "[Ordinal/Name Pointer] Table" in line:
                in_tbl = True
                continue
            if in_tbl:
                if not line.strip():
                    in_tbl = False
                    continue
                parts = line.split()
                if len(parts) >= 2:
                    res.add(parts[-1])
        return res


def is_system(name, system_dir):
    low = name.lower()
    if low in SYSTEM_NAMES or low.startswith(SYSTEM_PREFIXES):
        return True
    return os.path.isfile(os.path.join(system_dir, low)) if system_dir else False


def pe_files(root):
    for dirpath, _dirnames, filenames in os.walk(root):
        for fn in filenames:
            if fn.lower().endswith(PE_EXT):
                yield os.path.join(dirpath, fn)


def main():
    ap = argparse.ArgumentParser(description="递归补齐并校验部署目录的运行时 DLL")
    ap.add_argument("--deploy", required=True, help="部署目录（windeployqt 产物的落点）")
    ap.add_argument("--search", action="append", default=[],
                    help="DLL 来源目录，可重复；顺序即优先级（前面的先被选中）")
    ap.add_argument("--objdump", default=None, help="objdump.exe 路径（默认自动探测）")
    ap.add_argument("--system-dir", default=os.environ.get("SystemRoot", r"C:\Windows") + r"\System32")
    ap.add_argument("--check-only", action="store_true", help="只校验，不拷贝")
    ap.add_argument("--json", default=None, help="把清单写成 JSON")
    ap.add_argument("--symbol-check", dest="symbol_check", action="store_true", default=True)
    ap.add_argument("--no-symbol-check", dest="symbol_check", action="store_false")
    args = ap.parse_args()

    deploy = os.path.abspath(args.deploy)
    if not os.path.isdir(deploy):
        sys.exit(f"[FATAL] 部署目录不存在: {deploy}")
    search = [os.path.abspath(d) for d in args.search]
    for d in search:
        if not os.path.isdir(d):
            sys.exit(f"[FATAL] --search 目录不存在: {d}")
    od = Objdump(find_objdump(args.objdump, search))

    log(f"部署目录 : {deploy}")
    for d in search:
        log(f"来源目录 : {d}")
    log(f"objdump  : {od.path}")
    log(f"系统目录 : {args.system_dir}")

    copied = {}          # 文件名 -> (来源, 被谁需要)
    unresolved = []      # (需要者, dll名)

    # ---- 1. 递归补齐 ----
    if not args.check_only:
        log("\n== 递归补齐开始 ==")
        changed = True
        while changed:
            changed = False
            missing = {}
            for f in pe_files(deploy):
                for dll in od.imports(f):
                    if is_system(dll, args.system_dir):
                        continue
                    if os.path.isfile(os.path.join(deploy, dll)):
                        continue
                    if os.path.basename(f).lower() == dll:
                        continue
                    missing.setdefault(dll, []).append(os.path.relpath(f, deploy))
            for dll, users in sorted(missing.items()):
                src = None
                for d in search:
                    cand = os.path.join(d, dll)
                    if os.path.isfile(cand):
                        src = cand
                        break
                if src is None:
                    unresolved.append((dll, users))
                    continue
                dst = os.path.join(deploy, dll)
                shutil.copy2(src, dst)
                copied[dll] = (src, users)
                log(f"  + {dll:<28} <- {src}   (被 {len(users)} 个文件需要: {', '.join(users[:3])}"
                    f"{', ...' if len(users) > 3 else ''})")
                changed = True
        if not copied:
            log("  （无需补充：windeployqt 已经带全）")

    # ---- 2. 校验：不能有解析不了的导入 ----
    log("\n== 校验导入是否都有落点 ==")
    total_imports, local_imports = 0, 0
    for f in sorted(pe_files(deploy)):
        for dll, syms in od.imports(f).items():
            total_imports += 1
            if is_system(dll, args.system_dir):
                continue
            if os.path.isfile(os.path.join(deploy, dll)):
                local_imports += 1
                continue
            unresolved.append((dll, [os.path.relpath(f, deploy)]))
    log(f"  部署目录内 PE 文件 {len(list(pe_files(deploy)))} 个，"
        f"导入项 {total_imports} 个（其中 {local_imports} 个由本地 DLL 满足）")

    # ---- 3. 校验：符号级（ABI）----
    sym_missing = []
    if args.symbol_check:
        log("\n== 校验符号级 ABI（本地 DLL 是否真的导出被导入的符号）==")
        exp_cache = {}
        checked = 0
        for f in sorted(pe_files(deploy)):
            for dll, syms in od.imports(f).items():
                if is_system(dll, args.system_dir):
                    continue
                prov = os.path.join(deploy, dll)
                if not os.path.isfile(prov):
                    continue
                if prov not in exp_cache:
                    exp_cache[prov] = od.exports(prov)
                exports = exp_cache[prov]
                if not exports:
                    log(f"  [WARN] {dll} 解析不出导出表，跳过符号校验")
                    continue
                checked += 1
                for s in syms:
                    if s not in exports:
                        sym_missing.append((os.path.relpath(f, deploy), dll, s))
        log(f"  检查了 {checked} 组「调用方 -> 本地 DLL」，"
            f"缺失符号 {len(sym_missing)} 个")
        for caller, dll, s in sym_missing[:20]:
            log(f"    [MISS] {caller} -> {dll} :: {s}")

    # ---- 4. 总表 ----
    log("\n== 本次补充的 DLL 清单 ==")
    for dll in sorted(copied):
        src, _users = copied[dll]
        log(f"  {dll:<30} {os.path.getsize(os.path.join(deploy, dll)):>10,} B   <- {src}")

    if args.json:
        with open(args.json, "w", encoding="utf-8") as fp:
            json.dump({
                "deploy": deploy,
                "search": search,
                "copied": {k: {"source": v[0], "needed_by": v[1]} for k, v in copied.items()},
                "unresolved": [{"dll": d, "needed_by": u} for d, u in unresolved],
                "symbol_missing": [{"caller": c, "dll": d, "symbol": s} for c, d, s in sym_missing],
            }, fp, indent=2, ensure_ascii=False)
        log(f"\n清单已写入 {args.json}")

    ok = not unresolved and not sym_missing
    log("\n== 结论 ==")
    if unresolved:
        log(f"  [FAIL] 有 {len(set(d for d, _ in unresolved))} 个导入找不到来源：")
        for dll, users in sorted(set(unresolved)):
            log(f"      {dll}  <- {', '.join(users[:3])}")
    if sym_missing:
        log(f"  [FAIL] 有 {len(sym_missing)} 个符号缺失（ABI 不匹配）")
    if ok:
        log("  [PASS] 部署目录自洽：无缺失依赖、无缺失符号")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
