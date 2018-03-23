/* 
 * File name : Media
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#ifndef __PMTTABLE_H__
#define __PMTTABLE_H__
#include <unordered_map>
#include <string>
#include <iostream>
#include <functional>
#include "DvbtTable.h"
namespace Noovo {
	class StreamInfo {
	public:
		StreamInfo(int pid_, std::string descriptor_): pid(pid_), descriptor(descriptor_) {
		
		}
		int pid;
		std::string descriptor;
	};

	class PMTTable : public  DvbtTable
	{
	public:
		PMTTable();
		PMTTable(int index);
		virtual ~PMTTable();
		virtual bool IsReadable();
		virtual void Parse( uint8_t *metadata);
		std::vector<StreamInfo> stream_info;
		void PushData(unsigned int pid,std::string info);
	private:
		void _setHandler();
		void _pushCallBack(uint8_t *metadata );
		int _pmt_programid;
		dvbpsi_t * _pmt_handler;


	};
};
#endif
