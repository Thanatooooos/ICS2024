#!/bin/bash

# usage: addenv env_name path
# path should be absolute
function addenv() {
  # 删除旧的 export 行
  sed -i -e "/^export $1=.*/d" ~/.bashrc
  # 写入新的 export，path 已是绝对路径
  echo "export $1=\"$2\"" >> ~/.bashrc
  echo "✅ Environment variable '$1' set to: $2"
}

# usage: init repo branch directory trace [env]
# trace = true|false
function init() {
  local repo="$1"
  local branch="$2"
  local dir="$3"
  local trace="$4"
  local env_name="$5"

  if [ -d "$dir" ]; then
    echo "📁 $dir is already initialized, skipping..."
    # 如果已有目录且指定了 env_name，则仍应确保环境变量正确
    if [ -n "$env_name" ]; then
      local abs_path="$(readlink -f "$dir")"
      addenv "$env_name" "$abs_path"
    fi
    return
  fi

  echo "📥 Cloning $repo into $dir..."
  git clone -b "$branch" "git@github.com:$repo.git" "$dir"
  while [ $? -ne 0 ]; do
    echo "⚠️  Clone failed. Retrying..."
    rm -rf "$dir"
    git clone -b "$branch" "git@github.com:$repo.git" "$dir"
  done

  local log="$repo $(cd "$dir" && git log --oneline --no-abbrev-commit -n1)"
  echo "📌 Commit: $log"

  if [ "$trace" == "true" ]; then
    # 跟踪整个目录
    git add -A "$dir"
    git commit -m "$repo $branch initialized"$'\n\n'"$log"
  else
    # 不跟踪，加入 .gitignore
    sed -i -e "/^\/$dir\$/d" .gitignore 2>/dev/null || true
    echo "/$dir" >> .gitignore
    git add .gitignore
    git commit --no-verify --allow-empty -m "$repo $branch initialized without tracing"$'\n\n'"$log"
  fi

  # 设置环境变量（使用绝对路径）
  if [ -n "$env_name" ]; then
    local abs_path="$(readlink -f "$dir")"
    addenv "$env_name" "$abs_path"
  fi
}

# 主逻辑
case $1 in
  nemu)
    init NJU-ProjectN/nemu ics2024 nemu true NEMU_HOME
    ;;
  abstract-machine)
    init NJU-ProjectN/abstract-machine ics2024 abstract-machine true AM_HOME
    init NJU-ProjectN/fceux-am ics2021 fceux-am false
    ;;
  am-kernels)
    init NJU-ProjectN/am-kernels ics2021 am-kernels false
    ;;
  nanos-lite)
    init NJU-ProjectN/nanos-lite ics2021 nanos-lite true
    ;;
  navy-apps)
    init NJU-ProjectN/navy-apps ics2024 navy-apps true NAVY_HOME
    ;;
  *)
    echo "❌ Invalid input. Usage: $0 {nemu|abstract-machine|am-kernels|nanos-lite|navy-apps}"
    exit 1
    ;;
esac

echo "💡 Don't forget to run: source ~/.bashrc"
