
#pragma once

#ifdef DCICOMMONEXT_MODULE
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllexport)
#else
#define DCICOMMONEXT_MODULE_EXPIMP __declspec(dllimport)
#endif

#include "Event.h"

namespace Ext
{
	namespace CPP
	{
		class DCICOMMONEXT_MODULE_EXPIMP Defer
		{
		public:
			Event Do;
			void *&Data;

			Defer(void *&data, EventHandle& handle) : Data(data)
			{
				Do += handle;
			}

			~Defer()
			{
				EventArg arg(this, Data);
				this->Do.Raise(&arg);
			}
		};

		#define DEFER(p, e) \
			Ext::CPP::Defer defer##p((void*&)p, EventHandle(&e));

		#define DEFERX(p, c, e) \
			Ext::CPP::Defer defer##p((void*&)p, EventHandle(c, &e));
	} // End namespace CPP
} // End namespace Ext