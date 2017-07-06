# 源码自动生成模板 java-demo

### 概述

* 模板: java-demo
* 模板使用时间: 2017-07-06 09:31:22

### Docker
* Image: registry.cn-beijing.aliyuncs.com/codebase/java-demo
* Tag: 1.2
* SHA256: 398945ee3e2137546a20f964eebe08b05f0723c6d79be49ea33ac46548f54bb1

### 用户输入参数
* repoUrl: "git@code.aliyun.com:3tee/win-demo-avei.git" 
* appName: "win-demo-avei" 
* operator: "aliyun_040939" 

### 上下文参数
* appName: win-demo-avei
* operator: aliyun_040939
* gitUrl: git@code.aliyun.com:3tee/win-demo-avei.git
* branch: master


### 命令行
	sudo docker run --rm -v `pwd`:/workspace -e repoUrl="git@code.aliyun.com:3tee/win-demo-avei.git" -e appName="win-demo-avei" -e operator="aliyun_040939"  registry.cn-beijing.aliyuncs.com/codebase/java-demo:1.2

