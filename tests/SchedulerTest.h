//---------------------------------------------------------------
//
// SchedulerTest.h
//

#pragma once

#include "common/Scheduler.h"
#include "common/generated/SpellIdEnums.h"

#include <chrono>
#include <memory>

namespace Tests {

//===============================================================================

using namespace std::chrono_literals;

class SpellCastHandler : public std::enable_shared_from_this<SpellCastHandler>
{
public:
	void OnSpellCast(Common::SpellIds ids)
	{
	}


	Common::MockScheduler m_scheduler;
};

//class SchedulerInternalHelper;
//struct SchedulerHelper : public std::enable_shared_from_this<SchedulerHelper>
//{
//
//	friend class SchedulerInternalHelper;
//
//	SchedulerHelper()
//	{
//		Run();
//	}
//
//	void Run()
//	{
//		while (isRunning)
//		{
//			scheduler.Process();
//		}
//	}
//
//	void PostToScheduler(const std::function<void()>& cb)
//	{
//		//m_helper->PostToScheduler(cb);
//	}
//
//	void PostToSchedulerInterval(const std::function<void()>& cb, const std::chrono::milliseconds& interval)
//	{
//		m_helper->PostToSchedulerInterval(cb, interval);
//	}
//
//	struct SchedulerInternalHelper : std::enable_shared_from_this<SchedulerInternalHelper>
//	{
//		SchedulerInternalHelper(SchedulerHelper* parent)
//			: m_parent(parent)
//		{
//
//		}
//		void PostToScheduler(const Common::Task& callback,
//			const std::optional<Common::TaskOptions>& options)
//		{
//			m_parent->scheduler.Post(shared_from_this(), callback,
//				options.value_or(Common::TaskOptions{}));
//		}
//
//		void PostToSchedulerInterval(const std::function<void()>& cb, const std::chrono::milliseconds& interval)
//		{
//
//			//m_parent->scheduler.Post(std::move(task));
//		}
//
//		SchedulerHelper* m_parent;
//	};
//
//	Common::Scheduler scheduler;
//	bool isRunning = true;
//	std::shared_ptr<SchedulerInternalHelper> m_helper = std::make_shared<SchedulerInternalHelper>(this);
//};

//===============================================================================

}
