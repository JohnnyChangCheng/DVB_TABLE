#ifndef __DEMUXER_H__
#define __DEMUXER_H__
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include "PATTable.h"
#include "SDTTable.h"
#include "PMTTable.h"
#include "DvbtTable.h"


#define GET_PID(buf) ((((buf)[1] & 0x1f) << 8) + (buf)[2])
namespace Noovo {

	class ChannelDetail{
	public:
		int program_id = 0;
		std::vector<StreamInfo> stream_info;
		std::vector<std::string> service;
		
	};
	class Demuxer 
	{
	public:
		
		Demuxer();
		virtual ~Demuxer();
		virtual void DoTask();
		void SetMetadata(uint8_t* metadata);
		int HandlePacket();
		void PATInit();
		void PMTInit(int pmt_index,int pid);
		void SDTInit();
		std::vector<int>& ReturnArrayPID();
	private:
		std::vector<int> _pids;
		std::unordered_map<unsigned int,Noovo::ChannelDetail> Channel_Data;
		int num_program=0;
		int count=0;
		uint8_t *_metadata;
		std::unordered_map<unsigned int, std::shared_ptr<Noovo::PMTTable> > _pmt;
		std::shared_ptr<Noovo::PATTable> _pat;
		bool _patready=false;
		bool _sdtready=false;
		std::unordered_map<unsigned int,bool> filter;
		std::shared_ptr<Noovo::SDTTable> _sdt;
		void _init_tables();
		void _dumpPMTTable();
		void _dumpSDTTable();
		void _dumpPATTable();
		
	};
};
#endif
