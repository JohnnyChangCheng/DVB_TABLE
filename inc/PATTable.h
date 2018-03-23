/* 
 * File name : Programs
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#ifndef __PATTABLE_H__
#define __PATTABLE_H__
#include "DvbtTable.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <functional>

namespace Noovo {
	class PATTable : public DvbtTable
	{
	public:
		PATTable();
		virtual ~PATTable();
		virtual bool IsReadable();
		virtual void Parse( uint8_t *metadata);
		std::unordered_map<unsigned int,unsigned int> pid_programid;
		void PushData(unsigned int pid,unsigned int programid);
	private:
		dvbpsi_t * _pat_handler;
		void _setHandler();
		void _pushCallBack(uint8_t *metadata );
		bool _readable=false;
		

	};
};
#endif
