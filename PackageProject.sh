#!/bin/bash

# 可用参数
# --noclear：打包后不清理打包目标文件夹。默认清理，为的是顺便清理Config这类动态生成的文件，不让这类文件影响项目。
# --debug：以Debug设置打包项目。**目前在二进制引擎上不可用。**
# --Shipping：以Shipping配置打包项目。
# --Development：以Development配置打包项目。
# --Test：以Test配置打包项目。**目前在二进制引擎上不可用。**

# 标记用变量
export ClearArchived=true

# 后续使用的变量。
export ArchivedDirectory=$(PWD)/Archived
export BuildConfig=DebugGame

for Arg in $*
do
	if [ "${Arg,,}" == "--noclear" ]
	then
		unset ClearArchived
	fi

	if [ "${Arg,,}" == "--debug" ] || [ "${Arg,,}" == "--shipping" ] || [ "${Arg,,}" == "--development" ] || [ "${Arg,,}" == "--test" ]
	then
		BuildConfig=${Arg#--}
	fi
done

# 清理打包文件夹。
if [ -n $ClearArchived ] && [ -d "$ArchivedDirectory" ]
then
	echo "Deleting archived directory..."
	rm -r $ArchivedDirectory
fi

if [ $? -ne 0 ]
then
	echo "Deleting archived directory failed, Aborting package..."
	exit
fi

# 打包。
$UE4UAT BuildCookRun -Project=$(PWD)/AreYouReady.uproject -Clientconfig=${BuildConfig} -TargetPlatform=Win64 -Build -Pak -Cook -Archive -Stage -Archivedirectory=$ArchivedDirectory -UTF8Output
