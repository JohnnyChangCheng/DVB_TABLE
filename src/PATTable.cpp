#include "PATTable.h"
static std::function<void(unsigned int, unsigned int)> delegate;
static void message_pat(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg)
{
	switch (level)
	{
	case DVBPSI_MSG_ERROR: fprintf(stderr, "Error: ");   break;
	case DVBPSI_MSG_WARN:  fprintf(stderr, "Warning: "); break;
	case DVBPSI_MSG_DEBUG: fprintf(stderr, "Debug: ");   break;
	default: /* do nothing */
		return;
	}

}



static void PATCallback(void* p_zero, dvbpsi_pat_t* p_pat)
{

	*((bool*)p_zero) = true;
	dvbpsi_pat_program_t* p_program = p_pat->p_first_program;

	while (p_program)
	{
		delegate(p_program->i_pid, p_program->i_number);
		LOG("PAT : pid " << p_program->i_pid << "channel :" << p_program->i_number);
		p_program = p_program->p_next;

	}
}

using namespace Noovo;

PATTable::PATTable()
{
	delegate = std::bind(&PATTable::PushData, this, std::placeholders::_1, std::placeholders::_2);
}
PATTable::~PATTable()
{
	dvbpsi_pat_detach(_pat_handler);

	dvbpsi_delete(_pat_handler);

}
void Noovo::PATTable::Parse(uint8_t *metadata)
{

	try {

		_setHandler();

		_pushCallBack(metadata);

		_readable = true;

	}
	catch (std::string mes) {

		std::cout << (mes) << std::endl;

	}


}
bool Noovo::PATTable::IsReadable()
{
	return _readable;
}
void Noovo::PATTable::_pushCallBack(uint8_t *metadata) {

	dvbpsi_packet_push(_pat_handler, metadata);
}

void Noovo::PATTable::_setHandler()
{
	_pat_handler = dvbpsi_new(&message_pat, DVBPSI_MSG_ERROR);

	if (_pat_handler == NULL) {

		throw("Cannot new PAT decoder handler");

	}


	//Use delegate to get callback value this test is useless or can use pointer of unorder_map to pass value

	if (!dvbpsi_pat_attach(_pat_handler, PATCallback, &_readable))

	{
		dvbpsi_delete(_pat_handler);
		_pat_handler = NULL;
		throw("Cannot attach PAT decoder.");
	}

}
void Noovo::PATTable::PushData(unsigned int pid, unsigned int programid)
{
	pid_programid.insert(std::make_pair(pid, programid));
}