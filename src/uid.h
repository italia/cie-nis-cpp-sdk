/** file uis.h
 *  @brief UID generator
 *
 *  This file contains an (really simple) helper class that generated unique identifier
 */

#ifndef UID_GUARD
#define UID_GUARD

#include <atomic>

class Utils 
{
public:
	/** 
	 * Generates an UID
	 * @return the UID generated
	 */
	static uint32_t getUid() { static std::atomic<std::uint32_t> uid { 0 }; return ++uid; }
};

#endif
