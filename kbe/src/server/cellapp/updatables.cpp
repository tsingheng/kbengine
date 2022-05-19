// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#include "updatables.h"	

#include <thread>

#include "helper/profile.h"	
#include "network/channel.h"

namespace KBEngine{	


//-------------------------------------------------------------------------------------
Updatables::Updatables()
{
	base_index = 0;
}

//-------------------------------------------------------------------------------------
Updatables::~Updatables()
{
	clear();
}

//-------------------------------------------------------------------------------------
void Updatables::clear()
{
	objects_.clear();
}

//-------------------------------------------------------------------------------------
bool Updatables::add(Updatable* updatable)
{
	INFO_MSG("Updatables::add() begine");
	// 由于没有大量优先级需求，因此这里固定优先级数组
	std::string base_service = fmt::format("{}:{}", updatable->getChannel()->addr().ipAsString(), updatable->getChannel()->addr().port);
	DEBUG_MSG(fmt::format("base server {}", base_service));
	if (map_.find(base_service) == map_.end())
	{
		DEBUG_MSG(fmt::format("new base server {}", base_service));
		map_[base_service] = base_index++;
		DEBUG_MSG(fmt::format("base server {} index {}", base_service, map_[base_service]));
		objects_.push_back(std::map<uint32, Updatable*>());
	}

	// objects_存放的是要同步更新的对象容器,按优先级排序
	// 改造下按baseip排序,然后开多线程去同步

	//if (objects_.size() == 0)
	//{
	//	objects_.push_back(std::map<uint32, Updatable*>());
	//	objects_.push_back(std::map<uint32, Updatable*>());
	//}

	// KBE_ASSERT(updatable->updatePriority() < objects_.size());

	static uint32 idx = 1;
	std::map<uint32, Updatable*>& pools = objects_[map_[base_service]];

	// 防止重复
	while (pools.find(idx) != pools.end())
		++idx;

	pools[idx] = updatable;

	// 记录存储位置
	updatable->removeIdx = idx++;
	INFO_MSG("Updatables::add() compelete");
	return true;
}

//-------------------------------------------------------------------------------------
bool Updatables::remove(Updatable* updatable)
{
	INFO_MSG("Updatables::remove() begine");
	std::string base_service = fmt::format("{}:{}", updatable->getChannel()->addr().ipAsString(), updatable->getChannel()->addr().port);
	DEBUG_MSG(fmt::format("base server {}, index={}", base_service, map_[base_service]));
	//const intptr_t objects_index = ((intptr_t)updatable->getChannel())%5;
	std::map<uint32, Updatable*>& pools = objects_[map_[base_service]];
	pools.erase(updatable->removeIdx);
	updatable->removeIdx = -1;
	INFO_MSG("Updatables::remove() complete");
	return true;
}

	void doUpdateThread(std::map<uint32, Updatable*> *pools)
{
	DEBUG_MSG("doUpdateThread start");
	DEBUG_MSG(fmt::format("size of pool is {}", pools->size()));
	std::map<uint32, Updatable*>::iterator iter = pools->begin();
	for (; iter != pools->end();)
	{
		if (!iter->second->update())
		{
			pools->erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}
	
//-------------------------------------------------------------------------------------
void Updatables::update()
{
	//AUTO_SCOPED_PROFILE("callUpdates");

	INFO_MSG("Updatables::update() begine");
	if (objects_.empty())
	{
		INFO_MSG("objects_ is empty");
		return;
	}

	std::thread threads[objects_.size()];
	
	std::vector< std::map<uint32, Updatable*> >::iterator fpIter = objects_.begin();
	int i = 0;
	for (; fpIter != objects_.end(); ++fpIter)
	{
		std::map<uint32, Updatable*>& pools = (*fpIter);
		threads[i] = std::thread(doUpdateThread, &pools);
		i++;
	}

	for (auto& t : threads)
	{
		t.join();
	}

	//std::vector< std::thread* >::iterator thIter = threads.begin();
	//for(; thIter != threads.end(); ++thIter)
	//{
	//	(*thIter)->join();
	//}
	INFO_MSG("Updatables::update() compelete");
}

//-------------------------------------------------------------------------------------
}
