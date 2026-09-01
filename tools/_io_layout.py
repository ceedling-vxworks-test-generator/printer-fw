"""Shared input/output-folder convention for tools/*.py.

Each tool defaults to two folders that live next to its own script:

    <script_dir>/<script_stem>_target/   -- put files to analyze here
    <script_dir>/<script_stem>_output/   -- results are written here automatically

Every regular file under the target folder is scanned recursively,
*regardless of extension* -- there is no .hpp/.cpp filter. A file that
doesn't look like C++ simply won't match any pattern and is harmless
to include; this lets you drop in files with unusual/missing
extensions without the tool silently skipping them.

Both folders are created on first use if missing. Their contents are
gitignored (see tools/.gitignore) except for a short README explaining
what belongs there, so the folders themselves stay discoverable in a
fresh clone without accumulating whatever each run copies in/out.
"""

from __future__ import annotations

from pathlib import Path

SKIP_DIR_NAMES = {".git", "__pycache__", ".vscode", ".devcontainer", ".github"}

_TARGET_README = """\
# {name}_target/

このフォルダの中身（サブフォルダ含む、拡張子問わず）を再帰的に解析対象とします。
解析したいファイル一式をここにコピーしてから `python3 {name}.py` を実行してください。
"""

_OUTPUT_README = """\
# {name}_output/

`python3 {name}.py` の実行結果（CSV/Excel）がここに出力されます。
"""


def _ensure_dir_with_readme(d: Path, template: str, script_stem: str) -> Path:
    d.mkdir(parents=True, exist_ok=True)
    readme = d / "README.md"
    if not readme.exists():
        readme.write_text(template.format(name=script_stem), encoding="utf-8")
    return d


def target_dir_for(script_file: str | Path) -> Path:
    p = Path(script_file).resolve()
    return _ensure_dir_with_readme(p.parent / f"{p.stem}_target", _TARGET_README, p.stem)


def output_dir_for(script_file: str | Path) -> Path:
    p = Path(script_file).resolve()
    return _ensure_dir_with_readme(p.parent / f"{p.stem}_output", _OUTPUT_README, p.stem)


def iter_all_files(target_dir: Path):
    """Yield every regular file under target_dir, recursively, any extension."""
    for p in sorted(target_dir.rglob("*")):
        if not p.is_file():
            continue
        if p.name == "README.md" and p.parent == target_dir:
            continue
        if any(part in SKIP_DIR_NAMES for part in p.parts):
            continue
        yield p
