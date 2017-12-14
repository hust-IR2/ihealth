/********************************************************************
	Created:	2013.10.11  14:03
	Author:		hps
	
	Purpose:	做为 CSM 子模块时需要进行一些必须的初始化
	Remark:		
*********************************************************************/

#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

#include "RPC.h"
#include "RPCClient.h"
#include "RPCServer.h"
#include "RPCResponse.h"
#include "ExtCPP.h"
using namespace Ext::CPP;

namespace Ext
{
	/// <summary>
	/// 表示 CSM 架构下的一个子模块或子服务
	/// </summary>
	class DCICOMMONEXT_MODULE_EXPIMP CSMModule
	{
	private:
		static Panic getRootServiceAddr(char *ip, u_short *port);
		static void initDefaultRPC();

	public:
		CString Name;
		RPCClient *Client;
		RPCServer *Server;

		static CSMModule *DefaultModule;

		CSMModule();
		CSMModule(RPCClient *client, RPCServer *server);
		~CSMModule();

		// 一般情况下使用 DefaultModule 调用即可
		static Panic SetName(const CString &moduleName);
		static Panic MakesureCSMReady();
		static Panic SetupRPC();
		static Panic TeardownRPC();
		static Panic Release();

		// 实体方法调用，一个程序同时做为 CSM 的多个子模块情况下使用。Ins means Instance。
		Panic InsSetName(const CString &moduleName);
		Panic InsMakesureCSMReady();
		Panic InsSetupRPC();
		Panic InsTeardownRPC();
		Panic InsRelease();
	};
}