#ifndef CONCENTRATOR_H
#define CONCENTRATOR_H


extern bool concentrator_is_enabled;
extern volatile unsigned int concentrator_cycle;

void concentrator_start();
void concentrator_stop();
//void concentrator_run();

#endif // CONCENTRATOR_H
