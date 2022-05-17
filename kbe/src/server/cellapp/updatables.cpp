// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#include "updatables.h"	

#include <thread>

#include "helper/profile.h"	

namespace KBEngine{	


//-------------------------------------------------------------------------------------
Updatables::Updatables()
{
	
}

//-------------------------------------------------------------------------------------
Updatables::~Updatables()
{
	clear();
}

//-------------------------------------------------------------------------------------
void Updatables::clear()
{
	objects1_.clear();
	objects2_.clear();
	objects3_.clear();
	objects4_.clear();
	objects5_.clear();
}

//-------------------------------------------------------------------------------------
bool Updatables::add(Updatable* updatable)
{
	const intptr_t objects_index = ((intptr_t)updatable)%5 + 1;
	std::vector< std::map<uint32, Updatable*> > objects_;
	if (objects_index == 1)
		objects_ = objects1_;
	else if (objects_index == 2)
		objects_ = objects2_;
	else if (objects_index == 3)
		objects_ = objects3_;
	else if (objects_index == 4)
		objects_ = objects4_;
	else if (objects_index == 5)
		objects_ = objects5_;
	DEBUG_MSG(fmt::format("Cellapp::updatables::put {} into objects{}_", (intptr_t)updatable, objects_index));
	// 由于没有大量优先级需求，因此这里固定优先级数组
	if (objects_.size() == 0)
	{
		objects_.push_back(std::map<uint32, Updatable*>());
		objects_.push_back(std::map<uint32, Updatable*>());
	}

	KBE_ASSERT(updatable->updatePriority() < objects_.size());

	static uint32 idx = 1;
	std::map<uint32, Updatable*>& pools = objects_[updatable->updatePriority()];

	// 防止重复
	while (pools.find(idx) != pools.end())
		++idx;

	pools[idx] = updatable;

	// 记录存储位置
	updatable->removeIdx = idx++;

	return true;
}

//-------------------------------------------------------------------------------------
bool Updatables::remove(Updatable* updatable)
{
	const intptr_t objects_index = ((intptr_t)updatable)%5 + 1;
	std::vector< std::map<uint32, Updatable*> > objects_;
	if (objects_index == 1)
		objects_ = objects1_;
	else if (objects_index == 2)
		objects_ = objects2_;
	else if (objects_index == 3)
		objects_ = objects3_;
	else if (objects_index == 4)
		objects_ = objects4_;
	else if (objects_index == 5)
		objects_ = objects5_;
	std::map<uint32, Updatable*>& pools = objects_[updatable->updatePriority()];
	pools.erase(updatable->removeIdx);
	updatable->removeIdx = -1;
	return true;
}

//-------------------------------------------------------------------------------------
void Updatables::update()
{
	AUTO_SCOPED_PROFILE("callUpdates");

	std::thread t1(UpdateThread(&objects1_), 1);
	std::thread t2(UpdateThread(&objects2_), 1);
	std::thread t3(UpdateThread(&objects3_), 1);
	std::thread t4(UpdateThread(&objects4_), 1);
	std::thread t5(UpdateThread(&objects5_), 1);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
}

	
//-------------------------------------------------------------------------------------
}
