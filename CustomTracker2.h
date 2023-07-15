#include "esphome.h"

//State Machine modes:
#define UNITIALIZED 0
#define TRACKING 1
#define FLATENNING 2
#define WINDING 3
#define STANDBY 4

class SolarTracker : public PollingComponent {

private:
void initialize() {
    if( !id(connected) )
        return;

    ESP_LOGI("Tracker", "Tracker is Initialized");
    standby();
    // moveToFlatness();
    
}

void SetStatus(float status) {
    if(GetStatus() == status)
        return;

    auto call = id(trackerState).make_call();
    call.set_value(status);
    call.perform();
}

float GetStatus() {

    // ESP_LOGI("Tracker", attribute(next_noon).value);
    ESP_LOGI("log", "%s", id(sun_next_noon).state);         //Post a log text with the state
    return id(trackerState).state;
}
void moveToFlatness() {
    if( isFlat() )
    {
        if( GetStatus() == FLATENNING )
        {
            motorOff();
        }
        ESP_LOGI("Tracker", "Flat");
        // SetStatus(TRACKING);
        // ESP_LOGD("Tracker", "Tracking");
        return;
    }
    if(GetStatus() != FLATENNING){
        SetStatus(FLATENNING);
        ESP_LOGD("Tracker", "Going Flat");
        if(id(roll).state <= 0){
            id(relay_apus).turn_on();
            ESP_LOGI("Tracker","Se roteste spre Apus");
            delay(500);
            }             
        else{
            id(relay_rasarit).turn_on();
            ESP_LOGI("Tracker","Se roteste spre Rasarit");
            delay(500);
            }
    }
}
// void track() {
//     float sun = id(sunPosition);
//     if(isWindTooHigh()) {
//         SetStatus(WINDING);
//         return; 
//     }
//     if(id(sunPosition) >= 91 || id(sunPosition) <= -91)
//     {
//         ESP_LOGI("Tracker", "Sun out of reach");
//         moveToFlatness();
//         return;
//     }
    
//     if(id(sunPosition) >= 61.5 || id(sunPosition) <= -61.5) {
//         ESP_LOGI("Tracker", "Shifting to placaFinal to avoid shadows %f",id(placaFinal));
//         sun = id(placaFinal);
//     }

//     if(id(roll) >= 42.5 && sun > 42.5)
//     {
//         ESP_LOGI("Tracker", "Tracking reached max position");
//         motorOff();
//         return;
//     }
//     if(id(roll) <= -42.5 && sun < -42.5)
//     {
//         ESP_LOGI("Tracker", "Tracking reached min position");
//         motorOff();
//         return;
//     }

//     if(sun - id(roll) <= -1.5)
//     {
//         ESP_LOGI("Tracker", "Tracking Forward");
//         setMotorDirectionForward();  
//         motorOn();
//         return;   
//     }
//     if(sun - id(roll) >= 1.5)
//     {
//         ESP_LOGI("Tracker", "Tracking Reverse");
//         setMotorDirectionReverse();
//         motorOn();
//         return;   
//     }
//     motorOff();
// }
// void winding() {
//     if(isWindTooHigh())
//     {
//         SetStatus(WINDING);
//         ESP_LOGD("Tracker", "Winding");
//         moveToFlatness();
//         return;
//     }
//     else
//         SetStatus(TRACKING);
// }
bool isFlat(){
    return id(roll).state >= -3 && id(roll).state <= 3; 
}
// bool isWindTooHigh() {
//     return id(wind).state >= 40;
// }
// void motorOn() {
//     if(id(sentido_motor).state == 1) { // Reverse
//        if(id(fcReverse).state == 1) {
//            moveToFlatness();
//            return;
//        }
            
//     } else {
//        if(id(fcForward).state == 1) {
//            moveToFlatness();
//            return;
//        }
//    }
//     id(ligar_motor).turn_on();
// }
void motorOff() { 
    id(relay_rasarit).turn_off();
    id(relay_apus).turn_off();
}
// bool forward = true;
// void setMotorDirectionForward() {
//     if(!forward) {
//         motorOff();
//         delay(100);
//         forward = true;
//     }
    
//     id(sentido_motor).turn_off();
//     ESP_LOGV("Tracker", "Forwarded");
// }
// void setMotorDirectionReverse() {
//     if(forward) {
//         motorOff();
//         delay(100);
//         forward = false;
//     }
//     id(sentido_motor).turn_on();
//     ESP_LOGV("Tracker", "Reversed");
// }
void standby()
{
    SetStatus(STANDBY);
    ESP_LOGI("Standby", "Tracker at standby");
}
public:
    SolarTracker() : PollingComponent(100) { 
        SetStatus(UNITIALIZED);
    }
    void setup() override { }

    void update() override 
    {
        switch((int) GetStatus()) {
            case UNITIALIZED:       // zero
                initialize();
                break;
            // case TRACKING:          // 1
            //     track();
            //     break;
            case FLATENNING:        // 2
                moveToFlatness();
            //     break;
            // case WINDING:           // 3
            //     winding();
            //     break;
            case STANDBY:           // 4
                standby();
                break;
        }
    }
};