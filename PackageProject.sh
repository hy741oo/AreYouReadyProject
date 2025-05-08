#!/bin/bash

# 标记用变量
set ClearArchived

# 后续使用的路径。
ArchivedDirectory=$(pwd)/Archived

for Arg in $*
do
	if [[ "$Arg"=="--noclear" ]]
	then
		echo $Arg
		unset ClearArchived
	fi
done

# 清理打包后的文件夹。
if [[ -n "$ClearArchived" && -d "$ArchivedDirectory" ]]
then
	echo "Deleting archived directory..."
	rm -r $ArchivedDirectory
fi

mkdir $ArchivedDirectory

# 打包。
#$UE4UAT BuildCookRun -Project=$(PWD)/AreYouReady.uproject -Clientconfig=DebugGame -TargetPlatform=Win64 -Build -Pak -Cook -Archive -Stage -Archivedirectory=$ArchivedDirectory -UTF8Output
