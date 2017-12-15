#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

#include <map>

// 注册的日志名称，与一个日志文件进行绑定
#define DCI_LOG _T("DCI_LOG")

namespace Ext
{
	namespace File
	{

		/// <summary>
		/// 表示日志类
		/// </summary>
		class DCICOMMONEXT_MODULE_EXPIMP CLog
		{
		public:

			/// <summary>
			/// 使用注册的日志名称初始化一个日志类的实例。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="strRegistLogName">[I] 指定一个已经注册的日志名称。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.10.13   17:31"/>
			CLog(const CString strRegistLogName);

			/// <summary>
			/// 析构函数。
			/// </summary>
			/// <remarks></remarks>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.10.13   17:32"/>
			virtual ~CLog(void);

			/// <summary>
			/// 注册一个日志名称。
			/// </summary>
			/// <remarks>
			/// <br> 1. 一个日志名称和一个文件只能存在一对一的关系。</br>
			/// <br> 2. 如果一个日志名称已经被注册，但指向和原来不同的日志文件时，</br>
			/// <br>    仅当新的日志文件没有被绑定到其他日志名称才会注册成功，</br>
			/// <br>    并且旧的绑定关系被删除。</br>
			/// </remarks>
			/// <param name="strLogName">[I] 指定要进行注册的日志名称。</param>
			/// <param name="strLogFilePath">[I] 指定将要绑定的日志文件。</param>
			/// <returns>若注册成功，则返回 true；否则返回 false。</returns>
			/// <author name="peishengh" date="2011.10.13   17:32"/>
			static bool Regist(CString strLogName, CString strLogFilePath);

			/// <summary>
			/// 注册一个日志名称。
			/// </summary>
			/// <remarks>
			/// <br> 1. 一个日志名称和一个文件只能存在一对一的关系。</br>
			/// <br> 2. 如果一个日志名称已经被注册，但指向和原来不同的日志文件时，</br>
			/// <br>    仅当新的日志文件没有被绑定到其他日志名称才会注册成功，</br>
			/// <br>    并且旧的绑定关系被删除。</br>
			/// </remarks>
			/// <param name="strLogName">[I] 指定要进行注册的日志名称。</param>
			/// <param name="strLogFilePath">[I] 指定将要绑定的日志文件。</param>
			/// <param name="rLog">[O] 返回注册成功的 CLog 实例。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.10.14   12:32"/>
			static bool Regist(CString strLogName, CString strLogFilePath, CLog &rLog);

			/// <summary>
			/// 解除一个已经注册的日志名称。
			/// </summary>
			/// <remarks>即解除日志名称和日志文件的绑定关系。</remarks>
			/// <param name="strLogName">[I] 指定要解除的日志名称。</param>
			/// <returns>若解除成功，则返回 true；否则返回 false。若指定的日志名称未注册，则返回 true。</returns>
			/// <author name="peishengh" date="2011.12.7   16:57"/>
			static bool UnRegist(CString strLogName);

			/// <summary>
			/// 获取一个值，指示指定的日志名称是否已经被注册。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="strLogName">[I] 指定要检测的日志名。</param>
			/// <param name="strLogFilePath">[O] 保存指定的日志名绑定的日志文件。若该日志名还未被注册，则文件名为空。</param>
			/// <returns>若该日志名已被注册，则返回 true；否则返回 false。</returns>
			/// <author name="peishengh" date="2011.10.13   17:34"/>
			static bool IsRegistLogName(const CString &strLogName, CString &strLogFilePath);

			/// <summary>
			/// 获取一个值，指示指定的文件是否已经被绑定到一个日志名。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="strLogFilePath">[I] 指定要检测</param>
			/// <param name="strLogName"></param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.10.13   17:37"/>
			static bool IsRegistLogFile(const CString &strLogFilePath, CString &strLogName);

			/// <summary>
			/// 当前日志注册到指定文件路径。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="strLogFilePath">[I] 指定当前日志要绑定到的文件路径。</param>
			/// <returns>若注册成功，则返回 true；否则返回 false。</returns>
			/// <author name="peishengh" date="2011.10.14   12:26"/>
			bool Regist(const CString &strLogFilePath);

			/// <summary>
			/// <br> 写日志信息。</br>
			/// <br> 若当前日志名称未注册到任何日志文件，则不进行任何操作。</br>
			/// </summary>
			/// <remarks></remarks>
			/// <param name="strMessageFormat">[I] 指定要写进日志文件的信息。</param>
			/// <param name="...">[I] 指定一个或多个格式化参数。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.10.13   17:53"/>
			void Write(const TCHAR *strMessageFormat, ...);
			static void WriteTo(const CString &strLogName, const TCHAR *strMessageFormat, ...);

			/// <summary>
			/// <br> 在新行上写一条日志信息。</br>
			/// <br> 若当前日志名称未注册到任何日志文件，则不进行任何操作。</br>
			/// </summary>
			/// <remarks></remarks>
			/// <param name="strMessageFormat">[I] 指定要写进日志文件的信息。</param>
			/// <param name="...">[I] 指定一个或多个格式化参数。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.10.13   17:54"/>
			void WriteLine(const TCHAR *strMessageFormat, ...);
			static void WriteLineTo(const CString &strLogName, const TCHAR *strMessageFormat, ...);

			/// <summary>
			/// <br> 写日志信息。</br>
			/// <br> 若当前日志名称未注册到任何日志文件，则不进行任何操作。</br>
			/// </summary>
			/// <remarks>此方法会在写日志信息之前，先追加一份时间戳（Time Stamp） [Y.M.D H:M:S]</remarks>
			/// <param name="strMessageFormat">[I] 指定要写进日志文件的信息。</param>
			/// <param name="">[I] 指定一个或多个格式化参数。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.10.13   17:55"/>
			void WriteTS(const TCHAR *strMessageFormat, ...);
			static void WriteTSTo(const CString &strLogName, const TCHAR *strMessageFormat, ...);

			/// <summary>
			/// <br> 在新行上写一条日志信息。</br>
			/// <br> 若当前日志名称未注册到任何日志文件，则不进行任何操作。</br>
			/// </summary>
			/// <remarks>此方法会在写日志信息之前，先追加一份时间戳（Time Stamp） [Y.M.D H:M:S]</remarks>
			/// <param name="strMessageFormat">[I] 指定要写进日志文件的信息。</param>
			/// <param name="">[I] 指定一个或多个格式化参数。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.10.13   17:57"/>
			void WriteLineTS(const TCHAR *strMessageFormat, ...);
			static void WriteLineTSTo(const CString &strLogName, const TCHAR *strMessageFormat, ...);

			/// <summary>
			/// 获取当前日志名称。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回当前日志名称。</returns>
			/// <author name="peishengh" date="2011.10.14   11:39"/>
			CString GetLogName() const;

			/// <summary>
			/// 获取当前日志对应的日志文件。
			/// </summary>
			/// <remarks></remarks>
			/// <returns>返回当前绑定到的日志文件。</returns>
			/// <author name="peishengh" date="2011.10.13   17:58"/>
			CString GetLogFilePath() const;

			/// <summary>
			/// 获取一个值，用于指示当前日志名称是否注册了。
			/// </summary>
			/// <remarks>可调用 CLog::Regist(...) 静态方法注册一个日志名称。</remarks>
			/// <returns>如果当前日志名称已经注册过了，则返回 true；否则返回 false。</returns>
			/// <author name="peishengh" date="2011.10.13   17:59"/>
			bool IsRegistered();

			/// <summary>
			/// 解除当前日志名称的注册文件。
			/// </summary>
			/// <remarks>>即解除日志名称和日志文件的绑定关系。</remarks>
			/// <returns>若解除成功，则返回 true；否则返回 false。若当前日志名称未注册，则返回 true。</returns>
			/// <author name="peishengh" date="2011.12.7   17:05"/>
			bool UnRegist();

		private:

			/// <summary>
			/// 获取写文件的 StdioFile 对象。
			/// </summary>
			/// <remarks>以写的形式打开文件，并将写入点移到文件末尾。</remarks>
			/// <param name="rWritor"></param>
			/// <returns>若成功初始化 StdioFile 对象，则返回 true；否则返回 false。</returns>
			/// <author name="peishengh" date="2011.10.13   17:59"/>
			bool GetWritor(CStdioFile &rWritor);

			/// <summary>
			/// 初始化当前对象。
			/// </summary>
			/// <remarks></remarks>
			/// <param name="bIsRegisted">[I] 显示指定日志名是否被注册了没有。</param>
			/// <param name="strLogName">[I] 指定日志名称。</param>
			/// <param name="strLogFilePath">[I] 指定日志文件路径。</param>
			/// <returns></returns>
			/// <author name="peishengh" date="2011.10.14   12:23"/>
			void Initial(bool bIsRegisted, const CString &strLogName, const CString strLogFilePath);

		private:
			/// <summary>
			/// 日志文件名
			/// </summary>
			CString m_strLogName;

			/// <summary>
			/// 日志文件绑定到的日志文件路径
			/// </summary>
			CString m_strLogFilePath;

			/// <summary>
			/// 指示当前的日志名是否注册了没有
			/// </summary>
			bool m_bIsRegisted;

			/// <summary>
			/// 保存已经注册的日志文件名。形式：<日志文件名, 绑定到的日志文件>
			/// </summary>
			static std::map<CString, CString> sm_logNameMapToFile;
		};

	}// End namespace File
}// End namespace Ext