#!/usr/bin/env bash
# Usage: ./flatten_graphicsplayground_recursive.sh
# 현재 디렉토리: GraphicsPlayground
# 목표: GraphicsPlayground/GraphicsPlayground/**/* → 상위 GraphicsPlayground/

set -e
shopt -s globstar dotglob  # ** 와 숨김파일 포함

INNER_DIR="./GraphicsPlayground"

if [[ ! -d "$INNER_DIR" ]]; then
  echo "[ERROR] '$INNER_DIR' folder not found."
  exit 1
fi

echo "====================================================="
echo "Recursively flattening '$INNER_DIR' into current directory..."
echo "Git tracked files will keep history."
echo "Conflicts will be skipped."
echo "====================================================="
echo

# 모든 파일과 폴더 재귀적으로 순회
for src in "$INNER_DIR"/**/*; do
  # 실제 파일만
  if [[ -f "$src" ]]; then
    # 상위 폴더 기준 상대 경로
    rel_path="${src#$INNER_DIR/}"
    dest="./$rel_path"

    # 중간 디렉토리 없으면 생성
    dest_dir=$(dirname "$dest")
    mkdir -p "$dest_dir"

    # 이름 충돌 방지
    if [[ -e "$dest" ]]; then
      echo "⚠️  Skip (name conflict): $rel_path"
      continue
    fi

    # Git tracked 여부 확인
    if git ls-files --error-unmatch "$src" &>/dev/null; then
      git mv "$src" "$dest"
      echo "🔹 git mv: $rel_path → $dest"
    else
      mv "$src" "$dest"
      echo "🔸 mv: $rel_path → $dest"
    fi
  fi
done

echo
echo "✅ Done. All possible files moved. Conflicts were skipped."
