#!/bin/bash

# 标记用变量
export CleanArchived=true

# 后续使用的变量。
export ArchivedDirectory=$(PWD)/Archived
export BuildConfig=DebugGame
export ProjectName=AreYouReady
export NoEditor=-NoCompileEditor

for Arg in $*
do
	if [ "${Arg,,}" == "--buildeditor" ]
	then
		unset NoEditor
		continue
	fi

	if [ "${Arg,,}" == "--rebuild" ]
	then
		export CleanBuild="-Clean"
		continue
	fi

	if [ "${Arg,,}" == "--noclear" ]
	then
		unset CleanArchived
		continue
	fi

	if [ "${Arg,,}" == "--debug" ] || [ "${Arg,,}" == "--shipping" ] || [ "${Arg,,}" == "--development" ] || [ "${Arg,,}" == "--test" ]
	then
		BuildConfig=${Arg#--}
		# 如果为Shipping配置选项则需要去掉调试信息。
		if [ "${Arg,,}" == "--shipping" ]
		then
			export NoDebugInfo="-NoDebugInfo"
		fi
		continue
	fi

	if [ "${Arg,,}" == "--help" ]
	then
		echo "可用参数："
		echo "--NoClean：打包后不清理打包目标文件夹。默认清理，为的是顺便清理Config这类动态生成的文件，不让这类文件影响项目。"
		echo "--Debug：以Debug设置打包项目。**目前在二进制引擎上不可用。**"
		echo "--Shipping：以Shipping配置打包项目。"
		echo "--Development：以Development配置打包项目。"
		echo "--Test：以Test配置打包项目。**目前在二进制引擎上不可用。**"
		echo "--Rebuild：完全重新构建项目。"
		echo "--BuildEditor：构建项目时候也会构建编辑器内容，例如编辑器插件，默认不构建编辑器内容。"
		exit
	fi

	echo -e "\e[91mUnrecognized commond \"${Arg,,}\", Package aborting..."
	exit
done

# 清理打包文件夹。
if [ -n $CleanArchived ]
then
	# 如果构建配置为Shipping，则还需要清理电脑上的配置文件夹。
	if [ ${BuildConfig} == "shipping" ] && [ -d "$LOCALAPPDATA/${ProjectName}" ]
	then
		echo -e "\e[92mDeleting shipping config directory: ${LOCALAPPDATA}/${ProjectName}"
		rm -r ${LOCALAPPDATA}/${ProjectName}
	fi

	# 如果上一个打包项目还未删除，此处执行删除逻辑。
	if [ -d "${ArchivedDirectory}" ]
	then
		echo -e "\e[92mDeleting archived directory: ${ArchivedDirectory}"
		rm -r $ArchivedDirectory
	fi
fi

if [ $? -ne 0 ]
then
	echo -e "\e[91mDeleting archived directory failed, Aborting package..."
	exit
fi

# 打包。
export Pkg="$UE4UAT BuildCookRun -Project=$(PWD)/${ProjectName}.uproject -Clientconfig=${BuildConfig} -TargetPlatform=Win64 -Build -Pak -Cook $NoDebugInfo -Stage -Archive -Archivedirectory=$ArchivedDirectory -UTF8Output $CleanBuild $NoEditor"
echo -e "\e[92m$Pkg"
$Pkg
