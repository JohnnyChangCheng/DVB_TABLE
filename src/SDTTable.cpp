#include "SDTTable.h"
extern "C" {
#ifdef GTEST
	char * Descriptor(const void *buf, size_t length) {
		return 0;
	}
#else
#include "iconv.h"
#endif // GTEST
#include "libdvbpsi/demux.h"
};
std::function<void(unsigned int, uint8_t*)> delegate_sdt;

using namespace Noovo;
static void message_sdt(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg)
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

static void DumpSDT(void* p_zero, dvbpsi_sdt_t* p_sdt)
{
	dvbpsi_sdt_service_t* p_service = p_sdt->p_first_service;

	while (p_service)
	{
		delegate_sdt(p_service->i_service_id, p_service->p_first_descriptor->p_data);


		p_service = p_service->p_next;

	}

}
static void SDTSubTable(dvbpsi_t *p_dvbpsi, uint8_t i_table_id, uint16_t i_extension,
	void * p_zero)
{
	if (i_table_id == 0x42)
	{
		*((bool*)p_zero) = true;

		if (!dvbpsi_sdt_attach(p_dvbpsi, i_table_id, i_extension, DumpSDT, p_zero))

		{
			throw("Cannot attach sdt decoder");
		}
	}
}
SDTTable::SDTTable() :_ready(new bool(0))
{
	delegate_sdt = std::bind(&SDTTable::PushData, this, std::placeholders::_1, std::placeholders::_2);
}
SDTTable::~SDTTable()
{
	dvbpsi_DetachDemux(_sdt_handler);
	dvbpsi_delete(_sdt_handler);
	delete _ready;
}
void Noovo::SDTTable::Parse(uint8_t *metadata)
{


	try {
		_setHandler();

		_pushCallBack(metadata);
	}
	catch (std::string mes) {

		std::cout << (mes) << std::endl;

	}

}
bool Noovo::SDTTable::IsReadable()
{
	return (*_ready);
}
void Noovo::SDTTable::_pushCallBack(uint8_t *metadata) {

	dvbpsi_packet_push(_sdt_handler, metadata);
}

void Noovo::SDTTable::_setHandler()
{
	_sdt_handler = dvbpsi_new(&message_sdt, DVBPSI_MSG_ERROR);

	if (_sdt_handler == NULL) {

		throw("Cannot new SDT decoder handler");

	}




	//Use delegate in static variable to get callback value this test is useless or can use pointer of unorder_map to pass value

	if (!dvbpsi_AttachDemux(_sdt_handler, SDTSubTable, _ready))
	{
		dvbpsi_delete(_sdt_handler);

		_sdt_handler = NULL;

		throw("Cannot new SDT decoder handler");

	}

}
void Noovo::SDTTable::PushData(unsigned int programid, uint8_t* descriptor_data)
{
	/*
	Table 86: Service descriptor
	Syntax                                       Number of bits                             Identifier
	service_descriptor(){
	descriptor_tag                                       8                                uimsbf
	descriptor_length                                    8                                uimsbf
	service_type                                         8                                uimsbf
	service_provider_name_length                         8                                uimsbf
	for (i=0;i<N;i++){
	char                                             8                                uimsbf
	}
	service_name_length                                  8                                uimsbf
	for (i=0;i<N;i++){
	char                                             8                                uimsbf
	}
	}
	*/
	//libdvpsi help us filter out the descriptor tag and descriptor length , so our data begin at service type

	unsigned char service_type = descriptor_data[0];

	int service_provider_name_length = descriptor_data[1];

	char *service_provider_name = new char[service_provider_name_length];

	for (int i = 0; i < service_provider_name_length; i++) {

		service_provider_name[i] = descriptor_data[2 + i];

	}

	int begin = service_provider_name_length + 1;

	int service_name_length = descriptor_data[begin + 1];

	char* service_name = new char[service_name_length];

	for (int i = 0; i < service_name_length; i++) {
		service_name[i] = descriptor_data[2 + begin + i];
	}
	char * conv = Descriptor((const void*)service_provider_name, service_provider_name_length);

	char * conv_2 = Descriptor((const void*)service_name, service_name_length);

	std::string ServiceProvider(conv);

	std::string ServiceName(conv_2);

	std::vector<std::string> tempt;

	tempt.push_back(ServiceProvider);

	tempt.push_back(ServiceName);

	SI_Table.insert(std::make_pair(programid, tempt));

	delete[] service_provider_name;
	delete[] service_name;

	LOG("Porgram ID : " << programid << "Service : " << ServiceName << "Servicer Provider :" << ServiceProvider);

}