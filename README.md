# Solar Charger Controller

🎓 **Academic Hardware Final Project**  
*Developed as a final practical hardware project evaluating embedded solar power management systems.*

An embedded solar charge controller system designed using Arduino to manage solar panel voltage and optimize battery charging through multi-stage algorithms (Bulk, Absorption, Float).

---

## 🛠 Features
- **Multi-Stage Charging Algorithm:** Bulk, Absorption, and Float modes.
- **Real-Time LCD Display:** 16x2 I2C LCD showing battery status, voltage, and PWM duty cycle.
- **Visual Status Indicators:** 
  - 🟢 **Green LED:** Fully Charged
  - 🔴 **Red LED:** Charging Active
  - 🟡 **Yellow LED:** Low Panel Output / Night / Error
- **Serial Telemetry:** Live data logging via 9600 baud Serial output.

---

## 📐 Circuit Schematic & Simulation
The project was modeled and simulated using **Proteus ISIS**. 

> For the full academic report and detailed documentation, check [`Proje-Solar.Charger.Controller.pdf`](./Proje-Solar.Charger.Controller.pdf).

---

## 💻 Code & Schematics
- **Source Code:** [`sketch_jun14a.ino`](./sketch_jun14a.ino)
- **Proteus Simulation File:** [`Solar Charger Controller.pdsprj`](./Solar%20Charger%20Controller.pdsprj)
