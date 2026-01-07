import os
import sys

def merge_code_to_markdown(source_folder):
    """
    读取指定路径，合并代码，并自动在根目录下生成 markdown 文件
    构建文件（CMakeLists.txt/.pro等）优先写入到「项目特征总结」下方，源文件上方
    """

    # ================= 配置区域 =================
    # 1. 想要排除的文件后缀
    IGNORE_EXTS = {
        '.png', '.jpg', '.jpeg', '.gif', '.exe', '.dll', '.so', '.o',
        '.user', '.lock', '.zip', '.tar', '.gz', '.pdf',
        '.class', '.pyc'
    }

    # 2. 想要排除的文件夹
    IGNORE_DIRS = {
        '.git', '.idea', '.vscode', '__pycache__', 'build',
        'dist', 'node_modules', 'bin', 'obj', 'cmake-build-debug'
    }

    # 3. 想要排除的特定文件名（移除CMakeLists.txt排除）
    EXCLUDE_FILES = set()

    # 4. 构建文件规则（需要优先写入的构建类文件）
    BUILD_FILES_PATTERN = {
        # 文件名完全匹配
        'CMakeLists.txt', 'Makefile', 'makefile', 'GNUmakefile',
        # 后缀匹配
        '.pro', '.cmake', '.pri', '.prf', '.qmake.conf'  # Qt/qmake相关构建文件
    }
    # ===========================================

    # 1. 路径预处理
    source_folder = source_folder.strip().strip("'").strip('"')
    source_folder = os.path.abspath(source_folder)

    if not os.path.exists(source_folder):
        print(f"\n❌ 错误：找不到路径 -> {source_folder}")
        return

    # 2. 自动生成输出文件名
    project_name = os.path.basename(source_folder)
    output_filename = f"{project_name}_knowledge_base.md"
    EXCLUDE_FILES.add(output_filename)  # 防止自读取
    output_file_path = os.path.join(source_folder, output_filename)

    print(f"\n📂 正在扫描项目：{project_name}")
    print(f"📝 准备生成文件：{output_file_path}")
    print("-" * 30)

    # ========== 第一步：收集所有构建文件 ==========
    build_files = []  # 存储构建文件的(相对路径, 绝对路径, 是否是后缀匹配)
    for root, dirs, files in os.walk(source_folder):
        dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
        for file in files:
            if file in EXCLUDE_FILES:
                continue

            file_path = os.path.join(root, file)
            if os.path.abspath(file_path) == output_file_path:
                continue

            # 匹配构建文件规则
            file_name = os.path.basename(file)
            file_ext = os.path.splitext(file)[1].lower()
            is_build_file = False

            # 匹配完整文件名（如CMakeLists.txt/Makefile）
            if file_name in BUILD_FILES_PATTERN:
                is_build_file = True
            # 匹配后缀（如.pro/.cmake）
            elif file_ext in BUILD_FILES_PATTERN:
                is_build_file = True

            if is_build_file:
                relative_path = os.path.relpath(file_path, source_folder)
                build_files.append((relative_path, file_path))

    # ========== 第二步：写入文件 ==========
    try:
        with open(output_file_path, 'w', encoding='utf-8') as outfile:
            # 1. 写入项目标题
            outfile.write(f"# 项目: {project_name}\n\n")

            # 2. 写入项目特征总结占位
            outfile.write("## 项目特征总结\n")
            outfile.write("\n" * 5)
            outfile.write("---\n\n")

            # 3. 优先写入构建文件（核心需求：放在特征总结后，源文件前）
            if build_files:
                outfile.write("## 构建配置文件\n")
                outfile.write("---\n\n")
                for rel_path, abs_path in build_files:
                    try:
                        with open(abs_path, 'r', encoding='utf-8') as infile:
                            content = infile.read()

                        # 确定构建文件的语法高亮标记
                        file_name = os.path.basename(abs_path)
                        file_ext = os.path.splitext(abs_path)[1].lower()
                        if file_name == 'CMakeLists.txt' or file_ext == '.cmake':
                            lang_tag = 'cmake'
                        elif file_ext == '.pro' or file_ext == '.pri' or file_ext == '.prf':
                            lang_tag = 'qmake'  # qmake构建文件
                        elif 'makefile' in file_name.lower():
                            lang_tag = 'makefile'
                        else:
                            lang_tag = 'plaintext'

                        # 写入构建文件内容
                        outfile.write(f"### File: {rel_path}\n")
                        outfile.write(f"```{lang_tag}\n")
                        outfile.write(content)
                        outfile.write(f"\n```\n\n")
                        outfile.write("---\n\n")
                        print(f"✅ 已合并构建文件: {rel_path}")

                    except UnicodeDecodeError:
                        print(f"⚠️  跳过二进制/非UTF-8构建文件: {rel_path}")
                    except Exception as e:
                        print(f"❌ 读取构建文件错误 {rel_path}: {e}")

            # 4. 写入普通源文件
            outfile.write("## 项目源文件\n")
            outfile.write("---\n\n")
            file_count = 0

            for root, dirs, files in os.walk(source_folder):
                dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]
                for file in files:
                    # 跳过排除文件、构建文件（避免重复）、输出文件
                    if file in EXCLUDE_FILES:
                        continue

                    file_path = os.path.join(root, file)
                    if os.path.abspath(file_path) == output_file_path:
                        continue

                    # 跳过已处理的构建文件
                    rel_path_check = os.path.relpath(file_path, source_folder)
                    if any(rel_path_check == bf[0] for bf in build_files):
                        continue

                    file_ext = os.path.splitext(file)[1].lower()
                    # 后缀排除检查
                    if file_ext in IGNORE_EXTS:
                        continue

                    # 允许的源文件后缀白名单
                    ALLOW_EXTS = {
                        '.cpp', '.h', '.c', '.hpp', '.cppm', '.cc', '.cxx',
                        '.py', '.java', '.js', '.ts', '.html', '.css',
                        '.md', '.txt', '.json', '.toml', '.dat', '.xml', '.sql', '.sh'
                    }
                    if file_ext not in ALLOW_EXTS:
                        continue

                    relative_path = os.path.relpath(file_path, source_folder)
                    try:
                        with open(file_path, 'r', encoding='utf-8') as infile:
                            content = infile.read()

                        # 确定代码块语言标记
                        lang_tag = file_ext.replace('.', '')
                        if lang_tag in ['h', 'hpp']:
                            lang_tag = 'cpp'

                        # 写入源文件内容
                        outfile.write(f"### File: {relative_path}\n")
                        outfile.write(f"```{lang_tag}\n")
                        outfile.write(content)
                        outfile.write(f"\n```\n\n")
                        outfile.write("---\n\n")

                        print(f"✅ 已合并源文件: {relative_path}")
                        file_count += 1

                    except UnicodeDecodeError:
                        print(f"⚠️  跳过二进制或非UTF-8源文件: {relative_path}")
                    except Exception as e:
                        print(f"❌ 读取源文件错误 {relative_path}: {e}")

            # 输出统计信息
            print("-" * 30)
            print(f"🎉 处理完成！")
            print(f"📦 构建文件数量: {len(build_files)}")
            print(f"📄 源文件数量: {file_count}")
            print(f"💾 文件位置: {output_file_path}")

    except PermissionError:
        print("❌ 错误：没有权限写入文件，请检查目录权限。")
    except Exception as e:
        print(f"❌ 发生未知错误: {e}")

if __name__ == "__main__":
    try:
        print("\n=== 代码转 Markdown 知识库工具 ===")
        input_path = input("请输入项目根目录路径 (可直接拖入文件夹): ").strip()
        if input_path:
            merge_code_to_markdown(input_path)
        else:
            print("未输入路径，程序退出。")
    except KeyboardInterrupt:
        print("\n\n程序已手动取消。")
