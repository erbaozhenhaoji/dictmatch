#ifndef PY_QUEUE_H
#define PY_QUEUE_H
#include <py_log.h>
#include <stdlib.h>


#define PY_QUEUE_FULL  1
#define PY_QUEUE_EMPTY 2

const int nDefaultQueueSize = 50;

template <class T>
class Queue {

public:

	// Constructor

	Queue(int initSize=nDefaultQueueSize)
	{
		if (initSize < 1)
			initSize = nDefaultQueueSize;
		mPool = new T[initSize];
		if (!mPool){
			py_log_write(PY_LOG_FATAL, "Queue::Queue() alloc failed\n");
			exit(1);
		}
		mFront    = 0;
		mRear     = 0;
		mCount    = 0;
		mPoolSize = initSize;
	}

	// Destructor
	~Queue()
	{
		if(mPool){
			delete [] mPool;
		}

		mFront    = 0;
		mRear     = 0;
		mCount    = 0;
		mPoolSize = 0;
	}

	// judge if queue is empry
	int IsEmpty(void)
	{
		return mFront == mRear;
		//return mCount == 0;
	}

	// judge if queue is full
	int IsFull(void) 
	{
		return (mRear+1) % mPoolSize == mFront;
		//return mCount == mPoolSize;
	}

	// the the queue length (number of elements)
	int Size(void) 
	{
		return mCount;
	}

	// append an element
	void Append(const T &item) 
	{
		if (mCount == mPoolSize) {
			py_log_write(PY_LOG_WARN, "Queue full, add new element failed\n");
		}
		mCount++;
		mPool[mRear] = item;
		mRear = (mRear + 1) % mPoolSize; 
	}

	// Pop the front element
	int Pop(T &data) 
	{
		if(mCount > 0) {
			data = mPool[mFront];
			mCount --;
			mFront = (mFront + 1) % mPoolSize; 
			return 0;
		}
		else{
			py_log_write(PY_LOG_WARN, "Queue empty, Can not pop element\n");
			return PY_QUEUE_EMPTY;
		}
	}

	// Read the front element
	int GetFront(T &data) 
	{
		if (mCount > 0){
			data = mPool[mFront];
			return 0;
		}
		else{
			py_log_write(PY_LOG_WARN, "Queue empty, Can not read element\n");
			return PY_QUEUE_EMPTY;
		}
		return data;
	}

	// Clear the queue
	void Clear() 
	{
		mFront = 0;
		mRear  = 0;
		mCount = 0;
	}

	
private:
	T*   mPool; 	// pointer to element array
	int  mPoolSize;  // queue size
	int  mFront;     // position of the first elment
	int  mRear;      // next position of the last element
	int  mCount;     // element number
};

#endif

