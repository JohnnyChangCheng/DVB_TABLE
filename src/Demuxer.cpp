#include "Demuxer.h"
using namespace Noovo;

Demuxer::Demuxer()
{

}

Demuxer::~Demuxer()
{
}
void Noovo::Demuxer::SetMetadata(uint8_t *metadata)
{

	if (metadata == NULL) {

		std::cout << "Set the metadata pointer first\n";

		//return -1;

	}

	_metadata = metadata;
}
void Noovo::Demuxer::DoTask()
{
	try {
		HandlePacket();
	}
	catch (char const* msg) {
		LOG(msg);
	}
}

int Noovo::Demuxer::HandlePacket()
{

	if (_metadata == NULL) {

		throw("Set the metadata pointer first\n");

	}

	if ((int)_metadata[0] != 0x47) {

		throw("Not TS format \n");

	}

	int pid = GET_PID(_metadata);

	LOG("PID : " << pid);

	if (pid == 0x0) {


		PATInit();


	}
	else if (_patready && pid == 0x11) {


		SDTInit();

		_sdtready = true;

	}
	else if (_patready) {


		auto check = filter.find(pid);

		if (check != filter.end() && check->second != true)
		{
			check->second = true;

			count++;

			std::cout << "pid for pmt " << pid << std::endl;

			PMTInit(_pat->pid_programid[pid], pid);


		}
	}
}
void Noovo::Demuxer::PATInit()
{
	if (_pat != NULL) {
		_pat.reset(new Noovo::PATTable);
	}
	else {
		_pat = std::make_shared<Noovo::PATTable>();
	}
	_pat->Parse(_metadata);

	_patready = _pat->IsReadable();

	for (auto it = _pat->pid_programid.begin(); it != _pat->pid_programid.end(); it++)
	{
		if (it->second != 0) {//program_id =0 is useless program without pmt table.
			filter.insert(std::make_pair(it->first, false));

		}

	}
	num_program = filter.size();


}
void Noovo::Demuxer::PMTInit(int pmt_program, int pid)
{
	std::shared_ptr<Noovo::PMTTable> tempt = std::make_shared<Noovo::PMTTable>(pmt_program);

	tempt->Parse(_metadata);

	_pmt.insert(std::make_pair(pid, tempt));

}

void Noovo::Demuxer::SDTInit()
{
	if (_sdt != NULL) {
		_sdt.reset(new SDTTable);
	}
	else {
		_sdt = std::make_shared<SDTTable>();
	}

	_sdt->Parse(_metadata);

	_sdtready = _sdt->IsReadable();

}
void Noovo::Demuxer::_init_tables()
{
	_dumpPATTable();

	_dumpPMTTable();

	_dumpSDTTable();
}
void Noovo::Demuxer::_dumpPMTTable() {

	for (auto it = Channel_Data.begin(); it != Channel_Data.end(); it++)
	{

		it->second.stream_info = _pmt[it->first]->stream_info;

	}

}
void Noovo::Demuxer::_dumpPATTable() {
	if (_patready) {
		for (auto it = _pat->pid_programid.begin(); it != _pat->pid_programid.end(); it++)
		{
			if (it->second != 0)//會掃出一個pid=16or0但program=0的資料要剔除
			{
				Noovo::ChannelDetail tempt;

				tempt.program_id = it->second;

				Channel_Data.insert(std::make_pair(it->first, tempt));
			}
		}
	}
}
void Noovo::Demuxer::_dumpSDTTable() {
	if (_sdtready) {
		for (auto it = Channel_Data.begin(); it != Channel_Data.end(); it++)
		{

			int id = it->second.program_id;

			it->second.service = _sdt->SI_Table[id];
		}
	}


}
std::vector<int>& Noovo::Demuxer::ReturnArrayPID() {

	int count = 0;

	_pids.clear();
	
	for (auto it = filter.begin(); it != filter.end(); ++it) {

		_pids.push_back(it->first);

		//LOG(_pids[count]);

		count++;
	}

	return _pids;

}