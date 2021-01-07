#ifndef ROLA_LINGEX_LOOPABLE_HPP
#define ROLA_LINGEX_LOOPABLE_HPP

#include <functional>

namespace rola
{
	/// <summary>
	/// Any early loop (e.g while).
	/// </summary>
	class Early_loop
	{
	public:
		/// <summary>
		/// Performs the Do statement after evaluating the previous looping statement.
		/// </summary>
		/// <param name=""></param>
		virtual void Do(std::function<void()>) = 0;
	};

	/// <summary>
	/// Any early loop builder (e.g while)
	/// </summary>
	class Early_loop_builder
	{
	public:
		/// <summary>
		/// Performs the Do statement after evaluating the previous looping statement.
		/// </summary>
		/// <param name=""></param>
		virtual void Do(std::function<void()>) = 0;

		/// <summary>
		/// Evaluates the specified condition to be used to break the looping statment lately (before the end of the loop).
        /// While-Do-LaterBreak
		/// </summary>
		/// <param name=""></param>
		virtual Early_loop& LaterBreak(std::function<bool()>) = 0;

		/// <summary>
		/// Evaluates the specified condition to be used to break the looping statment early (at the begining of the loop).
        /// While-EarlyBreak-Do
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Early_loop& EarlyBreak(std::function<bool()>) = 0;

		/// <summary>
		/// Evaluates the specified condition to be used to continue the looping statment lately (before the end of the loop).
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Early_loop& LaterContinue(std::function<bool()>) = 0;

		/// <summary>
		/// Evaluates the specified condition to be used to continue the looping statment early (at the begining of the loop).
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Early_loop& EarlyContinue(std::function<bool()>) = 0;
	};

	/// <summary>
	/// Any late loop (e.g Do-While).
	/// </summary>
	class Later_loop
	{
	public:
		/// <summary>
		/// Performs the while statement using the specifed condition after it has evaluated the previous chained Do statement.
		/// </summary>
		/// <param name=""></param>
		virtual void While(std::function<bool()>) = 0;
	};

	/// <summary>
	/// Any late loop builder (e.g Do-While)
	/// </summary>
	class Later_loop_builder
	{
	public:
		/// <summary>
		/// Performs the while statement using the specified condition statement after evaluating the previous Do statement.
		/// </summary>
		/// <param name=""></param>
		virtual void While(std::function<bool()>) = 0;

		/// <summary>
		/// Evaluates the specified condition to be used to break the looping statment lately (before the end of the loop).
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Later_loop& laterBreak(std::function<bool()>) = 0;

		/// <summary>
		/// Evaluates the specified condition to be used to break the looping statment early (at the begining of the loop).
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Later_loop& earlyBreak(std::function<bool()>) = 0;

		/// <summary>
		/// Evaluates the specified condition to be used to continue the looping statment lately (before the end of the loop).
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Later_loop& laterContinue(std::function<bool()>) = 0;

		/// <summary>
		/// Evaluates the specified condition to be used to continue the looping statment early (at the begining of the loop).
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Later_loop& earlyContinue(std::function<bool()>) = 0;
	};

	class Loopable
		: public Early_loop
		, public Early_loop_builder
		, public Later_loop
		, public Later_loop_builder
	{
		enum class LoopStoperLocation
		{
			BEGINING_OF_THE_LOOP,
			END_OF_THE_LOOP
		};

		enum class LoopStopers
		{
			BREAK,
			CONTINUE
		};

	private:
		std::function<bool()> condition_value_;
		std::function<void()> action_;

		LoopStoperLocation loop_stoper_location_;
		std::function<bool()> loop_stoper_cond_action_;
		LoopStopers loop_stoper_;
		bool loop_stoper_cond_;

	public:
		Loopable(std::function<bool()> cond)
			: condition_value_(cond)
		{}

		Loopable(std::function<void()> action)
			: action_(action)
		{}

		Loopable(Loopable const& rhs) = delete;
		Loopable(Loopable&& rhs) = delete;
		Loopable& operator=(Loopable const& rhs) = delete;
		Loopable& operator=(Loopable&& rhs) = delete;

	public:

		void Early_loop_builder::Do(std::function<void()> action)
		{
			while (condition_value_())
			{
				action();
			}
		}

		void Early_loop::Do(std::function<void()> action)
		{
			while (condition_value_())
			{
				if (loop_stoper_location_ == LoopStoperLocation::BEGINING_OF_THE_LOOP)
				{
					if (loop_stoper_cond_action_)
					{
						if (loop_stoper_cond_action_())
						{
							if (loop_stoper_ == LoopStopers::BREAK)
							{
								break;
							}
							continue;
						}
					}
					else
					{
						if (loop_stoper_cond_)
						{
							if (loop_stoper_ == LoopStopers::BREAK)
							{
								break;
							}
							continue;
						}
					}
				}
				action();

				if (loop_stoper_location_ == LoopStoperLocation::END_OF_THE_LOOP)
				{
					if (loop_stoper_cond_action_)
					{
						if (loop_stoper_cond_action_())
						{
							if (loop_stoper_ == LoopStopers::BREAK)
							{
								break;
							}
							continue;
						}
					}
					else
					{
						if (loop_stoper_cond_)
						{
							if (loop_stoper_ == LoopStopers::BREAK)
							{
								break;
							}
							continue;
						}
					}
				}
			}
		}

		Early_loop& Early_loop_builder::LaterBreak(std::function<bool()> cond)
		{
			loop_stoper_cond_action_ = cond;
			loop_stoper_location_ = LoopStoperLocation::END_OF_THE_LOOP;
			loop_stoper_ = LoopStopers::BREAK;
			return *this;
		}

		Early_loop& Early_loop_builder::EarlyBreak(std::function<bool()> cond)
		{
			loop_stoper_cond_action_ = cond;
			loop_stoper_location_ = LoopStoperLocation::BEGINING_OF_THE_LOOP;
			loop_stoper_ = LoopStopers::BREAK;
			return *this;
		}

		Early_loop& Early_loop_builder::LaterContinue(std::function<bool()> cond)
		{
			loop_stoper_cond_action_ = cond;
			loop_stoper_location_ = LoopStoperLocation::END_OF_THE_LOOP;
			loop_stoper_ = LoopStopers::CONTINUE;
			return *this;
		}

		Early_loop& Early_loop_builder::EarlyContinue(std::function<bool()> cond)
		{
			loop_stoper_cond_action_ = cond;
			loop_stoper_location_ = LoopStoperLocation::BEGINING_OF_THE_LOOP;
			loop_stoper_ = LoopStopers::CONTINUE;
			return *this;
		}

		void Later_loop_builder::While(std::function<bool()> cond)
		{
			do
			{
				action_();
			} while (cond());
		}

		void Later_loop::While(std::function<bool()> cond)
		{
			do
			{
				if (loop_stoper_location_ == LoopStoperLocation::BEGINING_OF_THE_LOOP)
				{
					if (loop_stoper_cond_action_)
					{
						if (loop_stoper_cond_action_())
						{
							if (loop_stoper_ == LoopStopers::BREAK)
							{
								break;
							}
							continue;
						}
					}
					else
					{
						if (loop_stoper_cond_)
						{
							if (loop_stoper_ == LoopStopers::BREAK)
							{
								break;
							}
							continue;
						}
					}
				}
				action_();

				if (loop_stoper_location_ == LoopStoperLocation::END_OF_THE_LOOP)
				{
					if (loop_stoper_cond_action_)
					{
						if (loop_stoper_cond_action_())
						{
							if (loop_stoper_ == LoopStopers::BREAK)
							{
								break;
							}
							continue;
						}
					}
					else
					{
						if (loop_stoper_cond_)
						{
							if (loop_stoper_ == LoopStopers::BREAK)
							{
								break;
							}
							continue;
						}
					}
				}
			} while (cond());
		}

		Later_loop& Later_loop_builder::laterBreak(std::function<bool()> cond)
		{
			loop_stoper_cond_action_ = cond;
			loop_stoper_location_ = LoopStoperLocation::END_OF_THE_LOOP;
			loop_stoper_ = LoopStopers::BREAK;
			return *this;
		}

		Later_loop& Later_loop_builder::earlyBreak(std::function<bool()> cond)
		{
			loop_stoper_cond_action_ = cond;
			loop_stoper_location_ = LoopStoperLocation::BEGINING_OF_THE_LOOP;
			loop_stoper_ = LoopStopers::BREAK;
			return *this;
		}

		Later_loop& Later_loop_builder::laterContinue(std::function<bool()> cond)
		{
			loop_stoper_cond_action_ = cond;
			loop_stoper_location_ = LoopStoperLocation::END_OF_THE_LOOP;
			loop_stoper_ = LoopStopers::CONTINUE;
			return *this;
		}

		Later_loop& Later_loop_builder::earlyContinue(std::function<bool()> cond)
		{
			loop_stoper_cond_action_ = cond;
			loop_stoper_location_ = LoopStoperLocation::BEGINING_OF_THE_LOOP;
			loop_stoper_ = LoopStopers::CONTINUE;
			return *this;
		}
	};
} // namespace rola
#endif // !ROLA_LINGEX_LOOPABLE_HPP
