#ifndef ROLA_LINGEX_CONDITIONABLE_HPP
#define ROLA_LINGEX_CONDITIONABLE_HPP

#include <functional>

namespace rola
{
	// forward declare
	class Condition_builder;

	/// <summary>
	/// Any condition action.
	/// </summary>
	class Conditional_action
	{
	public:
		/// <summary>
		/// Performs the else part of the if statement its chained to.
		/// </summary>
		/// <param name=""></param>
		virtual void Else(std::function<void()>) = 0;

		/// <summary>
		/// Prepares for the extra ElseIf condition, this requires the call to Then eventually.
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Condition_builder& ElseIf(std::function<bool()>) = 0;
		virtual Condition_builder& ElseIf(bool condition) = 0;
	};

	/// <summary>
	/// Any condition builder.
	/// </summary>
	class Condition_builder
	{
	public:
		/// <summary>
		/// Performs the action for the previous conditional control statment (If, ElseIf).
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Conditional_action& Then(std::function<void()>) = 0;

		/// <summary>
		/// Evaluates the specified condition with the previously chained condition using AND.
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Condition_builder& And(std::function<bool()>) = 0;
		virtual Condition_builder& And(bool condition) = 0;

		/// <summary>
		/// Evaluates the specified condition with the previously chained condition using AND NOT.
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Condition_builder& AndNot(std::function<bool()>) = 0;
		virtual Condition_builder& AndNot(bool condition) = 0;

		/// <summary>
		/// Evaluates the specified condition with the previously chained condition using OR.
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Condition_builder& Or(std::function<bool()>) = 0;
		virtual Condition_builder& Or(bool condition) = 0;

		/// <summary>
		/// Evaluates the specified condition with the previously chained condition using OR NOT.
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Condition_builder& OrNot(std::function<bool()>) = 0;
		virtual Condition_builder& OrNot(bool condition) = 0;

		/// <summary>
		/// Evaluates the specified condition with the previously chained condition using XOR.
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Condition_builder& Xor(std::function<bool()>) = 0;
		virtual Condition_builder& Xor(bool condition) = 0;

		/// <summary>
		/// Evaluates the specified condition with the previously chained condition using XNOR.
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		virtual Condition_builder& Xnor(std::function<bool()>) = 0;
		virtual Condition_builder& Xnor(bool condition) = 0;
	};

	/// <summary>
	/// if | then | else | elseif
	/// </summary>
	class Condition_express : public Conditional_action, public Condition_builder
	{
	private:
		std::function<bool()> condition_value_;
		bool stop_condition_evalution_;

	private:
		Condition_express(std::function<bool()> cond, bool stop_eval)
			: condition_value_(cond)
			, stop_condition_evalution_(stop_eval)
		{}

	public:
		Condition_express(std::function<bool()> cond)
			: Condition_express(cond, false)
		{}

		Condition_express(bool condition)
			: condition_value_([=]() {return condition; })
			, stop_condition_evalution_(false)
		{}

		Condition_express(Condition_express const& rhs)				= delete;
		Condition_express(Condition_express&& rhs)					= delete;
		Condition_express& operator=(Condition_express const& rhs)	= delete;
		Condition_express& operator=(Condition_express&& rhs)		= delete;

	public:
#pragma region Conditional_action implement

		void Else(std::function<void()> action) override
		{
			if (condition_value_() || stop_condition_evalution_) return;
			stop_condition_evalution_ = true;
			action();
		}

		Condition_builder& ElseIf(std::function<bool()> condition) override
		{
			condition_value_ = condition;
			return *this;
		}

		Condition_builder& ElseIf(bool condition) override
		{
			condition_value_ = [=]() {return condition; };
			return *this;
		}

#pragma endregion

#pragma region Condition_builder implement

		Conditional_action& Then(std::function<void()> action) override
		{
			if (!condition_value_() || stop_condition_evalution_) return *this;
			stop_condition_evalution_ = true;
			action();
			return *this;
		}

		Condition_builder& And(std::function<bool()> condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation && condition(); };
			return *this;
		}

		Condition_builder& And(bool condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation && condition; };
			return *this;
		}

		Condition_builder& AndNot(std::function<bool()> condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation && !condition(); };
			return *this;
		}

		Condition_builder& AndNot(bool condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation && !condition; };
			return *this;
		}

		Condition_builder& Or(std::function<bool()> condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation || condition(); };
			return *this;
		}

		Condition_builder& Or(bool condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation || condition; };
			return *this;
		}

		Condition_builder& OrNot(std::function<bool()> condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation || !condition(); };
			return *this;
		}

		Condition_builder& OrNot(bool condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation || !condition; };
			return *this;
		}

		Condition_builder& Xor(std::function<bool()> condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation ^ condition(); };
			return *this;
		}

		Condition_builder& Xor(bool condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return previous_evaluation ^ condition; };
			return *this;
		}

		Condition_builder& Xnor(std::function<bool()> condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return !(previous_evaluation ^ condition()); };
			return *this;
		}

		Condition_builder& Xnor(bool condition) override
		{
			bool previous_evaluation = condition_value_();
			condition_value_ = [=]() {return !(previous_evaluation ^ condition); };
			return *this;
		}

#pragma endregion
	};
} // namespace rola
#endif // !ROLA_LINGEX_CONDITIONABLE_HPP
