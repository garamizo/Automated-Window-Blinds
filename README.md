# Automated-Window-Blinds

Custom window blinds controller operated over the cloud.

[CAD Files](https://cad.onshape.com/documents/a6c06eb241c09c8feb21c497/w/6788825bff947651f59a3d86/e/f01a4c1e6d0f6e358718e8e2?renderMode=0&uiState=657a3d49c3782d42471a5744) available on Onshape.

<p float="left">
  <img src="images/perspective.png" alt="Module perspective view" height=250/>
  <img src="images/back.png" alt="Module perspective view" height=250/>
  <img src="images/front.png" alt="Module perspective view" height=250/>
</p>

**Components**: 
- ESP8266 Controller (ESP-12F NodeMcu D1),
- Stepper motor 28BYJ-48
- 
**Libraries**:
- IOT Library (Sinric Pro): Control device via cloud, compatible with Alexa
- Over The Air Update (ArduinoOTA): Update controller firmware over local Wi-Fi

### Principle of Operation 

https://github.com/garamizo/Automated-Window-Blinds/assets/8237486/454b9f97-6c7d-487f-a07e-cb1c5066af00

**Standard Manual Operation**: The pitch of the blind blades are adjusted by pulling one of two cords that hangs out of the blind body.
These cords are part of a winch mechanism, wrapping the cords around a pulley, rotating a worm gear, rotating a drum, pulling another set of strings that are attached to the blind blades.

**Automated Operation**: This custom controller module actuate the pulley in parallel to the original cord mechanism, allowing both manual and automated operation.
In addition, this design is compact enough to fit inside the window blind body, hidden from view.

https://github.com/garamizo/Automated-Window-Blinds/assets/8237486/26fe96e3-e9e2-4dbf-b783-912a90e7b5c0

### Novelty

- Split flexible gear: Makes a driven gear from a pulley with constrained volume
- Herrinbone gear on flexible assembly: Self-aligning Herringbone gears are fit to flexible and low tolerance assemblies
  


