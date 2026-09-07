# BLDC Motor Control System for RE-RASSOR Rover

This project focuses on the redesign and reimplementation of the motor system for the Florida Space Institute's RE-RASSOR (Research and Education Regolith Advance Surface Systems Operation Robot) rover[cite: 1]. The primary objective is to replace the rover's original, power-inefficient stepper motors with a lighter and highly efficient Brushless DC (BLDC) motor drivetrain[cite: 1]. The system operates with stepper-like control precision, maintaining functionality as a seamless drop-in replacement that requires no changes to the rover's existing higher-level software architecture[cite: 1].

## 👥 Project Stakeholders and Team
* **Sponsor:** Mike Conroy, Florida Space Institute[cite: 1]
* **Development Team:** Alex Fowler, Nicholas Bernhoft, Gabriel Pullam, Noah Fourari, Jake Weber, Kenneth Arias[cite: 1]

## 🛠️ Hardware Architecture
The upgraded hardware replaces drone-style motors and standard H-bridges with a high-resistance gimbal motor and a dedicated 3-phase BLDC driver to prevent thermal failure[cite: 1].
* **Microcontroller:** Arduino Mega 2560 Rev3 (handles low-level system control and PWM generation)[cite: 1].
* **Motor Driver:** SimpleFOC Mini v1 utilizing the DRV 8313 chip (operates efficiently between 8V and 35V)[cite: 1].
* **Motor:** 2208 Gimbal Brushless Motor, 90KV (features high phase resistance of 12.5Ω–16.4Ω to naturally limit current draw and minimize heat)[cite: 1].
* **Server/SBC:** Libre Computer "Le Potato" (receives Wi-Fi commands and relays them to the Arduino via UART)[cite: 1].

## 💻 Software & Communication Pipeline
Commands originate from the rover operator and travel sequentially down to the motors:
* The operator issues speed (levels 1-3), direction (FWD/REV/LEFT/RIGHT), and STOP commands via a Graphical User Interface (GUI)[cite: 1].
* The Le Potato receives these wireless commands over a local router and parses the data[cite: 1].
* Python serialization scripts convert the commands into bytearrays and send them to the Arduino via a USB-to-serial (UART) connection at a 115200 baud rate[cite: 1].
* The Arduino reads the serialized binary data and outputs highly precise Pulse Width Modulation (PWM) signals to the DRV 8313 driver[cite: 1].
* The driver energizes the 3-phase BLDC motor windings to produce smooth, stepper-like rotational movement[cite: 1].

## 🚀 Performance Metrics & Improvements
The transition to the BLDC motor system yielded significant operational improvements for the RE-RASSOR platform:
* **Weight Reduction:** The motor subsystem mass was reduced from 390 grams per motor to approximately 74 grams[cite: 1]. This equates to a total weight saving of roughly 1,264 grams across the four-wheel drivetrain[cite: 1].
* **Power Efficiency:** Total power draw was minimized to approximately 3.2 W per wheel (12.8 W total for the rover), achieving an estimated system efficiency of 92%[cite: 1].
* **Torque Output:** Combined with a custom 4:1 planetary gearbox, the system achieves an output torque of 0.65 Nm, successfully exceeding the original stepper motor's 0.59 Nm holding torque[cite: 1].
* **Thermal Stability:** The high-resistance gimbal motor naturally limits continuous current draw to a fraction of an Amp, keeping the driver chip temperatures safe (only rising about 10-15°C above ambient under load)[cite: 1].

## 🛡️ Safety and Fault Handling
* **Overcurrent Protection:** The driver actively monitors phase current and disables output transistors to stop power delivery if the rated threshold is exceeded[cite: 1].
* **Communication Watchdog:** If the Arduino stops receiving command updates from the Le Potato within a 300 ms timeout window, all PWM outputs are disabled and the motor safely coasts or brakes to a stop[cite: 1].
* **Thermal Monitoring:** The system is designed to reduce the PWM duty cycle or perform an emergency shutdown if safe operating temperatures are exceeded[cite: 1].

## 📄 Additional Documentation
For comprehensive hardware assembly instructions, bench testing procedures, mechanical gearbox integration details, and complete system analysis, please refer to the verbatim project documentation file: `BLDC- Final Design Document (9).pdf`[cite: 1].
