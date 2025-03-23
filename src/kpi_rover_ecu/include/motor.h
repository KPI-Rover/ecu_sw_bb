#ifndef MOTOR_H
#define MOTOR_H

class Motor {
   public:
    static constexpr int kMinRpm = 8000;
    static constexpr int kMaxRpm = 26500;

    Motor(int assigned_number, bool is_inverted);
    int MotorGo(int newRPM);
    int MotorStop() const;
    int GetEncoderCounter() const;

   private:
    static constexpr int kBrakeTime = 100000;  // 100 ms

    int motorNumber_;
    bool inverted_;
    double currentDutyCycle_;
    double GetDC(int entryRPM);
};

#endif  // MOTOR_H