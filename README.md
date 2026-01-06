# 选课管理系统

README初始内容

# 快速加入开发

## Git 日常开发速查命令集（占位符版）

##  SSH 配置与管理

### **1. SSH 密钥生成与管理**
```bash
# 生成 SSH 密钥（如果还没有）
ssh-keygen -t ed25519 -C "你的邮箱"
# 或使用 RSA
ssh-keygen -t rsa -b 4096 -C "你的邮箱"

# 查看公钥
cat ~/.ssh/id_ed25519.pub

# 复制公钥（macOS）
pbcopy < ~/.ssh/id_ed25519.pub
# Linux
cat ~/.ssh/id_ed25519.pub | xclip -selection clipboard
# Windows (Git Bash)
cat ~/.ssh/id_ed25519.pub | clip
```

### **2. SSH 连接测试**
```bash
# 测试 GitHub 连接
ssh -T git@github.com

# 测试 Gitee 连接
ssh -T git@gitee.com

# 测试 GitLab 连接
ssh -T git@gitlab.com
```

##  远程仓库操作

### **3. 克隆仓库**
```bash
# 克隆仓库（默认分支）
git clone <仓库URL>

# 克隆特定分支
git clone -b <分支名> <仓库URL>

# 克隆 SSH 方式
git clone git@github.com:<用户名>/<仓库名>.git

# 克隆并指定本地目录名
git clone <仓库URL> <本地目录名>
```

### **4. 拉取远程分支到本地**
```bash
# 查看所有远程分支
git branch -r

# 拉取远程分支并创建本地分支
git switch -c <本地分支名> origin/<远程分支名>
# 或经典方式
git checkout -b <本地分支名> origin/<远程分支名>

# 拉取所有远程分支信息
git fetch --all

# 更新指定远程分支
git fetch origin <远程分支名>
```

##  分支操作

### **5. 分支查看与切换**
```bash
# 查看本地分支
git branch

# 查看所有分支（包括远程）
git branch -a

# 查看分支详细信息（跟踪关系）
git branch -vv

# 切换到已有分支
git switch <分支名>
# 或经典方式
git checkout <分支名>

# 切换回上一个分支
git switch -
# 或
git checkout -
```

### **6. 创建与删除分支**
```bash
# 创建新分支（不切换）
git branch <新分支名>

# 创建并切换到新分支
git switch -c <新分支名>
# 或
git checkout -b <新分支名>

# 从特定提交创建分支
git switch -c <新分支名> <提交哈希>

# 删除本地分支（已合并）
git branch -d <分支名>
# 强制删除本地分支（未合并）
git branch -D <分支名>

# 删除远程分支
git push origin --delete <分支名>
# 或
git push origin :<分支名>
```

##  状态与提交

### **7. 查看状态与差异**
```bash
# 查看工作区状态
git status

# 简略状态（紧凑格式）
git status -s

# 查看未暂存的修改
git diff

# 查看已暂存的修改
git diff --staged
# 或
git diff --cached

# 查看特定文件的修改
git diff <文件名>
```

### **8. 提交代码**
```bash
# 添加所有修改到暂存区
git add .

# 添加特定文件
git add <文件1> <文件2>

# 添加目录
git add <目录名>/

# 交互式添加（选择要添加的部分）
git add -p

# 提交暂存区的修改
git commit -m "提交说明"

# 提交并添加详细描述
git commit -m "标题" -m "详细描述..."

# 直接提交所有修改（跳过暂存区，慎用）
git commit -am "提交说明"

# 修改上次提交（未推送时）
git commit --amend -m "新的提交信息"
```

##  推送与拉取

### **9. 推送到远程仓库**
```bash
# 推送到远程并设置上游分支（第一次推送）
git push -u origin <分支名>

# 推送当前分支（已设置上游）
git push

# 强制推送（慎用）
git push -f origin <分支名>
# 更安全的强制推送
git push --force-with-lease origin <分支名>

# 推送所有分支
git push --all origin

# 推送标签
git push origin <标签名>
# 推送所有标签
git push origin --tags
```

### **10. 拉取与合并**
```bash
# 拉取并合并（git fetch + git merge）
git pull origin <分支名>

# 拉取并变基（保持历史线性）
git pull --rebase origin <分支名>

# 仅拉取不合并
git fetch origin

# 拉取特定分支
git fetch origin <分支名>

# 拉取所有远程分支
git fetch --all

# 拉取并允许不相关历史合并
git pull origin <分支名> --allow-unrelated-histories
```

##  标签操作

### **11. 标签管理**
```bash
# 查看所有标签
git tag

# 查看标签详细信息
git show <标签名>

# 创建轻量标签
git tag <标签名>

# 创建附注标签（推荐）
git tag -a <标签名> -m "标签说明"

# 在特定提交上打标签
git tag -a <标签名> <提交哈希> -m "标签说明"

# 删除本地标签
git tag -d <标签名>

# 删除远程标签
git push origin --delete <标签名>
# 或
git push origin :refs/tags/<标签名>

# 把本地的v1.0标签推送到远程 origin 仓库
git push origin <标签名>

# 批量推送所有标签（防遗漏）
git push origin --tags

# 查看远程标签列表
git ls-remote --tags origin
```

##  历史与日志

### **12. 查看历史**
```bash
# 单行显示提交历史
git log --oneline

# 显示最近 N 条提交
git log -<N>

# 图形化显示分支历史
git log --graph --oneline --all

# 显示完整历史
git log

# 显示特定作者的提交
git log --author="<作者名>"

# 显示包含特定关键词的提交
git log --grep="<关键词>"

# 显示某时间段内的提交
git log --since="<开始日期>" --until="<结束日期>"

# 显示文件修改历史
git log --follow <文件名>
```

### **13. 分支图可视化**
```bash
# 基础分支图
git log --oneline --graph --all

# 详细分支图
git log --oneline --graph --all --decorate

# 最近 N 条提交的图形化显示
git log --oneline --graph --all -<N>

# 彩色分支图（更清晰）
git log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit
```

##  日常开发工作流

### **14. 开发新功能**
```bash
# 1. 切换到开发分支并更新
git switch <开发分支>
git pull origin <开发分支>

# 2. 创建功能分支
git switch -c feature/<功能名>

# 3. 开发代码...
git add .
git commit -m "feat: 添加<功能名>"

# 4. 推送功能分支
git push -u origin feature/<功能名>

# 5. 在代码平台创建 Pull Request/Merge Request
```

### **15. 修复 bug**
```bash
# 1. 从稳定分支创建修复分支
git switch <稳定分支>
git switch -c hotfix/<bug描述>

# 2. 修复代码...
git add .
git commit -m "fix: 修复<bug描述>"

# 3. 推送到远程
git push -u origin hotfix/<bug描述>

# 4. 创建 PR/MR 或直接合并
```

### **16. 发布新版本**
```bash
# 1. 切换到发布分支
git switch <发布分支>
git pull origin <发布分支>

# 2. 从开发分支合并最新代码
git merge <开发分支> --no-ff -m "准备发布 <版本号>"

# 3. 打标签
git tag -a <版本号> -m "Release <版本号>"

# 4. 推送分支和标签
git push origin <发布分支>
git push origin <版本号>

# 5. 在代码平台创建正式 Release
```

##  撤销与恢复

### **17. 撤销操作**
```bash
# 撤销暂存（取消 add）
git restore --staged <文件名>
# 或经典方式
git reset HEAD <文件名>

# 丢弃工作区修改（危险，会丢失未提交内容）
git restore <文件名>
# 或经典方式
git checkout -- <文件名>

# 撤销上次提交（保留修改到工作区）
git reset --soft HEAD~1

# 撤销上次提交（丢弃修改）
git reset --hard HEAD~1

# 撤销到指定提交
git reset --hard <提交哈希>

# 安全撤销已推送的提交（生成反向提交）
git revert <提交哈希>
```

### **18. 恢复丢失的提交**
```bash
# 查看操作历史（包括已删除的提交）
git reflog

# 从 reflog 恢复分支
git switch -c <恢复分支名> <reflog哈希>

# 或直接重置
git reset --hard <reflog哈希>
```

##  日常开发速查表

### **开发流程**
```bash
# 开始新功能
git switch <主分支> && git pull && git switch -c feature/<功能名>

# 日常提交
git add . && git commit -m "feat: <功能>" && git push

# 拉取更新
git switch <当前分支> && git pull

# 查看状态
git status

# 查看历史
git log --oneline --graph --all -10
```

### **分支管理**
```bash
# 创建分支
git switch -c <分支名>

# 切换分支
git switch <分支名>

# 合并分支
git merge <源分支>

# 删除分支
git branch -d <分支名>
```

### **远程协作**
```bash
# 查看远程
git remote -v

# 添加远程
git remote add <远程名> <远程URL>

# 拉取代码
git pull <远程名> <分支名>

# 推送代码
git push <远程名> <分支名>
```

## ⚠️ 重要提醒

### **危险操作**
```bash
# 慎用！会丢失未提交的修改
git reset --hard

# 慎用！会覆盖远程历史
git push -f

# 慎用！会删除未跟踪的文件
git clean -fd

# 操作前先备份
git branch backup-<操作描述>
```

### **最佳实践**
1. **小步提交**：每个提交完成一个小功能
2. **清晰信息**：使用约定式提交格式（feat:, fix:, docs:, style:, refactor:, test:, chore:）
3. **先拉后推**：推送前先拉取最新代码避免冲突
4. **分支策略**：功能分支开发，主分支保护
5. **标签管理**：重要版本打标签标记

### **Git 别名配置（可选）**
```bash
# 添加到 ~/.gitconfig
[alias]
    co = checkout
    br = branch
    ci = commit
    st = status
    lg = log --color --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit
    pl = pull --rebase
    ps = push
    undo = reset HEAD~1
    wip = !git add -A && git commit -m "WIP"
```

## 🎯 如何使用此速查表

1. **替换占位符**：使用时将 `<占位符>` 替换为实际值
   - `<仓库URL>` → `https://github.com/username/repo.git`
   - `<分支名>` → `main`、`dev`、`feature/login`
   - `<文件名>` → `src/main.cpp`、`README.md`

2. **组合命令**：根据实际工作流组合使用

3. **实践练习**：在测试仓库中练习常用命令

4. **自定义修改**：根据团队规范调整命令格式

记住：**Git 是工具，熟练使用需要实践**。遇到问题时，`git status` 和 `git log --oneline --graph --all` 通常能帮你理清当前状态。
