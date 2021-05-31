#ifndef SLIDE_WINDOW_H
#define SLIDE_WINDOW_H
#include <stdint.h>
#include <string.h>
template <typename T>
class SlideWindowBase
{
public:
	SlideWindowBase(T* array, uint16_t size):
		MEM_LEN_(size),
		static_array_(array),
		back_ptr_(static_array_)
    {
		if (array == NULL) while (1);
		memset(static_array_, 0, MEM_LEN_);
	}
	
	bool push(const T& value)
	{

		if(!is_full){
		    *back_ptr_ = value;
            if(++back_ptr_ >= static_array_ + MEM_LEN_){
                is_full = true;
                back_ptr_ = static_array_ + MEM_LEN_ - 1;
            }
		}else{
		    for(int i = 0; i < MEM_LEN_ - 1; i++){
		        static_array_[i] = static_array_[i+1];
		    }
		    *back_ptr_ = value;
		}

		return true;
	}

	T getFilterValue()
	{
		if(!is_full){
		    return 0;
		}else{
		    double tmp = 0;
		    for(int i = 0; i < MEM_LEN_; i++){
                tmp += static_array_[i];
            }
		    return (T)(tmp / MEM_LEN_);
		}
	}
private:
    const uint16_t MEM_LEN_;
    T* const static_array_;
    T* back_ptr_;
    bool is_full{false};
};

template <typename T, uint16_t N = 0>
class SlideWindow : public SlideWindowBase<T>
{
public:
	SlideWindow(T* array, uint16_t size):
		SlideWindowBase<T>(array, size)
	{}
	
	SlideWindow():
		SlideWindowBase<T>(array_, N)
	{
		if(N == 0) while(1);
	}
	
private:
	T array_[N == 0 ? 1 : N];
};
#endif
//end of file
