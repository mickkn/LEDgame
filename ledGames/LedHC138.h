class LedHC138
{

public:
  LedHC138(const int A0, const int A1, const int A2);
  void on(int LED);
private:  
  int _A0;
  int _A1;
  int _A2;
};

