#pragma once

// std::apply를 C++11으로 구현

// 매개변수 3개일 때 예시
// gen_seq<3>은 seq<0, 1, 2>을 상속 받고 있음
//auto s = gen_seq<3>();
// gen_seq<3>
// : gen_seq<2, 2>
// : gen_seq<1, 1, 2>
// : gen_seq<0, 0, 1, 2>
// : seq<0, 1, 2>

template<int... Remains>
struct seq
{};

template<int N, int... Remains>
struct gen_seq : gen_seq<N-1, N-1, Remains...>
{ };

template<int... Remains>
struct gen_seq<0, Remains...> : seq<Remains...>
{ };

template<typename Ret, typename... Args>
void xapply(Ret(*func)(Args...), std::tuple<Args... >& tup)
{
	return xapply_helper(func, gen_seq<sizeof... (Args)>()/*매개변수 개수*/, tup);
}

template<typename F, typename... Args, int... ls>
void xapply_helper(F func, seq<ls...>, std::tuple<Args...>& tup)
{
	(func)(std::get<ls>(tup)...);
	// 매개변수 3개일 때 예시
	// std::get<ls>(tup)... == get<0>(tup), get<1>(tup), get<2>(tup) 이런식으로 풀어서 매개변수를 다 전달 가능.
}

template<typename T, typename Ret, typename... Args>
void xapply(T* obj, Ret(T::*func)(Args...), std::tuple<Args... >& tup)
{
	return xapply_helper(obj, func, gen_seq<sizeof... (Args)>(), tup);
}

template<typename T, typename F, typename... Args, int... ls>
void xapply_helper(T* obj, F func, seq<ls...>, std::tuple<Args...>& tup)
{
	(obj->*func)(std::get<ls>(tup)...);
}

class IJob
{
public:
	virtual void Execute() = 0;
};

// Functor 사용
template<typename Ret, typename... Args>
class FuncJob : public IJob
{
	using FuncType = Ret(*)(Args...);

public:
	FuncJob(FuncType func, Args... args)
		: _func(func)
		, _tuple(args...)
	{

	}

	virtual void Execute() override
	{
		//std::apply(_func, _tuple); // C++17이상부터 지원
		xapply(_func, _tuple);
	}

private:
	FuncType _func;
	std::tuple<Args...> _tuple;
};

template<typename T, typename Ret, typename... Args>
class MemberJob : public IJob
{
	using FuncType = Ret(T::*)(Args...);

public:
	MemberJob(T* obj, FuncType func, Args... args)
		: _obj(obj)
		, _func(func)
		, _tuple(args...)
	{

	}

	virtual void Execute() override
	{
		//std::apply(_func, _tuple); // C++17이상부터 지원
		xapply(_obj, _func, _tuple);
	}

private:
	T* _obj;
	FuncType _func;
	std::tuple<Args...> _tuple;
};

// 람다를 저장하고 실행할 새로운 Job 클래스를 정의합니다.
class LambdaJob : public IJob
{
public:
	// 생성자에서 실행할 람다(또는 모든 호출 가능한 객체)를 받습니다.
	LambdaJob(std::function<void()> job) : _job(job) {}

	// 가상 함수 Execute를 오버라이드하여 저장된 람다를 실행합니다.
	virtual void Execute() override
	{
		_job();
	}

private:
	std::function<void()> _job; // 람다를 저장하기 위한 가장 이상적인 타입
};

class JobQueue
{
public:
	void Push(shared_ptr<IJob> job)
	{
		WRITE_LOCK;
		_jobs.push(job);
	}

	shared_ptr<IJob> Pop()
	{
		WRITE_LOCK;
		if (_jobs.empty())
		{
			return nullptr;
		}

		shared_ptr<IJob> ret = _jobs.front();
		_jobs.pop();

		return ret;
	}

private:
	USE_LOCK;
	queue <shared_ptr<IJob>> _jobs;
};
