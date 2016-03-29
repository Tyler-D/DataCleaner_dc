#!/bin/bash

if [ $# -eq 0 ]; then # 通过$#取得脚本参数数目
    echo "Usage:rm_deprecated [image_path]"
     exit 1
fi

find $1 -name "deprecated" | xargs rm -r
