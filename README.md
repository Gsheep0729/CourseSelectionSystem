# 选课管理系统

README初始内容

# for_md.py 使用教程

`for_md.py` 是一款由GY设计编写轻量高效的代码归档工具，可将指定目录下的构建配置文件、各类源文件批量整合为结构化的 Markdown 文档，便捷实现代码归档、查阅与分享。

## 功能特点

1. **智能过滤无关内容**：自动排除图片、二进制文件（.exe/.dll/.so等）、编译产物（build/dist/obj等）、编辑器配置目录（.git/.idea/.vscode等），仅保留核心代码与构建文件；
2. **构建文件优先整理**：CMakeLists.txt、Makefile、.pro/.cmake（Qt/CMake）等构建配置文件，会优先整合到「项目特征总结」后方，便于快速掌握项目构建规则；
3. **多语言语法高亮**：适配 C/C++、Python、Java、JavaScript/TypeScript、HTML/CSS、SQL、CMake、QMake、Makefile 等主流格式，Markdown 代码块自动匹配对应语法高亮；
4. **结构化排版**：通过多级标题区分「项目信息」「构建配置文件」「项目源文件」，并预留「项目特征总结」编辑位，文档层次清晰；
5. **路径自适应兼容**：支持直接拖入文件夹生成路径，自动处理路径格式、转换为绝对路径，兼容 Windows/macOS/Linux 系统路径规范。

## 使用步骤

### 前置条件

确保本地已安装 Python 环境（Python 3.x 任意版本均可）。

### 操作步骤

1. **运行脚本**：在脚本所在目录打开终端/命令提示符，执行命令：
   ```bash
   python for_md.py
   ```
2. **输入目标路径**：根据终端提示，输入待归档的项目根目录路径（支持直接拖入文件夹自动生成路径），示例：
   ```
   /run/media/root/铠侠D/桌面文件/开源技术群资料/考试/CourseSelectionSystem/CourseSelectionSystem/src/CourseSelectionSystem/
   ```
3. **自动生成文档**：脚本会自动扫描目录、过滤无关文件、整合代码内容，完成后在指定项目目录下生成 `<项目名>_knowledge_base.md` 文件（例：`CourseSelectionSystem_knowledge_base.md`）。

## 注意事项
1. 路径输入支持绝对路径，若路径包含空格/特殊字符，无需手动添加引号，脚本会自动处理；
2. 生成的 Markdown 文件会被自动排除在扫描范围外，避免循环读取；
3. 若提示「权限错误」，请检查目标目录的写入权限（如 Windows 需以管理员身份运行终端）；
4. 非 UTF-8 编码的文件会被跳过（终端会提示），避免乱码或读取失败；
5. 可通过修改脚本内 `IGNORE_EXTS`/`IGNORE_DIRS` 等配置项，自定义过滤规则。

# 快速加入开发

## Git 日常开发速查命令集（占位符版）

### 如何使用此速查表

1. **替换占位符**：使用时将 `<占位符>` 替换为实际值
   - `<仓库URL>` → `https://github.com/username/repo.git`
   - `<分支名>` → `main`、`dev`、`feature/login`
   - `<文件名>` → `src/main.cpp`、`README.md`

2. **组合命令**：根据实际工作流组合使用

3. **实践练习**：在测试仓库中练习常用命令

4. **自定义修改**：根据团队规范调整命令格式

记住：**Git 是工具，熟练使用需要实践**。遇到问题时，`git status` 和 `git log --oneline --graph --all` 通常能帮你理清当前状态。

##  日常开发速查表

### **开发流程**

```bash
# 开始新功能
git switch <主分支> && git pull && git switch -c feature/<功能名>

# 日常提交
git add . && git commit -m "feat: <功能>" && git push

# 暂存所有已跟踪文件的修改
git add -u

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

### Tag 合并到 Release 分支速查表

#### 核心场景：添加 Tag → 合并到 Release 分支 → 推送远程

##### 1. 前置准备（确保本地代码/Tag 与远程一致）
```bash
# 拉取远程所有分支+Tag（必做，避免本地缺失）
git fetch origin --tags
```

##### 2. 本地添加 Tag（若未打 Tag 先执行）
```bash
# 方式1：附注标签（推荐，带说明）
git tag -a <版本号/标签名> -m "Release: <版本号> 发布说明"
# 示例：git tag -a v1.0.0 -m "Release: v1.0.0 正式版发布"

# 方式2：轻量标签（仅标记，无说明）
git tag <标签名>
```

### 3. 切换到 Release 分支并拉取最新代码
```bash
# 切换到release分支
git switch release
# 或经典写法：git checkout release

# 拉取远程release最新代码
git pull origin release
```

### 4. 合并 Tag 到 Release 分支（二选一）
#### 方式1：完整合并 Tag（推荐，保留版本历史）
```bash
# 合并指定Tag到release分支（--no-ff 保留分支记录）
git merge <标签名> --no-ff -m "合并 tag <标签名> 到 release 分支"
```

#### 方式2：摘取 Tag 对应提交（仅取单个提交）
```bash
# 精准摘取Tag对应的提交到release分支
git cherry-pick <标签名>
```

### 5. 冲突处理（合并/摘取时触发）
```bash
# 1. 手动解决冲突文件后，暂存冲突文件
git add <冲突文件名>

# 2. 继续完成合并/摘取操作
# 合并冲突：
git merge --continue
# 摘取冲突：
git cherry-pick --continue
```

### 6. 推送 Release 分支 + Tag 到远程
```bash
# 推送release分支到远程
git push origin release

# 推送Tag到远程（首次打Tag必做，方便追溯）
git push origin <标签名>

# 批量推送所有未推送的Tag（可选）
git push origin --tags
```

### ⚠️ 速查关键提醒
1. 合并前必须执行 `git fetch origin --tags`，确保本地有最新Tag；
2. 优先用 `git merge <标签名>` 合并完整版本，单个提交用 `git cherry-pick <标签名>`；
3. 禁止直接用 `git push -f` 推送release分支，避免覆盖远程稳定代码；
4. 合并后建议在代码平台（GitLab/GitHub）确认Release分支内容无误。

### **最佳实践**

1. **小步提交**：每个提交完成一个小功能
2. **清晰信息**：使用约定式提交格式（feat:, fix:, docs:, style:, refactor:, test:, chore:）
3. **先拉后推**：推送前先拉取最新代码避免冲突
4. **分支策略**：功能分支开发，主分支保护
5. **标签管理**：重要版本打标签标记

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

### **12. 将特定标签合并到发布分支**
当你需要在 dev 分支为某个版本打了 tag 后，仅将这个 tag 对应的特定版本代码精准合并到 release 发布分支，而不是合并 dev 分支的所有最新提交时，可以使用以下方法：

#### 方式 1：直接合并 tag（推荐，适用于 tag 是 dev 分支线性提交的终点）
```bash
# 1. 确保本地代码最新，拉取远程所有分支和 tag
git fetch origin --tags

# 2. 切换到 release 分支，并拉取最新的 release 代码
git checkout release
git pull origin release

# 3. 合并指定 tag 到 release 分支
git merge <tag名> --no-ff -m "合并 tag <tag名> 到 release 分支"

# 4. 如有冲突，解决后继续
git add <冲突文件>
git merge --continue
```

#### 方式 2：Cherry-pick tag 对应的提交（适用于只取单个提交）
```bash
# 1. 先切换到 release 分支并拉取最新
git checkout release
git pull origin release

# 2. 摘取 tag 对应的提交到 release
git cherry-pick <tag名>

# 3. 若有冲突，解决后执行：
git add <冲突文件>
git cherry-pick --continue
```

#### 后续步骤：推送 release 分支
```bash
# 推送 release 分支到远程
git push origin release

# 若 tag 未推过远程，补充推送 tag（可选，建议推送，方便追溯）
git push origin <tag名>
```

#### 关键要点
1. **核心命令**：合并 tag 优先用 `git merge <tag名>`（保留历史，适合完整版本），单个提交摘取用 `git cherry-pick <tag名>`
2. **前置准备**：合并前务必 `git fetch --tags` 和 `git pull release`，确保本地代码和远程一致
3. **冲突处理**：合并/摘取时的冲突需手动解决，解决后用 `--continue` 完成操作

##  历史与日志

### **13. 查看历史**
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

### **14. 分支图可视化**
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

### **15. 开发新功能**
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

### **16. 修复 bug**
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

### **17. 发布新版本**
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

### **18. 撤销操作**
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

### **19. 恢复丢失的提交**
```bash
# 查看操作历史（包括已删除的提交）
git reflog

# 从 reflog 恢复分支
git switch -c <恢复分支名> <reflog哈希>

# 或直接重置
git reset --hard <reflog哈希>
```
