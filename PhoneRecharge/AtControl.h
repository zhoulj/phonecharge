/*******************************************************************
| Copyright (c) 2013,长春威致科技有限公司                          |
| All rights reserved.                                             |
|------------------------------------------------------------------|
| 文件名称：AtControl.h                                            |
| 文件标识：                                                       |
| 摘要：    本文件定义了类AtControl及它的一些成员                  |
| 其它说明：初始化Com口，完成拨号、挂断、发送按键等功能            |
| 当前版本：                                                       |
| 作    者：                                                       |
| 完成日期：                                                       |
|----------------------------------------------------------------- |
| 修改记录1：                                                      |
| 修改日期：                                                       |
| 版 本 号：                                                       |
| 修 改 人：                                                       |
| 修改原因：                                                       |
| 修改内容：                                                       |
| 修改代码位置：                                                   |
|------------------------------------------------------------------|
| 修改记录2：…                                                    |
|******************************************************************/
#pragma once

#include "com.h"    // 引入串口通讯工具类

class CAtControl
{
public:
  /*******************************************************************
  | 函数名称：Com口初始化                                            |
  | 函数功能：初始化Com口，及其相关参数                              |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  0成功，其他失败                                        |
  | 说明：无                                                         |
  |******************************************************************/
	int AtInit();
  /*******************************************************************
  | 函数名称：拨号                                                   |
  | 函数功能：通过Com口At指令拨打充值服务电话                        |
  | 输入参数：ServiceNum服务充值号                                   |
  | 输出参数：无                                                     |
  | 返回值：  0成功，其他失败                                        |
  | 说明：无                                                         |
  |******************************************************************/
	int AtDial(char* ServiceNum);//拨充值服务号号
  /*******************************************************************
  | 函数名称：挂断                                                   |
  | 函数功能：通过Com口At指令挂断电话                                |
  | 输入参数：无                                                     |
  | 输出参数：无                                                     |
  | 返回值：  0成功，其他失败                                        |
  | 说明：无                                                         |
  |******************************************************************/
	int AtHangup();
  /*******************************************************************
  | 函数名称：发送按键值                                             |
  | 函数功能：通过At指令向Com口发送按键值                            |
  | 输入参数：KeyingValue，发送按键值，可以是多个键值如电话号        |
  | 输出参数：无                                                     |
  | 返回值：  0成功，其他失败                                        |
  | 说明：无                                                         |
  |******************************************************************/
	int SendKeying(char* KeyingValue);//
	
	CAtControl(void);
	~CAtControl(void);
private:
  Ccom m_Comm;
  HANDLE m_hRecvEvent;
};
