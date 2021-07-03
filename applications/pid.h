

template <typename T>
class Pid {
public:
    Pid(T ref, T p, T i, T d, T max, T min):_ref(ref),_p(p),_i(i),_d(d),_max(max),_min(min){}
    ~Pid(){}
    T getRef(){return _ref;}
    T setRef(T ref){_ref = ref;}
    T calc(T feed){
      T out;
      T err = _ref - feed;

      _sum += err;
      if(_sum > 0)
          _sum = _sum > 100000 ? _max : _sum;
      else
          _sum = _sum < -100000 ? _min : _sum;

      out = _p*err + _i * _sum + _d * (_pre_err - err);
      _pre_err = err;

      out = out > _max ? _max : out;
      out = out < _min ? _min : out;
      return out;
    }

private:
    T _pre_err;;
    T _ref;;
    T _feed;;
    T _sum;;
    T _max;;
    T _min;
    T _p;;
    T _i;;
    T _d;;
};

