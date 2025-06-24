#pragma once

// std::apply�� C++11���� ����

// �Ű����� 3���� �� ����
// gen_seq<3>�� seq<0, 1, 2>�� ��� �ް� ����
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
	return xapply_helper(func, gen_seq<sizeof... (Args)>()/*�Ű����� ����*/, tup);
}

template<typename F, typename... Args, int... ls>
void xapply_helper(F func, seq<ls...>, std::tuple<Args...>& tup)
{
	(func)(std::get<ls>(tup)...);
	// �Ű����� 3���� �� ����
	// std::get<ls>(tup)... == get<0>(tup), get<1>(tup), get<2>(tup) �̷������� Ǯ� �Ű������� �� ���� ����.
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

// Functor ���
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
		//std::apply(_func, _tuple); // C++17�̻���� ����
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
		//std::apply(_func, _tuple); // C++17�̻���� ����
		xapply(_obj, _func, _tuple);
	}

private:
	T* _obj;
	FuncType _func;
	std::tuple<Args...> _tuple;
};

// ���ٸ� �����ϰ� ������ ���ο� Job Ŭ������ �����մϴ�.
class LambdaJob : public IJob
{
public:
	// �����ڿ��� ������ ����(�Ǵ� ��� ȣ�� ������ ��ü)�� �޽��ϴ�.
	LambdaJob(std::function<void()> job) : _job(job) {}

	// ���� �Լ� Execute�� �������̵��Ͽ� ����� ���ٸ� �����մϴ�.
	virtual void Execute() override
	{
		_job();
	}

private:
	std::function<void()> _job; // ���ٸ� �����ϱ� ���� ���� �̻����� Ÿ��
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
