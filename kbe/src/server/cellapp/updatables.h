// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_UPDATABLES_H
#define KBE_UPDATABLES_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"
#include "updatable.h"	
// #define NDEBUG
// windows include	
#if KBE_PLATFORM == PLATFORM_WIN32	
#else
// linux include
#endif

namespace KBEngine{

class Updatables
{
public:
	Updatables();
	~Updatables();

	void clear();

	bool add(Updatable* updatable);
	bool remove(Updatable* updatable);

	void update();

private:
	std::vector< std::map<uint32, Updatable*> > objects1_;
	std::vector< std::map<uint32, Updatable*> > objects2_;
	std::vector< std::map<uint32, Updatable*> > objects3_;
	std::vector< std::map<uint32, Updatable*> > objects4_;
	std::vector< std::map<uint32, Updatable*> > objects5_;
};

	class UpdateThread
	{
	public:
		UpdateThread(std::vector< std::map<uint32, Updatable*> > *objects_){this->objects_ = objects_;}
		void operator()(int x)
		{
			std::vector< std::map<uint32, Updatable*> >::iterator fpIter = (*objects_).begin();
			for (; fpIter != objects_->end(); ++fpIter)
			{
				std::map<uint32, Updatable*>& pools = (*fpIter);
				std::map<uint32, Updatable*>::iterator iter = pools.begin();
				for (; iter != pools.end();)
				{
					if (!iter->second->update())
					{
						pools.erase(iter++);
					}
					else
					{
						++iter;
					}
				}
			}
		}
	private:
		std::vector< std::map<uint32, Updatable*> > *objects_;
	};
	
}
#endif
