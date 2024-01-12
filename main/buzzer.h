#ifndef BUZZER_
#define BUZZER_

#define BUZEER_PIN 19

extern char IsBuzzing;
extern char IsAlreadyBuzzing;

void initialize_buzzer();
void start_buzzer();
void stop_buzzer();
    
#endif