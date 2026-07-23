#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ex/*.md -> ex/html/*.html 生成器（再現可能なHTML再生成）

- Python標準的な `markdown` パッケージを使用（tables/fenced_code/toc）。
- ```mermaid フェンスは <div class="mermaid">...</div> へ変換し、CDNのmermaid.jsで描画。
- _deprecated_旧世代/ は対象外。index.html も再生成する。

使い方: python ex/tools/gen_html.py   （リポジトリ直下から実行）
"""
import os
import re
import html as htmllib
import markdown

EX_DIR = os.path.join(os.path.dirname(__file__), "..")
EX_DIR = os.path.abspath(EX_DIR)
HTML_DIR = os.path.join(EX_DIR, "html")

STYLE = """<style>
body { font-family: "Yu Gothic UI","Meiryo","Hiragino Sans",sans-serif; line-height:1.75;
       max-width: 960px; margin: 0 auto; padding: 24px 32px 80px; color:#1f2328; background:#fff; }
h1 { border-bottom: 3px solid #0969da; padding-bottom:.3em; }
h2 { border-bottom: 1px solid #d0d7de; padding-bottom:.25em; margin-top:2em; }
h3 { margin-top:1.6em; }
table { border-collapse: collapse; margin: 1em 0; width:100%; font-size:.95em; }
th, td { border: 1px solid #d0d7de; padding: 6px 12px; text-align:left; vertical-align:top; }
th { background:#f6f8fa; }
tr:nth-child(even) td { background:#fafbfc; }
code { background:#f6f8fa; padding:.15em .4em; border-radius:4px;
       font-family:"Consolas","MS Gothic",monospace; font-size:.9em; }
pre { background:#f6f8fa; border:1px solid #d0d7de; border-radius:6px; padding:14px; overflow-x:auto; }
pre code { background:none; padding:0; }
blockquote { border-left:4px solid #0969da; margin:1em 0; padding:.3em 1em; color:#57606a; background:#f6f8fa; }
.mermaid { background:#fff; border:1px dashed #d0d7de; border-radius:6px;
           padding:12px; margin:1em 0; text-align:center; }
.docnav { font-size:.9em; margin-bottom:1.2em; }
.docnav a { color:#0969da; text-decoration:none; }
footer { margin-top:3em; font-size:.8em; color:#8b949e; border-top:1px solid #d0d7de; padding-top:1em; }
</style>"""

MERMAID_SCRIPT = """<script type="module">
import mermaid from "https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.esm.min.mjs";
mermaid.initialize({ startOnLoad: true, theme: "neutral", securityLevel: "loose" });
</script>"""

MERMAID_FENCE = re.compile(r"```mermaid\n(.*?)```", re.S)


def render_md(md_text: str) -> str:
    # mermaidフェンスを退避してプレースホルダに置換
    blocks = []

    def stash(m):
        blocks.append(m.group(1))
        return f"\n\n@@MERMAID{len(blocks)-1}@@\n\n"

    tmp = MERMAID_FENCE.sub(stash, md_text)
    body = markdown.markdown(tmp, extensions=["tables", "fenced_code", "sane_lists"])
    # プレースホルダ（<p>@@MERMAIDn@@</p> になる）を mermaid div へ戻す
    for i, code in enumerate(blocks):
        div = '<div class="mermaid">\n' + htmllib.escape(code) + "</div>"
        body = body.replace(f"<p>@@MERMAID{i}@@</p>", div)
        body = body.replace(f"@@MERMAID{i}@@", div)
    return body


def page(title: str, body: str, rel_md: str) -> str:
    return (
        "<!DOCTYPE html>\n<html lang=\"ja\">\n<head>\n<meta charset=\"utf-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        f"<title>{htmllib.escape(title)}</title>\n{STYLE}\n</head>\n<body>\n"
        f"{body}\n"
        f"<footer>printer-fw RIM 設計書（Markdown原本: {htmllib.escape(rel_md)}）</footer>\n"
        f"{MERMAID_SCRIPT}\n</body>\n</html>\n"
    )


def iter_md():
    for root, dirs, files in os.walk(EX_DIR):
        if "_deprecated" in root or os.path.join(EX_DIR, "html") in root or os.path.join(EX_DIR, "tools") in root:
            continue
        for f in sorted(files):
            if f.endswith(".md"):
                yield os.path.join(root, f)


def main():
    generated = []
    for md_path in iter_md():
        rel = os.path.relpath(md_path, EX_DIR).replace("\\", "/")
        out_path = os.path.join(HTML_DIR, rel[:-3] + ".html")
        os.makedirs(os.path.dirname(out_path), exist_ok=True)
        text = open(md_path, encoding="utf-8").read()
        # H1をタイトルに（無ければファイル名）
        m = re.search(r"^#\s+(.+)$", text, re.M)
        title = m.group(1).strip() if m else os.path.basename(md_path)[:-3]
        html_out = page(title, render_md(text), rel)
        open(out_path, "w", encoding="utf-8").write(html_out)
        generated.append(rel[:-3] + ".html")
        print("gen:", rel[:-3] + ".html")

    write_index(generated)


def write_index(generated):
    # フォルダ単位でグルーピングして目次を作る
    groups = {}
    for rel in generated:
        parts = rel.split("/")
        grp = parts[0] if len(parts) > 1 else "横断"
        groups.setdefault(grp, []).append(rel)

    order = ["横断", "共通", "Adapter", "DataStore", "Capability", "Publisher", "Accessor"]
    def gkey(g):
        return order.index(g) if g in order else len(order)

    lis = []
    for grp in sorted(groups, key=gkey):
        items = "".join(
            f'<li><a href="{htmllib.escape(rel)}">{htmllib.escape(os.path.basename(rel)[:-5])}</a></li>'
            for rel in sorted(groups[grp])
        )
        lis.append(f"<h2>{htmllib.escape(grp)}</h2><ul>{items}</ul>")

    body = (
        "<h1>printer-fw RIM（ReactiveInfoManager）設計書一覧</h1>\n"
        "<p>4レイヤ（L1 RIM_AdapterLayer / L2 RIM_DatastoreLayer / L3 RIM_CapabilityLayer / "
        "L4 RIM_PublisherLayer）＋隣接 Accessor Layer。原本は同階層のMarkdown。"
        "旧世代は <code>_deprecated_旧世代/</code>（HTML化対象外）。</p>\n"
        + "".join(lis)
    )
    out = (
        "<!DOCTYPE html>\n<html lang=\"ja\">\n<head>\n<meta charset=\"utf-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        "<title>printer-fw RIM 設計書一覧</title>\n" + STYLE + "\n</head>\n<body>\n"
        + body + "\n<footer>generated from ex/*.md by ex/tools/gen_html.py</footer>\n</body>\n</html>\n"
    )
    open(os.path.join(HTML_DIR, "index.html"), "w", encoding="utf-8").write(out)
    print("gen: index.html")


if __name__ == "__main__":
    main()
