/* 
 * File name : DefaultLive
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#ifndef __DVBTTABLE_H__
#define __DVBTTABLE_H__
#include <stdint.h>
#define LOG(msg) \
    std::cout << __FILE__ << "(" << __LINE__ << "): " << msg << std::endl 
extern "C" {
#ifdef WIN32
	#define __attribute__(x)
	//typedef uint64_t ssize_t;
#endif
	#include <sys/types.h>
	#include "libdvbpsi/descriptor.h"
	#include "libdvbpsi/dvbpsi.h"
	#include "libdvbpsi/psi.h"
	#include "libdvbpsi/pat.h"
	#include "libdvbpsi/pmt.h"
	#include "libdvbpsi/sdt.h"
	
};
#include <string>
#include <vector>
#include <list>
namespace Noovo {
	class DvbtTable {
        public:
            virtual ~DvbtTable() {};
            virtual bool IsReadable() = 0;
            virtual void Parse(uint8_t *metadata) =0;
	};
};
#endif

