/* 
 * File name : Programs
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#ifndef __SDTTable_H__
#define __SDTTable_H__
#include "DvbtTable.h"
#include <unordered_map>
#include <memory>
#include <functional>
#include <iostream>
#include <vector>


namespace Noovo {
	class SDTTable : public DvbtTable
	{
	public:
		SDTTable();
		virtual ~SDTTable();
		virtual bool IsReadable();
		virtual void Parse( uint8_t *metadata);
		void PushData(unsigned int programid,uint8_t* descriptor_data);
		std::unordered_map<unsigned int,std::vector<std :: string> > SI_Table;
	private:
		void _setHandler();
		void _pushCallBack(uint8_t *metadata );
		dvbpsi_t * _sdt_handler;
		bool *_ready;
	};
};
#endif
