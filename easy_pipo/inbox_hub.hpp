#ifndef ROLA_EASY_PIPO_INBOX_HUB_HPP
#define ROLA_EASY_PIPO_INBOX_HUB_HPP

#include "inbox.hpp"

namespace rola
{
	/// <summary>
	/// a inbox manager
	/// </summary>
	class inbox_hub
	{
	public:
		inbox_hub() = default;

		inbox_hub(const inbox_hub&)				= delete;
		inbox_hub(inbox_hub&&)					= delete;
		inbox_hub& operator=(const inbox_hub&)	= delete;
		inbox_hub& operator=(inbox_hub&&)		= delete;

	public:
		~inbox_hub()
		{
			stop();
		}

	public:
		bool book_inbox(const std::string& box_name, inbox* box)
		{
			std::lock_guard<std::mutex> lg(m_);
			auto iter = inboxs_.find(box_name);
			if (iter != inboxs_.end()) // already exists
				return false;

			inboxs_[box_name] = box;
		}

		inbox* query_inbox(const std::string& box_name)
		{
			std::lock_guard<std::mutex> lg(m_);
			auto iter = inboxs_.find(box_name);
			return iter == inboxs_.end() ? nullptr : iter->second;
		}

		template <typename Msg>
		bool send_msg(const std::string& box_name, const Msg& msg)
		{
			inbox* box = query_inbox(box_name);
			if (box == nullptr)
				return false;

			box->send(msg);
			return true;
		}

		void stop()
		{
			for (auto& ib : inboxs_)
			{
				ib.second->stop();
			}
		}

	private:
		std::unordered_map<std::string, inbox*> inboxs_;
		std::mutex m_;
	};

	static inbox_hub& get_inboxes()
	{
		static inbox_hub ihub;
		return ihub;
	}
} // namespace rola

#endif // ROLA_EASY_PIPO_INBOX_HUB_HPP
