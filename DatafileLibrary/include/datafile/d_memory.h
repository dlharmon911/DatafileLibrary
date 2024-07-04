#ifndef _DLH_MEMORY_H_
#define _DLH_MEMORY_H_

#include <memory>

namespace dlh
{
	using object_t = std::shared_ptr<void>;

	template<typename Type>
	std::shared_ptr<void> make_shared(Type* object)
	{
		return std::shared_ptr<void>(object);
	}

	template<typename Type>
	std::shared_ptr<void> make_shared(Type* object, void (*Deleter)(Type*))
	{
		return std::shared_ptr<void>((void*)object, Deleter);
	}
}


#endif // !_DLH_MEMORY_H_


