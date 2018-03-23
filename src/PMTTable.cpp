/* 
 * File name : Media
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#include "PMTTable.h"

static std::function<void(unsigned int,std::string)> delegate_pmt;

using namespace Noovo;

static std::string GetTypeName(uint8_t type)
{
  switch (type)
    {
    case 0x00:
      return "Reserved";
    case 0x01:
      return "ISO/IEC 11172 Video";
    case 0x02:
      return "ISO/IEC 13818-2 Video";
    case 0x03:
      return "ISO/IEC 11172 Audio";
    case 0x04:
      return "ISO/IEC 13818-3 Audio";
    case 0x05:
      return "ISO/IEC 13818-1 Private Section";
    case 0x06:
      return "ISO/IEC 13818-1 Private PES data packets";
    case 0x07:
      return "ISO/IEC 13522 MHEG";
    case 0x08:
      return "ISO/IEC 13818-1 Annex A DSM CC";
    case 0x09:
      return "H222.1";
    case 0x0A:
      return "ISO/IEC 13818-6 type A";
    case 0x0B:
      return "ISO/IEC 13818-6 type B";
    case 0x0C:
      return "ISO/IEC 13818-6 type C";
    case 0x0D:
      return "ISO/IEC 13818-6 type D";
    case 0x0E:
      return "ISO/IEC 13818-1 auxillary";
    case 0x11:
      return "MP3";
      //return "ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3 / AMD 1";
    case 0x1b:
      return "H.264";
      //return "AVC video stream as defined in ITU-T Rec. H.264 | ISO/IEC 14496-10 Video (h.264)";
    default:
      if (type < 0x80)
	return "ISO/IEC 13818-1 reserved";
      else
    return "User Private";
    }
}
static void DumpPMT(void* p_zero, dvbpsi_pmt_t* p_pmt)
{
    dvbpsi_pmt_es_t* p_es = p_pmt->p_first_es;
    while(p_es)
    {
    
    delegate_pmt(p_es->i_pid,GetTypeName(p_es->i_type));
    
    LOG("PID : "<<p_es->i_pid << "Type :"<<p_es->i_type);

    p_es = p_es->p_next;
    
    }
   
}
static void message_pmt(dvbpsi_t *handle, const dvbpsi_msg_level_t level, const char* msg)
{
    switch(level)
    {
        case DVBPSI_MSG_ERROR: fprintf(stderr, "Error: ");   break;
        case DVBPSI_MSG_WARN:  fprintf(stderr, "Warning: "); break;
        case DVBPSI_MSG_DEBUG: fprintf(stderr, "Debug: ");   break;
        default: /* do nothing */
            return;
    }
   
}
PMTTable::PMTTable():_pmt_programid(0){

     delegate_pmt = std::bind(&PMTTable::PushData,this,std::placeholders::_1,std::placeholders::_2);

}
PMTTable::PMTTable(int pmt_programid):_pmt_programid(pmt_programid)
{
    
     delegate_pmt = std::bind(&PMTTable::PushData,this,std::placeholders::_1,std::placeholders::_2);
}
PMTTable::~PMTTable()
{
  dvbpsi_pmt_detach(_pmt_handler);
  dvbpsi_delete(_pmt_handler);
}
void Noovo::PMTTable::Parse(uint8_t *metadata)
{        
  try{
    if(_pmt_programid==0)
        throw (0);
    _setHandler();
      
    _pushCallBack(metadata);
    
  }
  catch(std::string mes){

      std::cout<<(mes)<<std::endl;
  }
  catch(int id){

      std::cout<<"When inital should notice the program id first "<<std::endl;
  }
}
void Noovo::PMTTable::_setHandler()
{   

    
    _pmt_handler    = dvbpsi_new(&message_pmt, DVBPSI_MSG_ERROR);

    int *test; 

    //Use delegate to get callback value this test is useless or can use pointer of unorder_map to pass value may be can revised
 
    if (!dvbpsi_pmt_attach(_pmt_handler, _pmt_programid, DumpPMT, test)) {

        throw("Cannot new pmt decoder handler");
    
    }
    
  
}

void Noovo::PMTTable::_pushCallBack(uint8_t *metadata )
{ 
  
  dvbpsi_packet_push(_pmt_handler, metadata);
   
}
bool Noovo::PMTTable::IsReadable()
{
    return true;
}
void Noovo::PMTTable::PushData(unsigned int pid,std::string info)
{
    stream_info.push_back(StreamInfo(pid,info));
} 