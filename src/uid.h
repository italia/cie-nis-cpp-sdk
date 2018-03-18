#ifndef UID_GUARD
#define UID_GUARD

#include <atomic>

class Utils 
{
public:
	static uint32_t getUid() { static std::atomic<std::uint32_t> uid { 0 }; return ++uid; }
};

#endif
