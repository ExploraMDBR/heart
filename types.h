#define KEEP_LED 200

#define HEART_SENSOR_NOT_FOUND "MAX30105 was not found. Please check wiring/power. "
#define HEART_SENSOR_OK "Place your index finger on the sensor with steady pressure."

enum class Beat_State {
  NO,
  PRECENSE,
  BEAT
};
