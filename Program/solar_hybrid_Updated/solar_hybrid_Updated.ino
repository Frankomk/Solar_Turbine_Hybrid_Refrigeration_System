
#define GREEN_LED 13        // Battery healthy indicator
#define YELLOW_LED 2        // Wind turbine active indicator
#define BLUE_LED 9          // Fridge running indicator
#define RED_LED 11          // Low battery warning
#define ANIMATED_BUZZER 12  // Visual alert (flashing)
#define TURBINE_MOTOR 10    // Wind turbine motor

#define BATTERY_PIN A0      // POT #1 - Battery voltage 
#define SOLAR_PIN A3        // POT #2 - Solar current 
#define WIND_PIN A4         // POT #3 - Wind current 


int batRaw, solRaw, windRaw;
float batteryVoltage;
float solarCurrent;
float windCurrent;
float totalRenewablePower;

// ========== TIMING FOR FLASHING EFFECTS ==========
unsigned long lastRedBlink = 0;
unsigned long lastBuzzerBlink = 0;
unsigned long lastMotorPulse = 0;
bool redLedState = false;
bool buzzerState = false;

// ========== SETUP FUNCTION ==========
void setup() {
  // Initialize all output pins
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(ANIMATED_BUZZER, OUTPUT);
  pinMode(TURBINE_MOTOR, OUTPUT);
  
  // Set initial states
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);     // Fridge starts ON
  digitalWrite(RED_LED, LOW);
  digitalWrite(ANIMATED_BUZZER, LOW);
  digitalWrite(TURBINE_MOTOR, LOW);
}

// ========== MAIN LOOP ==========
void loop() {
  // ========== READ ALL SENSORS ==========
  batRaw = analogRead(BATTERY_PIN);
  solRaw = analogRead(SOLAR_PIN);
  windRaw = analogRead(WIND_PIN);
  
  // Convert raw values to actual units
  batteryVoltage = (batRaw / 1023.0) * 25.0;      
  solarCurrent = (solRaw / 1023.0) * 20.0;        
  windCurrent = (windRaw / 1023.0) * 20.0;       
  
  
  totalRenewablePower = (solarCurrent + windCurrent) * batteryVoltage;
  
  if (batteryVoltage > 12.0) {
    digitalWrite(GREEN_LED, HIGH);
  } else {
    digitalWrite(GREEN_LED, LOW);
  }
  
  // ========== YELLOW LED & MOTOR: Wind Turbine ==========
  // Turbine active when wind > 50% (windRaw > 512)
  if (windRaw > 512) {
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(TURBINE_MOTOR, HIGH);   // Motor spins = turbine generating
  } else {
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(TURBINE_MOTOR, LOW);
  }
  
  // ========== ALERT SYSTEM: Red LED + Animated Buzzer ==========
  
  // CASE 1: EMERGENCY - Battery below 11.5V
  if (batteryVoltage < 11.5) {
    
    // Flash Red LED every 300ms
    if (millis() - lastRedBlink > 300) {
      redLedState = !redLedState;
      digitalWrite(RED_LED, redLedState);
      lastRedBlink = millis();
    }
    
    // Flash Animated Buzzer every 200ms (faster = more urgent)
    if (millis() - lastBuzzerBlink > 200) {
      buzzerState = !buzzerState;
      digitalWrite(ANIMATED_BUZZER, buzzerState);
      lastBuzzerBlink = millis();
    }
    
    // Turn OFF fridge to protect battery
    digitalWrite(BLUE_LED, LOW);
  }
  
  // CASE 2: WARNING - Battery between 11.5V and 11.8V
  else if (batteryVoltage < 11.8) {
    
    // Red LED steady ON
    digitalWrite(RED_LED, HIGH);
    redLedState = true;
    
    // Animated Buzzer steady ON (visual warning)
    digitalWrite(ANIMATED_BUZZER, HIGH);
    buzzerState = true;
    
    // Fridge stays ON (but warning active)
    digitalWrite(BLUE_LED, HIGH);
  }
  
  // CASE 3: NORMAL - Battery above 11.8V
  else {
    
    // All alerts OFF
    digitalWrite(RED_LED, LOW);
    digitalWrite(ANIMATED_BUZZER, LOW);
    redLedState = false;
    buzzerState = false;
    
    // Fridge running normally
    digitalWrite(BLUE_LED, HIGH);
  }
  
  // Small delay to prevent excessive CPU usage
  delay(100);
}

