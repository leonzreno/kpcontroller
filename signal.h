#ifndef SIGNAL_H
#define SIGNAL_H


class Executor;

class Signal
{
public:
    Signal(Executor* p);

private:
    void install_sig_handler(Executor* p);

};



#endif