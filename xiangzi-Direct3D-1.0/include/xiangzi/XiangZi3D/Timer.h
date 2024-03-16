
class Timer {
    microseconds duration;
    system_clock::time_point start;
    system_clock::time_point end;
    double spend_time;
    double least_time;
public:
    void Fps(double fps) {
        least_time = 1000.0 / fps;
    }
    void Start() {
        start = system_clock::now();
    }
    void End() {
        end = system_clock::now();
    }
    void Wait(){
        duration = duration_cast<microseconds>(end - start);
        
        spend_time = 1000 * (double(duration.count()) * microseconds::period::num / microseconds::period::den);
        if (spend_time < least_time) {
            //this_thread::sleep_for(microseconds( int(least_time - spend_time) ));
            Sleep(least_time - spend_time);
        }
    }
};