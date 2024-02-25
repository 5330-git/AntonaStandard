#! /bin/bash

# 首先移除build1 目录下的所有gcda文件
rm -f $(find ../build1 -name *.gcda) 

# 重新执行所有的可执行文件
for file in ../bin/Linux/Test*; do
    # 检查文件是否是可执行程序
    if [[ -x "$file" ]]; then
        # 执行可执行程序
        "$file"
    fi
done

# 在build1目录下搜索gcda文件
lcov -c -d ../build1 -o test.info --rc lcov_branch_coverage=1

# 生成html报告
genhtml --branch-coverage -o result test.info

