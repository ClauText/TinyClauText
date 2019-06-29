#ifndef ARRAY_STACK_H
#define ARRAY_STACK_H

#include <vector>
#define Array std::vector

namespace wiz {

	template <class T>
	class ArrayStack
	{
	public:
		void reserve(long long size)
		{
			p.reserve(size);
		}
	public:
		ArrayStack(const ArrayStack<T>& s)
			:p(s.p), count(s.count)
		{
			//
		}
		ArrayStack<T>& operator=(const ArrayStack<T>& s)
		{
			if (this == &s) { return *this; }
			//p.DoEmpty();
			p = s.p;
			count = s.count;
			return *this;
		}
		ArrayStack<T>& operator=(ArrayStack<T>&& s)
		{
			if (this == &s) { return *this; }
			p = std::move(s.p);
			count = s.count;

			s.p = Array<T>(1);
			s.count = 0;
			return *this;
		}
		ArrayStack(ArrayStack<T>&& s)
		{
			p = std::move(s.p);
			count = s.count;

			s.p = Array<T>(1);
			s.count = 0;
		}
	private:
		Array<T> p;
		int count;
	public:
		explicit ArrayStack()
			:count(0)
		{
			p = Array<T>();
		}

		explicit ArrayStack(const int size)
			: count(0)
		{
#ifdef STACKS_DEBUG
			wiz::checkUnderError(0, size, 1); // must size >= 1
#endif
			p = Array<T>(size);
		}
		virtual ~ArrayStack() {  } // added!! 2012.06.04
	public:
		// push
		void push(const T& data)
		{
			if (p.empty()) {
				p = Array<T>(1);
			}

			if (count >= p.size())
			{
				// changed 2012.06.04
				p.resize(p.size() * 2);
			}

			p[count] = data;
			count++;
		}
		void push(const T&& data)
		{
			if (p.empty()) {
				p = Array<T>(1);
			}

			if (count >= p.size())
			{
				// changed 2012.06.04
				p.resize(p.size() * 2);
			}

			p[count] = std::move(data);
			count++;
		}
		// pop
		T pop()
		{
			if (isEmpty())
			{
				throw "stack empty"; // StackEmptyError();
			}
			else
			{
				T temp = p[count - 1];
				//  p[count-1] = T();
				count--;
				return temp;
			}
		}
		void clear()
		{
			for (int i = 0; i < count; i++)
			{
				p[i] = T();
			}
			count = 0;
		}
		const T& Top()const { return p[count - 1]; }
		T& Top() { return p[count - 1]; }
		T getTop()const
		{
			if (isEmpty())
			{
				throw "stack empty"; // StackEmptyError();
			}
			else
			{
				return p[count - 1];
			}
		}
		void setTop(const T& val)
		{
			if (isEmpty())
			{
				throw "stack empty"; // StackEmptyError();
			}
			else
			{
				p[count - 1] = val;
			}
		}
		T& top()
		{
			if (isEmpty())
			{
				throw "stack empty"; // StackEmptyError();
			}
			else
			{
				return p[count - 1];
			}
		}
		const T& top() const
		{
			if (isEmpty())
			{
				throw "stack empty"; // StackEmptyError();
			}
			else
			{
				return p[count - 1];
			}
		}

		ArrayStack<T>& operator<<(const T& var)
		{
			push(var);

			return *this;
		}

		bool isEmpty() const { return count <= 0; }
		bool empty() const { return count <= 0; }
		Array<T> toArray()const {
			if (count <= 0) { return Array<T>(); }
			Array<T> temp(count);

			for (int i = 0; i < temp.size(); i++)
			{
				temp[i] = p[i];
			}

			return temp;
		}
		// int getMaxNum()const
		int size()const { return count; }
		int getNum()const
		{
			return count;
		}
		int getNumMax()const
		{
			return p.size();
		}
		T& operator[] (const int idx)
		{
			/// #define?  assert( idx is index 0 ~ count-1 )
			return p[idx];
		}
		const T& operator[](const int idx) const
		{
			return p[idx];
		}
	};
}

#endif
